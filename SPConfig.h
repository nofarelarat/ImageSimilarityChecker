#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "SPLogger.h"
#include "string.h"

/**
 * A data-structure which is used for configuring the system.
 */

typedef enum sp_config_msg_t
{
    SP_CONFIG_MISSING_DIR,
    SP_CONFIG_MISSING_PREFIX,
    SP_CONFIG_MISSING_SUFFIX,
    SP_CONFIG_MISSING_NUM_IMAGES,
    SP_CONFIG_CANNOT_OPEN_FILE,
    SP_CONFIG_ALLOC_FAIL,
    SP_CONFIG_INVALID_INTEGER,
    SP_CONFIG_INVALID_STRING,
    SP_CONFIG_INVALID_ARGUMENT,
    SP_CONFIG_INDEX_OUT_OF_RANGE,
    SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

typedef enum splitMethod
{
    RANDOM ,
    MAX_SPREAD,
    INCREMENTAL
} splitMethod;


typedef struct sp_config_t* SPConfig;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 *         contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
        int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothing is done.
 */
void spConfigDestroy(SPConfig config);

/*
 * This function will check if all the parameters in the configuration file were set
 * and if some default value is not set- it will assign it:
 *
 *
 * spPCADimension = 20
 * spNumOfFeatures = 100
 * spNumOfSimilarImages = 1
 * spLoggerLevel = 3
 * spPCAFilename = "pca.yml"
 * spExtractionMode = true
 * spMinimalGUI = false
 * spKDTreeSplitMethod = MAX_SPREAD
 * spLoggerFilename = "stdout"
 *
 */
SP_CONFIG_MSG spCheckDefaultVal(SPConfig conf);

/*
 * Assigns a value into a variable
 *
 * @param conf 			    - relevant conf file
 * @param variance			- the relevant variable
 * @param value_to_enter    - the relevant value
 * @param line 				- line number in conf file
 * @param file_name		    - filename of the conf file
 *
 * @return SP_CONFIG_SUCCESS for SUCCESS
 * SP_CONFIG_INVALID_STRING for failure
 */
SP_CONFIG_MSG spAssignVarValue(SPConfig conf, char *variance, char *value_to_enter, int line, const char* file_name);

/*
 * Searching for a string value (val_checked) in arr
 *
 * @param arr   		- Arr of string values
 * @param val_checked 	- Value to be checked
 * @param size  		- Arr size
 *
 * @return index of relevant value in case of success, or -1 otherwise
 */
int spFindValInArr(const char *arr[], char* val_checked, int size);


/**
 * Updates an integer value for config parameter
 *
 * @param val		   - the value to be updated
 * @param left_bottom  - minimum value
 * @param right_bottom - maximum value
 *
 * @return 0 for success, 1 otherwise
 */
int spUpdateValInt(char* val, int left_bottom, int right_bottom);

/**
 * This Function will return an Error messages in case that some parameter that does not have default
 * value is not properly set.
 * The message is in the following format:
 * 	<file name>
 * <number of problematic line>
 * "Parameter <parameter_name> is not set"
 *
 * @param conf 					- The relevant config file
 * @param file_name 			- Minimum value
 * @param param_to_check 		- The relevant parameter
 *
 */
void spPrintMissAttr(FILE* conf, const char* file_name, const char* param_to_check);

/**
 * check a line in the configuration file and set varReturn the be the relevant
 * variable (before the equal sign), and valueReturn (after the equal sign)
 *
 * @param line 			- Relevant line that need to be checked
 * @param conf_msg 		- Config message that the function will update in case of an error
 * @param is_ignore		 -Flag that said if the line should be ignored or not
 * @param before_sign 	- A string that will contain the name of the variable
 * @param after_sign 	- A string that will contain the value of the relevant variable
 *
 * @return
 *  - msg updated with the line reading status
 *    SP_CONFIG_SUCCESS - for success
 *    SP_CONFIG_INVALID_STRING - for failure
 *  - on success - the varReturn and valueReturn are updated correctly
 */

void spValidatekLine(char* line, SP_CONFIG_MSG* conf_msg, int* is_ignore,char** before_sign, char** after_sign);

/**
 * Returns the splitMethod that the user would like to use according to the config file
 *
 * @param conf - The relevant configuration file

 * @return positive integer in success (the split method that is defined in the file),negative integer otherwise(conf file is NULL)
 *
 */
splitMethod  spConfigGetspKDTreeSplitMethod(const SPConfig config);

/**
 * Returns the knn that is configured in the configuration file
 *
 * @param conf - The relevant configuration file
 *
 * @return positive integer in success (the spknn that is defined in the file),negative integer otherwise(conf file is NULL)
 *
 */
int spConfigGetKNN(const SPConfig conf);

/**
 * Returns the number of similar inmages that is configured in the configuration file
 *
 * @param conf - The relevant configuration file

 * @return positive integer in success (the number of similar images that is defined in the file),negative integer otherwise(conf file is NULL)
 *
 */
int spConfigGetspNumSimilarImages(const SPConfig conf);

/*
 * Update feature_path with the relevant path from the config file and returns
 *  a message that clarify whether we succeed or not.
 * @param feature_Path	- The pointer that needs to be updated.
 * @param conf 			- The relevant configuration file
 * @param index			- The index of the relevant feature.
 *
 * @return SP_CONFIG_SUCCESS - in case of success
 * 		  SP_CONFIG_INVALID_ARGUMENT- in case of feature_Path == NULL || conf == NULL
 * 		  SP_CONFIG_INDEX_OUT_OF_RANGE - in case that conf->spNumOfImages <= index
 */
SP_CONFIG_MSG spConfigGetImageFeatPath(char* feature_Path, const SPConfig conf, int index);

/*
 *  Update logger_name and logger_level  pointers accroding to the information in the config file
 *  and return  a message that clarify whether we succeed or not.
 *
 * @param conf 			- The relevant configuration file
 * @param logger_name	- the pointer that needs to be updated with logger name
 * @param logger_level	- A pointer that needs to be updated with the debug's logger level.
 *
 * @return :SP_CONFIG_SUCCESS - in case of success
 * 		   SP_CONFIG_INVALID_ARGUMENT- in case of conf == NULL
 */
SP_CONFIG_MSG spConfigGetLogger_Name_Level(const SPConfig conf, char** logger_name, SP_LOGGER_LEVEL* logger_level);
#endif /* SPCONFIG_H_ */
