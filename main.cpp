/*
 * main.cpp
 *
 *  Created on: Mar 2, 2017
 *      Author: Noy Pitterman
 */

#include "main_aux.h"

#define CLOSE_LOG       "Closing the log file"
#define EXIT            "Exiting...\n"

int main(int argc, char** argv)
{
	//Run All the tests!
	//spRunTest();
    SPConfig conf = spCreateConfigFromArgs(argc, argv);
    if(conf==NULL)
    {
        return 0;
    }//if
    if(spCreateLog(conf) == false)
    {
        spConfigDestroy(conf);
        printf(EXIT);
        return 0;
    }//if
    if(spIsImageExists(conf)== false)
    {
        spLoggerPrintDebug(CLOSE_LOG, __FILE__, __func__, __LINE__);
        spConfigDestroy(conf);
        printf(EXIT);
        return 0;
    }//if

    //var declaration
    SPPoint*** imagesFeat_Mat;                            // matrix of features
    SP_CONFIG_MSG conf_msg;                               // save config message
    int* n_features_arr;                                  // save number of features per image
    ImageProc proc(conf);
    SPKDTreeNode kd;
    bool flag;
    // getting number of images from config
    int nImages = spConfigGetNumOfImages(conf,&conf_msg);
    n_features_arr = (int*) malloc(sizeof(int)*nImages);
    imagesFeat_Mat = (SPPoint***) malloc(sizeof(SPPoint**)*nImages);

    if (n_features_arr == NULL || imagesFeat_Mat == NULL)
    {
        spDestroyAllFeaturesData(imagesFeat_Mat, n_features_arr, 0);
        spLoggerPrintError(FEATS_ALLOC_ERR, __FILE__, __func__, __LINE__);
        printf(PROCESS_ERR);
    }//if
    else
    {
        flag = true;
        //Extract features manual
        //In case we need the memory will be freed inside the function
        if(spConfigIsExtractionMode(conf, &conf_msg)==true)
        {
            flag = extractFeatsAllImages(conf, imagesFeat_Mat, &n_features_arr, proc);
        }//if

        // Extracting feature from file
        // In case we need the memory will be freed inside the function
        else
        {
            flag = spExtractFeaturesFromFiles(conf, imagesFeat_Mat, &n_features_arr);
        }//else

        if(flag==true)
        {
            splitMethod get_split_m = spConfigGetspKDTreeSplitMethod(conf);
            if(get_split_m  == -1)
            {
                printf(PROCESS_ERR);
            }//if
            else
            {
                kd = spCreateKDTree(imagesFeat_Mat, n_features_arr,nImages,get_split_m);
                spDestroyAllFeaturesData(imagesFeat_Mat, n_features_arr, nImages);
                if (kd != NULL)
                {
                    //Ask user Input in case of an error, the error will be printed inside the function
                    GetUserInput(proc, kd, conf);
                    spKDTreeDestroy(kd);
                }//if
            }//else
        }//if
        else //flag=false
        {
            printf(PROCESS_ERR);
        }//else
    }//else
    //print to log, free memory and exiting....
    spLoggerPrintDebug(CLOSE_LOG, __FILE__, __func__, __LINE__);
    spLoggerDestroy();
    spConfigDestroy(conf);
    printf(EXIT);
    return 0;
}//main
