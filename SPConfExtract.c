#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SPConfExtract.h"
//#include "SPConfig.h"
//#include "SPPoint.h"


void spDestroyFeatures(SPPoint** arr_feat, int number_feat)
{
    int j;
    if(!arr_feat)
    {
        for(j = 0; j < number_feat; j++)
        {
            spPointDestroy(arr_feat[j]);
        }//for
        free(arr_feat);
    }//if
}//spDestroyFeatures

void spDestroyAllFeaturesData(SPPoint*** featuresDB, int* acFeatures, int nImages)
{
    int j;
    for(j = 0; j < nImages; j++)
    {
        spDestroyFeatures(featuresDB[j], acFeatures[j]);
    }//for
    //free all images features and number of features
    free(featuresDB);
    free(acFeatures);
}//freeAllFeaturesData

/**
 * Writes single feature to .feats file
 *
 * @param feature - the features.
 * @param feats   - the file to extract the features to.
 * @return true if succeeded, otherwise false
 */
bool spExtractFeatureToFile(SPPoint* feature, FILE* feats)
{
    //var decleration
    int i;
    int dim;
    double cor;
    int numWrittenChars;

    dim = spPointGetDimension(feature);

    for(i = 0; i < dim; i++)
    {
        cor=spPointGetAxisCoor(feature, i);
        numWrittenChars = fprintf(feats, "%f" ",", cor);
        if(numWrittenChars < 0)
        {
            return false;
        }//if
    }//for
    //check if putting \n succeed
    if(fputc('\n', feats) == EOF)
    {
        return false;
    }//if
    return true;
}//spExtractFeatureToFile

/**
 * Writes features of image to file
 *
 * @param conf          - pointer to the config struct
 * @param img_Index     - the index of the image to be written
 * @param img_Features  - an array of points contains all image features
 * @param nFeatures     - the total number of features
 *
 */
void spstoreImageFeaturesToFile(SPConfig conf, int img_Index, SPPoint** img_Features, int nFeatures )
{
    //var deceleration
    SP_CONFIG_MSG msg;
    FILE* feats;
    char logmsg[MAX_STR + 100];
    bool flag = true;
    char fileName[MAX_STR + 1];
    spConfigGetImageFeatPath(fileName, conf, img_Index);

    feats = fopen(fileName, "w");
    if(feats != NULL)
    {
        int i;
        fprintf(feats, FIRST_LINE, nFeatures, spConfigGetPCADim(conf, &msg));
        for(i = 0; i < nFeatures && flag; i++)
        {
            flag = spExtractFeatureToFile(img_Features[i], feats);
        }//for
        fclose(feats);

        //in case of an error
        if(flag==false)
        {
            if(remove(fileName) == -1)
            {
                sprintf(logmsg, FAIL_W_FILE_ERR, img_Index, fileName);
            }//if
            else
            {
                sprintf(logmsg, FAIL_W_FILE_ERR, img_Index, fileName);
            }//else
            spLoggerPrintWarning(logmsg, __FILE__, __func__, __LINE__);
        }//if
        else
        {
            sprintf(logmsg, FINISH_WRITE_IMAGE_FEATS_INFO, img_Index, fileName);
            spLoggerPrintInfo(logmsg);
        }//else
    }//if
    else
    {//if feats == NULL
        sprintf(logmsg, FAIL_CREATE_FILE_ERR, img_Index, fileName);
        spLoggerPrintWarning(logmsg, __FILE__, __func__, __LINE__);
    }//else
}//spstoreImageFeaturesToFile


void spstoreImagesFeaturesToFiles(SPConfig config, SPPoint*** featuresDB ,int* acFeatures)
{
    //var deceleration
    int i;
    SP_CONFIG_MSG msg;
    int nImages = spConfigGetNumOfImages(config, &msg);

    for(i = 0; i < nImages; i++)
    {
        spstoreImageFeaturesToFile(config,i,featuresDB[i], acFeatures[i]);
    }//for
    spLoggerPrintInfo(FINISH_WRITE_FEATS_INFO);
}//spstoreImagesFeaturesToFiles

/**
 * Reads the features from  line in the .feats file.
 * Parsing the relevant line.
 *
 * @param feature_pointer - the feature to be parsed
 * @param featsFile - the file to be parsed
 * @param dim - the dimensions of the feature
 * @param img_index - the index of the image
 * @return true if succeeded, otherwise false
 */
bool spReadFeatureFromLine(SPPoint* feature_pointer, FILE* featsFile, int dim, int img_index)
{
    //var deceleration
    double* descriptors;
    bool flag = true;

    descriptors = (double*) calloc(sizeof(double) ,dim);
    if(descriptors != NULL)
    {
        int count_des= 0;

        while(count_des < dim && flag)
        {
            count_des++;
            flag = fscanf(featsFile, "%lf" ",", descriptors + count_des);
        }//while
        flag =(fgetc(featsFile) == '\n') &&  flag ;
        if(flag==true)
        {
            feature_pointer = spPointCreate(descriptors, dim, img_index);
            if(feature_pointer == NULL)
            {
                flag = false;
            }//if
        }//if
        free(descriptors);
    }//if
    else
    {//descriptor equals to NULL
        flag = false;
    }//if
    return flag;
}// readFeatureFromLine

/**
 * Extracting features of image at index <imageIndex> from .feats file
 *
 * @param conf - configuration struct (assuming config is not null)
 * @param img_index - the index of the image
 * @param img_features - array where the extracted features will be saves
 * @param numOfFeatures - pointer to the int that will save the number of extracted features
 * @return true if succeeded extracting features and false otherwise
 */
bool spExtractFeaturesFromFile(SPConfig conf, int img_index, SPPoint** img_features, int* numOfFeatures)
{
    //var decleration
    bool flag = true;
    char imageFeaturePath[MAX_STR];
    char logMsg[MAX_STR+ 50];
    FILE* feats;
    spConfigGetImageFeatPath(imageFeaturePath, conf, img_index);

    feats = fopen(imageFeaturePath, "r");
    if(feats!=NULL)
    {
        //var declaration
        SP_CONFIG_MSG msg;
        int currImageFeatsDim = 0;
        int feature_dim = spConfigGetPCADim(conf, &msg);

        if(fscanf(feats, FIRST_LINE, numOfFeatures, &currImageFeatsDim) != 2 || *numOfFeatures <= 0)
        {
            flag = false;
            //file is invalid since features dimensions are different from defined in config
            sprintf(logMsg, INVALID_LINE1_ERR,img_index,imageFeaturePath);
            spLoggerPrintError(logMsg, __FILE__, __func__, __LINE__);
        }//if
        else if(feature_dim == currImageFeatsDim)
        {
            img_features = (SPPoint**) malloc(sizeof(SPPoint*) * (*numOfFeatures));
            if(img_features == NULL)
            {
                spLoggerPrintError(ALLOC_FAIL_ERR, __FILE__, __func__, __LINE__);
                flag = false;
            }//if
            else
            {
                int i; //loop variable
                //reading features line by line
                for(i = 0; (i < *numOfFeatures) && flag; i++)
                {
                    flag = spReadFeatureFromLine(*(img_features + i), feats, feature_dim, img_index);
                }
                if(!flag)
                {
                    //failed calculating features, writing to logger and exiting
                    sprintf(logMsg, FAILED_R_FEAT_ERR , i,img_index, imageFeaturePath);
                    spLoggerPrintError(logMsg, __FILE__, __func__, __LINE__);
                    //free allocated memory
                    spDestroyFeatures(img_features, i);
                }//if
                else if(fgetc(feats) != EOF)
                {
                    flag = false;
                    //file is invalid since it contains more characters than expected
                    sprintf(logMsg, INVALID_FILE_EXT_CHARS, img_index,imageFeaturePath);
                    spLoggerPrintError(logMsg, __FILE__, __func__, __LINE__);
                    //free allocated memory
                    spDestroyFeatures(img_features, *numOfFeatures);
                }//else is
                else
                {
                    //succeeded calculating features, writing to logger
                    sprintf(logMsg, FINISHED_READ_IMAGE_FEATS, img_index, imageFeaturePath, *numOfFeatures);
                    spLoggerPrintInfo(logMsg);
                }//else
            }//else
        }//else if
        else
        {
            flag = false;
            //file is invalid since features dimensions are different from defined in config
            sprintf(logMsg, INVALID_FILE_DIMENSION_ERR, img_index, imageFeaturePath);
            spLoggerPrintError(logMsg, __FILE__, __func__, __LINE__);
        }//else
        fclose(feats);
    }//if
    else
    { //feats==NULL
        sprintf(logMsg, FAIL_OPEN_FEATS_ERR, img_index,imageFeaturePath);
        spLoggerPrintError(logMsg, __FILE__, __func__, __LINE__);
        flag = false;
    }//else
    return flag;
}//spExtractFeaturesFromFile


bool spExtractFeaturesFromFiles(SPConfig conf,SPPoint*** featuresDB, int** point_acfeatures)
{
    //var deceleration
    SP_CONFIG_MSG msg;
    int i;
    bool flag = true;
    int nImages = spConfigGetNumOfImages(conf, &msg);
    for (i = 0; (i < nImages) && flag; i++)
    {
        flag = spExtractFeaturesFromFile(conf, i,(*featuresDB + i), (*point_acfeatures + i));
    }//for
    if(flag==false)
    {
        *featuresDB = NULL;
        spDestroyAllFeaturesData(featuresDB, *point_acfeatures, i-1);
        *point_acfeatures = NULL;
    }//if
    spLoggerPrintInfo(FINISH_READ_FEATS_INFO);
    return flag;
}//spExtractFeaturesFromFiles
