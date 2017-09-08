/*
 * SPConfig.c
 *
 *  Created on: Mar 2, 2017
 *      Author: Noy Pitterman
 */

#include "SPConfig.h"


#define PCADIM_DEFAULT              20
#define PCANAME_DEFAULT             "pca.yml"
#define NFEATURES_DEFAULT           100
#define PCASIZE_DEAFULT             8
#define LOGFILESIZE_DEAFULT         7
#define LOGNAME_DEAFULT             "stdout"
#define MAX_STR_LEN                 1024
#define IGM_DEF						1
#define	KNN_DEF						1

#define IMGDIRECTORY                "spImagesDirectory"
#define IMGPREFIX                   "spImagesPrefix"
#define IMGSUFFIX                   "spImagesSuffix"
#define IMGNUMBER                   "spNumOfImages"
#define IMGNSIMILAR                 "spNumOfSimilarImages"
#define IMGPCADIM                   "spPCADimension"
#define IMGPCANAME                  "spPCAFilename"
#define IMGSPMETHOD                 "spKDTreeSplitMethod"
#define IMGLOGL                     "spLoggerLevel"
#define IMGLGFILE                   "spLoggerFilename"
#define POSS_SUFFIEXS               ".jpg", ".png", ".gif", ".bmp"
#define IMG_KNN                     "spKNN"
#define IMGNUMFE                    "spNumOfFeatures"
#define IMGEXMODE                   "spExtractionMode"
#define SPBIR                       "spcbir.config"
#define IMGMNGUI                    "spMinimalGUI"

/***********************
 * error messages MACROS
 ***********************/
#define INVALID_COMMAND_LINE        "Invalid command line : use -c <config_filename>\n"
#define CANTOPEN_DEAFULT            "The default configuration file spcbir.config couldn’'t be open"
#define CANTOPEN_NO_DEAFULT         "The configuration file %s couldn't be open\n"
#define INVALID_CONFIGURLINE        "File: %s\nLine: %d\nMessage: Invalid configuration line\n"
#define PARAMETER_NOTSETPROP        "File: %s\nLine: %d\nMessage: Parameter %s is not set\n"
#define CONSTRAINT_ERR              "File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n"

struct sp_config_t {
    char* spImagesDirectory;
    char* spImagesPrefix;
    char* spImagesSuffix;
    char* spPCAFilename;
    char* spLoggerFilename;
    int   spNumOfImages;
    int   spPCADimension;
    int   spNumOfFeatures;
    int   spNumSimilarImages;
    int   spKNN;
    int   spLoggerLevel;  // { 1 , 2 , 3  , 4}
    bool  spExtractionMode;
    bool  spMinimalGUI;
    splitMethod spKDTreeSplitMethod;
};

/*****************************
 * Help Functions Declarations (functions for internal use)
 *****************************/
/*
 *This Function allocates space to the new string and copy the copy_str to conf_str
 */
#define spStringAssign(str,copy_From) str = (char*) malloc(strlen(copy_From)+1); \
                                            if(!str){                         \
                                                return SP_CONFIG_ALLOC_FAIL;                \
                                            }                                           \
                                            strcpy(str,copy_From)
/*
 *This Function allocates space to the new string and copy the copy_str to conf_str
 */
#define spAssign(curr_field,value_to_assign) if(!conf->curr_field) conf->curr_field = value_to_assign;

/*
 *This Function will return the point that the string contains some char other than space
 */
#define spIgnoreSpaceConfig(str)             while(isspace(*str)){str++;}


SP_CONFIG_MSG spCheckDefaultVal(SPConfig conf)
{
	spAssign(spPCADimension, PCADIM_DEFAULT);
	spAssign(spNumOfFeatures, NFEATURES_DEFAULT);
	spAssign(spNumSimilarImages, IGM_DEF);

    spAssign(spKNN, KNN_DEF);

    if(!conf->spKDTreeSplitMethod)
    {
    	conf->spKDTreeSplitMethod = MAX_SPREAD;
    }
    if (!conf->spPCAFilename)
    {
        conf->spPCAFilename = (char*) malloc(PCASIZE_DEAFULT);
        if (!conf->spPCAFilename)
        {
            return SP_CONFIG_ALLOC_FAIL;
        }//if
        strcpy(conf->spPCAFilename, PCANAME_DEFAULT);
    }//if
    if (!conf->spLoggerFilename)
    {
        conf->spLoggerFilename = (char*) malloc(LOGFILESIZE_DEAFULT);
        if (!conf->spLoggerFilename)
        {
            return SP_CONFIG_ALLOC_FAIL;
        }//if
        strcpy(conf->spLoggerFilename,LOGNAME_DEAFULT);
    }//if

    if (conf->spLoggerLevel !=1 && conf->spLoggerLevel !=2 && conf->spLoggerLevel !=3 && conf->spLoggerLevel !=4)
    {
    	conf->spLoggerLevel = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
    }//if
    if (conf->spExtractionMode !=0 && conf->spExtractionMode !=1)
    {
    	conf->spExtractionMode = "true";
    }//if
    if (conf->spMinimalGUI !=0 && conf->spMinimalGUI!=1)
    {
            conf->spMinimalGUI = "false";
    }//if
    return SP_CONFIG_SUCCESS;
}//spCheckDefaultVal

/**
 * This function will gets a string and check if there is a space in the STR if so, return false, else return yes
 * param
 */
bool spIsSpace(char* str)
{
    int x;
    int len=strlen(str);
    for(x=0;x<len;x++)
    {
        if (isspace(str[x])==true)
        {
            return false;
        }//if
    }//for
    return true;
}//spIsSpace

int spUpdateValInt(char* val, int left_bottom, int right_bottom)

{
    int num_temp;
    char* str_copy= val;
    while (isdigit(*str_copy))
    {
        str_copy++;
    }//while
    if (*str_copy != '\0')
    {
        num_temp = 0;
    }//if
    else
    {
        num_temp = strtol(val, &str_copy, 10);
    }//else
    if(num_temp < left_bottom || num_temp> right_bottom )
    {
    	return 0;
    }//if
    return num_temp;
}//spUpdateValInt

int spFindValInArr(const char *arr[], char* val_checked, int size)
{
    int x;
    for (x = 0; x< size; x++)
    {
        if (!strcmp(arr[x], val_checked))
        {
            return x;
        }//if
    }//for
    return -1;
}//spFindValInArr

SP_CONFIG_MSG spAssignVarValue(SPConfig conf, char *variance, char *value_to_enter, int line, const char* file_name)
{
	//var deceleration
	int value_to_ass;

    //spImagesDirectory
    //Constraint: The string contains no spaces
    if (!strcmp(variance, IMGDIRECTORY))
    {
        if(spIsSpace(variance)==false ||spIsSpace(value_to_enter)==false)
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//if
        if(conf->spImagesDirectory!=NULL)
        {
        	free(conf->spImagesDirectory);
        }
        spStringAssign(conf->spImagesDirectory, value_to_enter);
        return SP_CONFIG_SUCCESS;
    }//if

    //spImagesPrefix
    //Constraint: The string contains no spaces
    if (!strcmp(variance, IMGPREFIX))
    {
        if(spIsSpace(variance)==false)
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//if
        if(conf->spImagesPrefix!=NULL)
        {
        	free(conf->spImagesPrefix);
        }
        spStringAssign(conf->spImagesPrefix, value_to_enter);
        return SP_CONFIG_SUCCESS;
    }//if

    //spImagesSuffix
    //Constraint: one of the following: .jpg, .png, .bmp, .gif
    if (!strcmp(variance, IMGSUFFIX))
    {
        const char *suffixes_arr[] = { POSS_SUFFIEXS };
        value_to_ass = spFindValInArr(suffixes_arr, value_to_enter, 4);
        if (value_to_ass != -1)
        {
            if(conf->spImagesSuffix!=NULL)
            {
            	free(conf->spImagesSuffix);
            }//if
        	spStringAssign(conf->spImagesSuffix, suffixes_arr[value_to_ass]);
            return SP_CONFIG_SUCCESS;
        }//if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//else
    }//if

    //spNumOfImages
    //Constraint: positive integers only
    if (!strcmp(variance, IMGNUMBER))
    {
    	value_to_ass = spUpdateValInt(value_to_enter, 1, INT_MAX);
        if (value_to_ass)
        {
            conf->spNumOfImages = value_to_ass;
            return SP_CONFIG_SUCCESS;
        }//if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//if

    //spPCADimension
    //Constraint: integers in range 10-28
    if (!strcmp(variance, IMGPCADIM))
    {
        int value_to_ass = spUpdateValInt(value_to_enter, 10, 28);
        if (value_to_ass)
        {
            conf->spPCADimension = value_to_ass;
            return SP_CONFIG_SUCCESS;
        }//if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//if

    //spPCAFilename
    //Constraint:the string contain no spaces
    if (!strcmp(variance, IMGPCANAME))
    {
        if(spIsSpace(variance)==false)
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//if
        if(conf->spPCAFilename!=NULL)
        {
        	free(conf->spPCAFilename);
        }
        spStringAssign(conf->spPCAFilename, value_to_enter);
        return SP_CONFIG_SUCCESS;
    }//if

    //spNumOfFeatures
    //Constraint: positive integers only
    if (!strcmp(variance, IMGNUMFE))
    {
    	value_to_ass = spUpdateValInt(value_to_enter, 1, INT_MAX);
        if (value_to_ass)
        {
            conf->spNumOfFeatures = value_to_ass;
            return SP_CONFIG_SUCCESS;
        }//if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//if

    //spExtractionMode
    //Constraint: one of the following: {true, false}
    if (!strcmp(variance, IMGEXMODE))
    {
        const char *boolean[] = { "true", "false" };
        value_to_ass = spFindValInArr(boolean, value_to_enter, 2);
        if (value_to_ass == 0)
        {
            conf->spExtractionMode = true;
            return SP_CONFIG_SUCCESS;
        }//if
        if (value_to_ass == 1)
        {
            conf->spExtractionMode = false;
            return SP_CONFIG_SUCCESS;
        }//if
        printf(CONSTRAINT_ERR, file_name, line);
        return SP_CONFIG_INVALID_STRING;
    }//if

    //spNumOfSimilarImages
    //Constraint: positive integers only
    if (!strcmp(variance, IMGNSIMILAR))
    {
        int val = spUpdateValInt(value_to_enter, 1, INT_MAX);
        if (val)
        {
            conf->spNumSimilarImages = val;
            return SP_CONFIG_SUCCESS;
        } //if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//if

    //spKDTreeSplitMethod
    //Constraint: one of the following :RANDOM, MAX_SPREAD,INCREMENTAL
    if (!strcmp(variance, IMGSPMETHOD))
    {
        const char *options_arr[] = {"RANDOM", "MAX_SPREAD", "INCREMENTAL"};
        value_to_ass =spFindValInArr(options_arr, value_to_enter, 3);
        switch (value_to_ass)
        {
        case 0:
            conf->spKDTreeSplitMethod = RANDOM;
            break;
        case 1:
            conf->spKDTreeSplitMethod = MAX_SPREAD;
            break;
        case 2:
            conf->spKDTreeSplitMethod = INCREMENTAL;
            break;
        default:
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//switch
        return SP_CONFIG_SUCCESS;
    }//if

    //spKNN
    //Constraint: spKNN>0
    if (!strcmp(variance, IMG_KNN))
    {
    	value_to_ass = spUpdateValInt(value_to_enter, 1, INT_MAX);
        if (value_to_ass>0)
        {
            conf->spKNN = value_to_ass;
            return SP_CONFIG_SUCCESS;
        } //if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//if

    //spMinimalGUI
    //Constraint: One of the following options: true, false
    if (!strcmp(variance, IMGMNGUI))
    {
        const char *boolean_arr[] = { "true", "false" };
        int val = spFindValInArr(boolean_arr, value_to_enter, 2);
        if (val == 0)
        {
            conf->spMinimalGUI = true;
            return SP_CONFIG_SUCCESS;
        }//if
        if (val == 1)
        {
            conf->spMinimalGUI = false;
            return SP_CONFIG_SUCCESS;
        }//if
        printf(CONSTRAINT_ERR, file_name, line);
        return SP_CONFIG_INVALID_STRING;
    }//if

    //spLoggerLevel
    //Constraint: Integers in range 1-4
    if (!strcmp(variance, IMGLOGL))
    {
    	value_to_ass = spUpdateValInt(value_to_enter, 1, 4);
        if (value_to_ass)
        {
            conf->spLoggerLevel = value_to_ass - 1; //first is 0 last is 3
            return SP_CONFIG_SUCCESS;
        }//if
        else
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_INTEGER;
        }//else
    }//sp

    //spLoggerFilename
    //Constraint: The string contains no spaces
    if (!strcmp(variance,IMGLGFILE))
    {
        if(spIsSpace(variance)==false || spIsSpace(value_to_enter)==false)
        {
            printf(CONSTRAINT_ERR, file_name, line);
            return SP_CONFIG_INVALID_STRING;
        }//if
        if(conf->spLoggerFilename!=NULL)
        {
        	free(conf->spLoggerFilename);
        }//if
        spStringAssign(conf->spLoggerFilename, value_to_enter);
        return SP_CONFIG_SUCCESS;
    }//if
    printf(INVALID_CONFIGURLINE, file_name, line);
    return SP_CONFIG_INVALID_STRING;
}//spAssignVarValue


void spPrintMissAttr(FILE* conf, const char* file_name, const char* param_to_check)
{
    //var deceleration
    char char_read;
    int lines = 1;
	if (file_name==NULL || conf==NULL || param_to_check==NULL)
    {
        return;
    }//if
    fseek(conf, SEEK_SET, 0);
    while (!feof(conf))
    {
    	char_read = fgetc(conf);
        if (char_read == '\n')
        {
            lines++;
        }//if
    }//while
    printf(PARAMETER_NOTSETPROP, file_name, lines, param_to_check);
}// spPrintMissAttr

/**
 * This function mean to be an helper function to spValidatekLine function-
 * This function will check if a string does not contain only spaces.
 *
 * @param conf_msg- message to be updated
 * @value - str to be checked
 * @return true in case that the str does not contain only space of false otherwise.
 */
bool not_contain_only_spaces (SP_CONFIG_MSG* conf_msg,char * value)
{
	int i;
	int len=strlen(value);
	for (i = 0; i < len; i++)
	{
		if (!isspace(*(value + i)))
	    {
			*conf_msg = SP_CONFIG_SUCCESS;
			return true;
	    }//if
	}//for
	return false;
}//not_contain_only_spaces

void spValidatekLine(char* line, SP_CONFIG_MSG* conf_msg, int* is_ignore,char** before_sign, char** after_sign)
{
    if (before_sign==NULL  || conf_msg==NULL || line==NULL || is_ignore == NULL|| after_sign == NULL )
    {
        return;
    }//if
    char *var_tmp;
    char *value_tmp;
    char *help_saver;
    char *str;
    //Stage A - ignore start line spaces if we have some
    spIgnoreSpaceConfig(line);
    //Stage B - check that we did not arrive to the end of the file
    if (*line == '\0' || *line == '#')
    {
        *conf_msg = SP_CONFIG_SUCCESS;
        *is_ignore = 1;
        return;
    }//if

    //Stage C - Saves the current line position
    var_tmp = line;
    value_tmp = line;

    //Stage D - Check if we did not ger an equal sign or end of str sign
    while (*value_tmp != '=' && *value_tmp != '\0')
    {
        if (isalpha(*value_tmp) || isspace(*value_tmp))
        {
            value_tmp++;
        }//if
        else
        {
            *conf_msg = SP_CONFIG_INVALID_STRING;
            return;
        }//else
    }//while

    //Stage E - check if this is the end of str sign
    if (*value_tmp == '\0')
    {
        *conf_msg = SP_CONFIG_INVALID_STRING;
        return;
    }//if

    //Stage F- the variable end here so we put /0 to declare this
    *value_tmp = '\0';
    value_tmp++;
    spIgnoreSpaceConfig(value_tmp);
    help_saver = var_tmp;

    //Stage G- Find the variable (what's right to the equal sign)
    while (*help_saver != '\0' && !isspace(*help_saver))
    {
    	help_saver++;
    }//while
    *help_saver = '\0';
    help_saver = value_tmp;
    while ( *help_saver != '\0' && !isspace(*help_saver))
    {
    	help_saver++;
    }//while
    if (*help_saver != '\0')
    {
    	str = help_saver;
        spIgnoreSpaceConfig(help_saver);
        //space were found inside the string and not at the end of the string
        if (*help_saver != '\0')
        {
            *conf_msg = SP_CONFIG_INVALID_STRING;
            return;
        }//if
        *str = '\0';
    }//if
    *before_sign = var_tmp;
    *after_sign = value_tmp;

    //Stage F- the variable end here so we put /0 to declare this
    bool no_space= not_contain_only_spaces (conf_msg,value_tmp);
    if (no_space==true)
    {
    	return;
    }//if
	*conf_msg=SP_CONFIG_INVALID_STRING;
}//spValidatekLine

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{

    assert(msg!=NULL);
    *msg = SP_CONFIG_SUCCESS;

    //var deceleration
    bool ErrorAccured;
    SPConfig conf ;
    int number_of_curr_line;
    int Comment;
    char line[MAX_STR_LEN];
    char *var;
    char *value;

    //Variables Initialize
    ErrorAccured = false;
    number_of_curr_line = 0;
    Comment= 0;
    conf = NULL;
    ErrorAccured = false;

    //checks that the file name is not null
    if(filename == NULL)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        printf(INVALID_COMMAND_LINE);
        return NULL;
    }//if

    FILE* config_file = fopen(filename,"r");
    //file did not open properly
    if (config_file==NULL)
    {
    	 //in case of default file
        if (!strcmp(filename, SPBIR))
        {

            printf(CANTOPEN_DEAFULT);
            *msg = SP_CONFIG_CANNOT_OPEN_FILE;
            return NULL;
        }//if
        //in case of not deafult
        else
        {
            printf(CANTOPEN_NO_DEAFULT, filename);
            *msg = SP_CONFIG_CANNOT_OPEN_FILE;
            return NULL;
        }//else
    }//if


    //config file creation
    conf = (SPConfig) malloc(sizeof(*conf));
    if (!conf)
    {
        *msg = SP_CONFIG_ALLOC_FAIL;
        ErrorAccured = true;
        fseek(config_file, SEEK_END, 0);
    }//if

    //Initialize with some data
    memset(conf, 0, sizeof(*conf));
    conf->spExtractionMode = -1;
    conf->spLoggerLevel = -1;

     if (spCheckDefaultVal(conf) != SP_CONFIG_SUCCESS)
     {
         ErrorAccured = true;
     }//if

    while (fgets(line, MAX_STR_LEN, config_file) != NULL)
    {
    	number_of_curr_line++;
        Comment = 0;

        //validates that there is no error in the line
        spValidatekLine(line, msg, &Comment, &var, &value);

        if (*msg != SP_CONFIG_SUCCESS)
        {
        	printf(INVALID_CONFIGURLINE, filename, number_of_curr_line);
            *msg = SP_CONFIG_INVALID_STRING;
            ErrorAccured = true;
            break;
        }//if
        //if the line is actual a comment that we should ignore it
        else if (Comment)
        {
            continue;
        }//elseif
        else //assign some value
        {
            *msg = spAssignVarValue(conf, var, value, number_of_curr_line, filename);
            if (*msg != SP_CONFIG_SUCCESS)
            {
                ErrorAccured = true;
                break;
            }//if
        }//else
    }//while

    // check if any other error occurred already
    if (!ErrorAccured)
    {
        if (!conf->spImagesDirectory)
        {
            *msg = SP_CONFIG_MISSING_DIR;
            spPrintMissAttr(config_file, filename, IMGDIRECTORY);
            ErrorAccured = true;
        }//if
        else if (!conf->spImagesPrefix)
        {
            *msg = SP_CONFIG_MISSING_PREFIX;
            spPrintMissAttr(config_file, filename, IMGPREFIX);
            ErrorAccured = true;
        }//elseif

        else if (!conf->spImagesSuffix)
        {
            *msg = SP_CONFIG_MISSING_SUFFIX;
            spPrintMissAttr(config_file, filename, IMGSUFFIX);
            ErrorAccured = true;
        } //elseif

        else if (!conf->spNumOfImages)
        {
            *msg = SP_CONFIG_MISSING_NUM_IMAGES;
            spPrintMissAttr(config_file, filename, IMGNUMBER);
            ErrorAccured = true;
        }//elseif
    }//if

    fclose(config_file);

    //check if any error occur until this point
    if (ErrorAccured==true)
    {
        spConfigDestroy(conf);
        return NULL;
    }//if
    return conf;
}//spConfigCreate

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
    assert(msg!=NULL);
    if(config == NULL)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }//if
    *msg = SP_CONFIG_SUCCESS;
    return config->spExtractionMode;
}//spConfigIsExtractionMode

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
    assert(msg!=NULL);
    if(NULL == config || NULL == msg)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }//if
    *msg = SP_CONFIG_SUCCESS;
    return config->spMinimalGUI;
}//spConfigMinimalGui

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
    assert(msg!=NULL);
    if(config == NULL)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }//if
    *(msg) = SP_CONFIG_SUCCESS;
    return config->spNumOfImages;
}//spConfigGetNumOfImages

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
    assert(msg!=NULL);
    if(config == NULL)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }//if
    *msg = SP_CONFIG_SUCCESS;
    return config->spNumOfFeatures;
}//spConfigGetNumOfFeatures

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
    assert(msg != NULL);
    if(config == NULL)
    {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return -1;
    }//if
    *msg = SP_CONFIG_SUCCESS;
    return config->spPCADimension;
}//spConfigGetPCADim

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,int index)
{
    if (!imagePath || !config)
    {
            return SP_CONFIG_INVALID_ARGUMENT;
    }//if
    if (index < 0 ||index >= config->spNumOfImages)
    {
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }//if
    sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
    config->spImagesPrefix, index, config->spImagesSuffix);
    return SP_CONFIG_SUCCESS;
}//spConfigGetImagePath

SP_CONFIG_MSG spConfigGetImageFeatPath(char* feature_Path, const SPConfig conf, int index)
{
    if (feature_Path == NULL || conf == NULL)
    {
            return SP_CONFIG_INVALID_ARGUMENT;
    }//if
    if (conf->spNumOfImages <= index)
    {
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }//if
    sprintf(feature_Path, "%s%s%d%s", conf->spImagesDirectory, conf->spImagesPrefix,index, ".feats");
    return SP_CONFIG_SUCCESS;
}//spConfigGetImageFeatPath

splitMethod  spConfigGetspKDTreeSplitMethod(SPConfig conf)
{
    if(conf == NULL)
    {
        return -1;
    }//if
    return conf->spKDTreeSplitMethod;
}// spConfigGetspKDTreeSplitMethod

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
    if (!pcaPath || !config)
    {
        return SP_CONFIG_INVALID_ARGUMENT;
    }//if
    sprintf(pcaPath, "%s%s", config->spImagesDirectory, config->spPCAFilename);
    return SP_CONFIG_SUCCESS;
}//spConfigGetPCAPath

int spConfigGetKNN(const SPConfig conf)
{
    if(conf==NULL)
    {
        return -1;
    }//if
    return conf->spKNN;
}//spConfigGetPCAPath

int spConfigGetspNumSimilarImages(const SPConfig conf)
{
    if(conf==NULL)
    {
       return -1;
    }//if
   return conf->spNumSimilarImages;
}//spConfigGetspNumSimilarImages

SP_CONFIG_MSG spConfigGetLogger_Name_Level(const SPConfig conf, char** logger_name, SP_LOGGER_LEVEL* logger_level)
{
    if (conf == NULL)
    {
        return SP_CONFIG_INVALID_ARGUMENT;
    }//if
    *logger_name = conf->spLoggerFilename;
    *logger_level = conf->spLoggerLevel;
    return SP_CONFIG_SUCCESS;
}//spConfigGetLogger_Name_Level

void spConfigDestroy(const SPConfig config)
{
    if (!config)
    {
        return;
    }//if
    free(config->spImagesDirectory);
    free(config->spImagesPrefix);
    free(config->spImagesSuffix);
    free(config->spLoggerFilename);
    free(config->spPCAFilename);
    free(config);
}//spConfigDestroy
