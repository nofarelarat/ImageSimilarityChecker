/*
 * main_aux.h
 *
 *  Created on: Mar 19, 2017
 *      Author: Noy Pitterman
 */

#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPImageProc.h"
extern "C"{
#include "SPConfExtract.h"
#include "SPConfig.h"
#include "KDTree.h"
#include "./unit_tests/main_test.h"
}
using namespace sp;

//Messages
#define NON_MINIMALGUI_OUT      "Best candidates for - %s - are:\n"
#define FINISH_FEATS            "Finished to calculate features"
#define FINISH_FEAT             "Finished to calculate features for image %s. Extracted %d features"
#define FEATS_ERR               "Failed to calculate features from images"
#define FEAT_ERR                "Failed to calculate features for image %s"
#define FEATS_ALLOC_ERR         "Failed to allocate feats array"
#define SIM_ALLOC_ERR           "Failed to allocate similar images array"
#define KNN_ALLOC_ERR           "Failed to allocate memory for knnIndex_arr of from hitCnt_arr: failed to find similar images for image number  %s"
#define CREATE_CONFIG_ERR       "Creating configuration file was failed, view log file for more information\n"
#define CREATE_LOGGER_SUCC      "Logger was created succesfully"
#define CREATE_LOGGER_ERR       "Logger was not created succesfully"
#define OPEN_LOGGER_ERR         "Error while opening log file %s\n"
#define IMG_DIR_ERR             "Error while checking if first image directory is accessible"
#define PROCESS_ERR             "Error occurred while pre-processing images, view log for more info\n"
#define BUILD_KD_INFO           "Started to build the kdTree"
#define INVALID_NUM_OF_IMG      "Failed To read number of Images from config"
#define INVALID_NUM_SIM_IMG     "Number of similar images is bigger then then number of images"
#define INVALID_PATH_OF_IMG     "Failed To read Image path from config"
#define INVALID_SIMILAR_IMG     "Failed To read number similar images from config"
#define INVALID_COMMANDLINE     "Invalid command line was given : use -c <config_file_name>\n"
#define INVALID_ARG_KNN         "Failed To read KNN arg from config"
#define FAILED_FINDSIM_ERR      "Failed to find the similar images for image number %s"
#define SIMILAR_FINISH_SUCC     "Successfully Finish finding similar images for query number %s"
#define MAX_STR_LEN             1024

//message for query part
#define QUERY_PATH              "Please enter an image path:\n"
#define QUERY_PROCESS_ERR               "Error occurred while preprocessing query, view log for more info\n"

/**
 * Display most similar images in Non minimal GUI state
 *
 * @param conf 			- Current config file
 * @param sim_Images	- Array of indexes of the similar images
 * @param nsim_Images 	- The number of similar images
 * @param path 			- The path of the query
 *
 */
void spDisplayNonMinimalGUI(SPConfig conf, int* sim_Images, int nsim_Images, char* path);


/**
 * find similar images for query feature
 *
 * @param kd                 - The kd tree to be used while the search
 * @param conf               - The config file
 * @param features_q         - Array of  points (features) of query
 * @param q_NumOfFeats       - The number of query features (actual points arr size)
 * @param similarImages      - Similar images arr
 * @param num_similar        - The number of similar images
 * @param path_q             - The path of the query
 *
 * @return true if succeeded false int the following cases:
 * -memory allocation erros
 * -errors in reading a parametwr from the config file
 */
bool spFindSimilarImages(SPKDTreeNode kd, SPConfig conf, SPPoint** features_q, int q_NumOfFeats, int* similarImages, int num_similar, char* path_q);

/**
 * Get arguments and creating conf file structure
 * argv[2]-contains FileName
 *
 * @return config struct if succeeded and NULL otherwise
 */
SPConfig spCreateConfigFromArgs(int argc, char** argv);

/**
 * Opening log file
 *
 * @param conf- current configuration file (we will assume not null)
 *
 * @return true if succeeded and false otherwise:
 *  -In case of logger message will be SP_LOGGER_CANNOT_OPEN_FILE
 *  -In case of logger message will be SP_LOGGER_OUT_OF_MEMORY
 */
bool spCreateLog(SPConfig conf);

/**
 * This function was created in order to confirm that the img directory that is listed in the configuration file exist and
 * actually contains the first img.
 *
 * @param conf - configuration File
 *
 * @return true if img directory is valid one, and false otherwise
 */
bool spIsImageExists(SPConfig conf);

/**
 * Creates kd Tree from a matrix of features
 *
 *
 * @param featuresDB - A matrix of points that contains feature per image (the i-th row holds all feature of the i-th image)
 * @param NFeatures  - An array that contains the number of features for each image
 * @param img_num    - The number of images to process
 * @param splitM     - The kdTree split method
 *
 *
 * @return pointer to the created kd Tree if succeeded
 *  otherwise return NULL
 */
SPKDTreeNode spCreateKDTree(SPPoint*** featuresDB,int* NFeatures, int img_num,  splitMethod splitM);

/**
 * Extracts images features from images
 * @param featuresDB       - Images features
 * @param point_acfeatures - Pointer to an integer array that will save the number of features for images
 * @paran imgProc		   - Image processor instance
 *
 * @return true if succeeded, false otherwise
*/

bool extractFeatsAllImages(SPConfig conf,SPPoint*** featuresDB, int** point_acfeatures, ImageProc Proc);

/**
 * Asks for input from the user and process it
 *
 * @param proc	 	- Image processor
 * @param conf 		- The relevant config file
 * @param kd 		- The relevant kdTree
 */
void GetUserInput(ImageProc proc, SPKDTreeNode kd, SPConfig conf);

#endif /* MAIN_AUX_H_ */
