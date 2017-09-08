/*
 * SPConfExtract.h
 *
 *  Created on: Aug 16, 2016
 *      Author: User
 */

#ifndef SPIMAGEUTILS_H_
#define SPIMAGEUTILS_H_

#include "SPPoint.h"
#include "SPConfig.h"
//#include "SPImageProc.h"

/* logger messages */
#define INVALID_FILE_DIMENSION_ERR          "Failed reading features for image %d (%s): the features dim are different from defined in config"
#define FINISH_WRITE_FEATS_INFO             "Finished writing to the files .feats"
#define FINISH_READ_FEATS_INFO              "Finished reading from the files .feats"
#define FAIL_CREATE_FILE_ERR                "Failed to create .feats file for the image %d (%s)"
#define FINISH_WRITE_IMAGE_FEATS_INFO       "Finished import features for image %d (%s)"
#define FAIL_W_FILE_ERR                     "Failed import features for image %d (%s)"
#define FAILED_R_FEAT_ERR                   "Failed to read feature %d for image %d (%s). please run again with extract mode"
#define INVALID_FILE_EXT_CHARS              "Failed to read features for image %d (%s): the file contains more chars than expected. please run again with extract mode"
#define FAIL_OPEN_FEATS_ERR                 "Failed to open .feats file for the image %d (%s)"
#define INVALID_LINE1_ERR                   "Failed reading features for image %d (%s): Invalid first line. please run again with extract mode"
#define ALLOC_FAIL_ERR                      "Allocation of image's Features failed"
#define FINISHED_READ_IMAGE_FEATS           "Finished reading features for image %d (%s). Extracted %d features"
#define MAX_STR                             1024
#define FIRST_LINE "%d %d\n"
//%d- num of features
//%d- dim
//both seperated by comma

/**
 * Free all features in our arr
 *
 * @param arr_feat - array of features to be freed
 * @paran number_feat - the number of features in array
 */
void spDestroyFeatures(SPPoint** arr_feat, int number_feat);

/**
 * Writes all images features data to files
 *
 * @param config     - conf file
 * @param featuresDB - contains all images features
 * @param acFeatures -contains all images actual number of features
 *
 */
void spstoreImagesFeaturesToFiles(SPConfig config, SPPoint*** featuresDB ,int* acFeatures);

/**
 * Extracts images features from .feats files
 *
 * @param featuresDB       -  images features
 * @param point_acfeatures - pointer to an integer array that will save the number of features for images
 *
 * @return true if succeeded, false otherwise
 */
bool spExtractFeaturesFromFiles(SPConfig conf,SPPoint*** featuresDB, int** point_acfeatures);


/**
 * Free function that is related to image features
 * @param featuresDB - array of all images features
 * @param acFeatures- array that contains actual number of features
 * @param nImages - number of images
 */
void spDestroyAllFeaturesData(SPPoint*** featuresDB, int* acFeatures, int nImages);
#endif /* SPIMAGEUTILS_H_ */
