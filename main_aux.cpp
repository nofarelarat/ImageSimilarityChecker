/*
 * main_aux.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: NofarE
 */

#include "main_aux.h"
extern "C"
{
	#include "SPPoint.h"
}//C

//this struct used as counter array
typedef struct hit_t
{
    int hitcnt; // counter of hits
    int index; // the index of the image

} temphits;


/**
 * Compare between 2 temp_hists;
 */
int comparator_Hits(const void *p, const void *q)
{

    //cast
    temphits*  pPointer = (temphits*)p;
    temphits*  qPointer = (temphits*)q;
	if (pPointer->hitcnt == qPointer->hitcnt)
	{
		return pPointer->index - qPointer->index;
	}//if
	return (qPointer->hitcnt - pPointer->hitcnt);
}//comparator_Hits

/**
 * Run over the similar images and show them
 *
 * @param conf - current config file
 * @param sim_Images - array of indexes of the similar images
 * @param nsim_Images - the number of similar images
 * @param Proc - image processor instance
*/
void spDisplayMinimalGUI(SPConfig conf, int* sim_Images, int nsim_Images, ImageProc Proc)
{
    //var deceleration
    char current_path[MAX_STR_LEN];
    int i;
    SP_CONFIG_MSG conf_msg;
    for(i = 0; i < nsim_Images; i++)
    {
        conf_msg = spConfigGetImagePath(current_path,conf,sim_Images[i]);
        if(conf_msg!=SP_CONFIG_SUCCESS)
        {
            spLoggerPrintError(INVALID_PATH_OF_IMG, __FILE__, __func__, __LINE__);
            return;
        }//if
        Proc.showImage(current_path);
    }//for
}//spDisplayMinimalGUI

void spDisplayNonMinimalGUI(SPConfig conf, int* sim_Images, int nsim_Images, char* path)
{
    //var deceleration
    int i;
    char curr_ImagePath[MAX_STR_LEN];
    printf(NON_MINIMALGUI_OUT,path);
    for (i = 0; i < nsim_Images; i++)
    {
        spConfigGetImagePath(curr_ImagePath,conf,sim_Images[i]);
        printf("%s\n", curr_ImagePath);
    }//for
}//spDisplayNonMinimalGUI

bool extractFeatsAllImages(SPConfig conf,SPPoint*** featuresDB, int** point_acfeatures, ImageProc Proc)
{
    //var deceleration
    int i;
    bool flag = true;
    SP_CONFIG_MSG conf_msg;

    int nImages = spConfigGetNumOfImages(conf,&conf_msg);
    if(conf_msg!=SP_CONFIG_SUCCESS)
    {
        spLoggerPrintError(INVALID_NUM_OF_IMG, __FILE__, __func__, __LINE__);
        return false;
    }//if

    for (i = 0; (i < nImages) && flag; i++)
    {
        char img_path[MAX_STR_LEN];
        char log_msg[MAX_STR_LEN + 70];
        conf_msg=spConfigGetImagePath(img_path, conf, i);
        if(conf_msg!=SP_CONFIG_SUCCESS)
        {
            spLoggerPrintError(INVALID_PATH_OF_IMG, __FILE__, __func__, __LINE__);
            flag=false;
        }//if
        featuresDB[i] = Proc.getImageFeatures(img_path, i, *point_acfeatures + i);
        if(featuresDB[i] != NULL)
        {
            sprintf(log_msg, FINISH_FEAT, img_path, (*point_acfeatures)[i]);
            spLoggerPrintInfo(log_msg);
        }//if
        else
        {
            flag = false;
            sprintf(log_msg, FEAT_ERR, img_path);
            spLoggerPrintError(log_msg, __FILE__, __func__, __LINE__);
        }//else
    }//for
    if(flag==true)
    {
        spLoggerPrintInfo(FINISH_FEATS);
        spstoreImagesFeaturesToFiles(conf, featuresDB, *point_acfeatures);
    }//if
    else
    {
        spDestroyAllFeaturesData(featuresDB, *point_acfeatures, i);
        spLoggerPrintError(FEATS_ERR , __FILE__, __func__, __LINE__);
    }//else
    return flag;
}//extractFeatsAllImages


bool spFindSimilarImages(SPKDTreeNode kd, SPConfig conf, SPPoint** features_q, int q_NumOfFeats, int* similarImages, int num_similar, char* path_q)
{
    //var decleration
    SP_CONFIG_MSG conf_msg;
    int numOfImages;
    temphits* hitCnt_arr;
    int* knnIndex_arr;
    int i;
    int j;
    int knn_size;
    bool flag = true;
    char log_Msg[MAX_STR_LEN + 70];

    // Read From Config File
    numOfImages = spConfigGetNumOfImages(conf, &conf_msg);
    knn_size = spConfigGetKNN(conf);

    //Does reading from config file succeed?
    if(conf_msg!=SP_CONFIG_SUCCESS)
    {
        spLoggerPrintError( INVALID_NUM_OF_IMG  , __FILE__, __func__, __LINE__);
        return false;
    }//if
    if(knn_size==-1)
    {

        spLoggerPrintError(INVALID_ARG_KNN, __FILE__, __func__, __LINE__);
        return false;
    }//if
    hitCnt_arr= (temphits*) calloc(sizeof(temphits),numOfImages);
    knnIndex_arr = (int*) calloc(sizeof(int),knn_size);
    if ( knnIndex_arr == NULL || hitCnt_arr == NULL )
    {
        free(hitCnt_arr);
        free(knnIndex_arr);
        sprintf(log_Msg, KNN_ALLOC_ERR, path_q);
        spLoggerPrintError(log_Msg, __FILE__, __func__, __LINE__);
        return false;
    }//if
    for (i = 0; i < numOfImages; i++)
    {
        hitCnt_arr[i].hitcnt = 0;
        hitCnt_arr[i].index = i;
    }//for
    for (i = 0; i< q_NumOfFeats&&flag; i++)
    {
        flag = spKDTreeKNearestNeighbors(kd,features_q[i], knn_size, knnIndex_arr);
        if(flag==true)
        {
            for (j = 0; j < knn_size; j++)
            {
                if (knnIndex_arr[j] != -1)
                {
                    hitCnt_arr[knnIndex_arr[j]].hitcnt = hitCnt_arr[knnIndex_arr[j]].hitcnt + 1;
                }//if
            }//for
        }//if
        else
        {
            sprintf(log_Msg, FAILED_FINDSIM_ERR, path_q);
            spLoggerPrintError(log_Msg, __FILE__, __func__, __LINE__);
            flag = false;
            break;
        }//else
    }//for
    if(flag==true)
    {
    	//sorting the array
        qsort(hitCnt_arr,numOfImages,sizeof(temphits), comparator_Hits);
        for (i = 0; i < num_similar; i++)
        {
            similarImages[i] = hitCnt_arr[i].index;
        }//for
    }//if
    else //flag==false
    {
        free(hitCnt_arr);
        free(knnIndex_arr);
        return false;
    }//else
    //free unused memory
    free(hitCnt_arr);
    free(knnIndex_arr);
    //print log message
    sprintf(log_Msg, SIMILAR_FINISH_SUCC, path_q);
    spLoggerPrintInfo(log_Msg);
    return true;
}//spFindSimilarImages

SPConfig spCreateConfigFromArgs(int argc, char** argv)
{
    //var deceleration
    SPConfig conf = NULL;
    SP_CONFIG_MSG config_Msg  = SP_CONFIG_SUCCESS;
    char* fileName;
    switch(argc)
    {
    case 1:
        //default config file
        conf = spConfigCreate("spcbir.config", &config_Msg);
        break;
    case 3:
        fileName = argv[2];
        //check is second char that was entered via Command Line was -c
        if(strcmp(argv[1], "-c") != 0)
        {
            printf(INVALID_COMMANDLINE);
            config_Msg = SP_CONFIG_CANNOT_OPEN_FILE;
            break;
        }//if
        conf = spConfigCreate(fileName, &config_Msg);
        break;
    default:
        printf(INVALID_COMMANDLINE);
        break;
    }//switch case
    //if we did not succeed to create config file
    if(config_Msg != SP_CONFIG_SUCCESS)
    {
        printf(CREATE_CONFIG_ERR);
    }//if
    return conf;
}//spCreateConfigFromArgs

bool spCreateLog(SPConfig conf)
{
    // var declaration
    SP_LOGGER_MSG log_Msg;
    char* logger_Name;
    bool flag = true;
    SP_LOGGER_LEVEL logger_Level;

    // Creating logger
    // Stage A-getting logger attributes - no need to the msg since we assume conf file is not a NULL
     spConfigGetLogger_Name_Level(conf,&logger_Name, &logger_Level);

    // StageB - if logger file name is "stdout" setting logger to be stdout -NEED TO CHANGE!!!!
     if(strcmp(logger_Name, "stdout") == 0 )
     {
         logger_Name = NULL;
     }//if

     log_Msg = spLoggerCreate(logger_Name,logger_Level);
    if (log_Msg  == SP_LOGGER_SUCCESS)
    {
        spLoggerPrintDebug(CREATE_LOGGER_SUCC, __FILE__, __func__, __LINE__);
    }//if

    //Stage C- if logger file name is not "stdout"
    else
    {
        flag = false;
        //Stage C.1 - Creation Fail?
        if (log_Msg == SP_LOGGER_CANNOT_OPEN_FILE)
        {
            printf(OPEN_LOGGER_ERR, logger_Name);
        }//if
        else if (log_Msg == SP_LOGGER_OUT_OF_MEMORY)
        {
            printf(CREATE_LOGGER_ERR);
        }//elseif
    }//else
    return flag;
}//spCreateLog

bool spIsImageExists(SPConfig conf)
{
    //var deceleration
    char Path[MAX_STR_LEN];
    //check if first images exists and if the path is valid
    spConfigGetImagePath(Path, conf, 0);
    FILE* image = fopen(Path, "r");
    if(image == NULL)
    {
        spLoggerPrintError(IMG_DIR_ERR, __FILE__, __func__, __LINE__);
        printf(PROCESS_ERR);
        return false;
    }//if
    //image exists
    fclose(image);
    return true;
}//spIsImageExists

SPKDTreeNode spCreateKDTree(SPPoint*** featuresDB,int* NFeatures, int img_num,  splitMethod splitM)
{
    if(featuresDB==NULL || NFeatures==NULL)
    {
        return NULL;
    }//if
    // var deceleration;
    int i;
    int j;
    int index;
    int allnum_Features_images = 0;
    SPPoint** featers;
    SPKDTreeNode kd;

    // Calc total number of feature of all images
    for (i = 0; i < img_num; i++)
    {
        allnum_Features_images = allnum_Features_images + NFeatures[i];
    }//for
    // allocating memory for all features
    featers = (SPPoint**) malloc (sizeof(SPPoint*) *  allnum_Features_images);
    if(featers == NULL)
    {
        spLoggerPrintError(FEATS_ALLOC_ERR, __FILE__, __func__, __LINE__);
        printf(PROCESS_ERR);
        return NULL;
    }
    index = 0;
    for (i = 0; i < img_num; i++)
    {
        for (j = 0; j < NFeatures[i]; j++)
        {
        	featers[index++] = featuresDB[i][j];
        }//for
    }//for

    kd = initKDTree(featers,allnum_Features_images,splitM);
    if (kd == NULL)
    {
        printf(PROCESS_ERR);
        free(featers);
        return NULL;
    }//if
    free(featers);
    return kd;
}//spCreateKDTree

void GetUserInput(ImageProc proc, SPKDTreeNode kd, SPConfig conf)
{
    //var deceleration
    int* sim_images;
    SP_CONFIG_MSG msg;
    int nsim_images;
    int actual_nImages;
    char q_path[MAX_STR_LEN];
    int actual_nFeats;
    static int q_index = 0;
    char log_msg[MAX_STR_LEN+70];

    actual_nImages = spConfigGetNumOfImages(conf,&msg);
    if(msg!=SP_CONFIG_SUCCESS)
    {
        spLoggerPrintError(INVALID_NUM_OF_IMG, __FILE__, __func__, __LINE__);
        return;
    }//if

    nsim_images = spConfigGetspNumSimilarImages(conf);
    if (nsim_images == -1)
    {
        spLoggerPrintError(INVALID_SIMILAR_IMG, __FILE__, __func__, __LINE__);
        return;
    }//if

    if (nsim_images > actual_nImages)
    {
        //numOfSimilarImages = numOfImages;
        spLoggerPrintError(INVALID_NUM_SIM_IMG, __FILE__, __func__, __LINE__);
        return;
    }//if

    sim_images = (int*) malloc(sizeof(int) * nsim_images);
    if (sim_images == NULL)
    {
        spLoggerPrintError(SIM_ALLOC_ERR, __FILE__, __func__, __LINE__);
        printf(QUERY_PROCESS_ERR);
        return;
    }//if
    //Ask the user to enter a path for query
    printf(QUERY_PATH);
    fflush(NULL);
    scanf("%s", q_path);
    while(strcmp(q_path, "<>"))
    {
        SPPoint** q_features = proc.getImageFeatures(q_path ,q_index++, &actual_nFeats);
        if(q_features == NULL)
        {
            sprintf(log_msg, FEAT_ERR, q_path);
            spLoggerPrintError(log_msg, __FILE__, __func__, __LINE__);
            printf(QUERY_PROCESS_ERR);
        }//if
        sprintf(log_msg, FINISH_FEAT, q_path, actual_nFeats);
        spLoggerPrintInfo(log_msg);
        if(spFindSimilarImages(kd,conf,q_features, actual_nFeats, sim_images, nsim_images, q_path)==false)
        {
            printf(QUERY_PROCESS_ERR);
        }//if
        else
        {
            SP_CONFIG_MSG msg;
            if (spConfigMinimalGui(conf, &msg)==true)
            {
                spDisplayMinimalGUI(conf, sim_images, nsim_images, proc);
            }//if
            else
            {
                spDisplayNonMinimalGUI(conf, sim_images, nsim_images, q_path);
            }//else
        }//else
        //Destroy Feature
        spDestroyFeatures(q_features, actual_nFeats);
        // asking for new input
        printf(QUERY_PATH);
        fflush(NULL);
        scanf("%s", q_path);
    }//while
    free(sim_images);
}//GetUserInput
