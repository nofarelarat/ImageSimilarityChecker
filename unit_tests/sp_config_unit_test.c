/*
 * sp_config_unit_test.c
 *
 *  Created on: Mar 21, 2017
 *      Author: NofarE
 */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "../SPConfig.h"
#include "unit_test_util.h"

/**
 * check the config file - compare the values with config file and check if ok
 *
 * @param conf 			- The pointer to the config file
 * @param img_path 		- The image valid path
 * @param index_img 	- The index image
 * @param num_feats 	- The number of features
 * @param num_img 		- The number of images
 * @param PCA_Path 		- The pca file path
 * @param PCA_Dim 		- The pca dimension
 * @param exc_mode 		- The extraction mode {true,false}
 * @param min_gui	    - If you want to use minimal GUI {true,false}
 * @param splitMethod s - The spliting method
 * @param knn_number 	- The knn num
 * @param sim_img 		- Num of similar images to find
 *
 * This is an helper function for all the rest of the function
 * @return true if config is as expected,else false
 */
static bool CheckWithConfigFile(SPConfig conf, char* img_path, int index_img, int num_feats, int num_img,
		int PCA_Dim, char* PCA_Path, bool exc_mode, bool min_gui, splitMethod s , int knn_number, int sim_img)
{
	SP_CONFIG_MSG conf_msg;
	char conf_img_path[1024];
	char conf_pca_path[1024];
	int tmp_comp;

	// Image path checking
	ASSERT_TRUE(spConfigGetImagePath(conf_img_path,conf,index_img) == SP_CONFIG_SUCCESS);
	tmp_comp = strcmp(conf_img_path, img_path);

	ASSERT_TRUE(tmp_comp == 0);

	// Number of features checking
	ASSERT_TRUE(spConfigGetNumOfFeatures(conf,&conf_msg) == num_feats);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);

	// Number of images checking
	ASSERT_TRUE(spConfigGetNumOfImages(conf, &conf_msg) == num_img);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);

	// Pca dimension checking
	ASSERT_TRUE(spConfigGetPCADim(conf,&conf_msg) == PCA_Dim);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);

	// Pca path checking
	ASSERT_TRUE(spConfigGetPCAPath(conf_pca_path,conf) == SP_CONFIG_SUCCESS);
	tmp_comp = strcmp(conf_pca_path, PCA_Path);
	ASSERT_TRUE(tmp_comp == 0);

	// Extraction Mode checking
	ASSERT_TRUE(spConfigIsExtractionMode(conf, &conf_msg) == exc_mode);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);

	// Minimal Gui checking
	ASSERT_TRUE(spConfigMinimalGui(conf, &conf_msg) == min_gui);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);

	// KDTree Split Method checking
	ASSERT_TRUE(spConfigGetspKDTreeSplitMethod(conf) == s);

	// KNN checking
	ASSERT_TRUE(spConfigGetKNN(conf) == knn_number);

	// Number of Similar Images Checking
	ASSERT_TRUE(spConfigGetspNumSimilarImages(conf) == sim_img);
	return true;
}//CheckWithConfigFile


static bool TextConfigFile1()
{
	//static bool CheckWithConfigFile(SPConfig conf, char* img_path,
	//int index_img, int num_feats, int num_img,
	//int PCA_Dim, char* PCA_Path, bool exc_mode, bool min_gui, splitMethod s , int knn_number, int sim_img)
	SP_CONFIG_MSG conf_msg;
	SPConfig conf = spConfigCreate("unit_tests/Configs/Valid1Test.config",&conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(conf != NULL);
	CheckWithConfigFile(conf,"./images1/imag0.png",0,50,25, 10, "./images1/pca2.yml", true, false,RANDOM,5,6);
	spConfigDestroy(conf);
	return true;
}//TextConfigFile1

/**
 * Check validation of config with Valid2Test.config
 */
static bool TextConfigFile2()
{

	SP_CONFIG_MSG conf_msg;
	SPConfig conf = spConfigCreate("unit_tests/Configs/Valid2Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(conf != NULL);
	CheckWithConfigFile(conf,"./images1/imag1.png",1,20,2, 28, "./images1/pca2.yml", false, true,INCREMENTAL,5,5);
	spConfigDestroy(conf);

	return true;
}//TextConfigFile2

static bool TextConfigFile3()
{
	SP_CONFIG_MSG conf_msg;
	SPConfig conf = spConfigCreate("unit_tests/Configs/Valid3Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(conf != NULL);
	CheckWithConfigFile(conf,"./images1/imag2.png",2,111,4, 28, "./images1/pca3.yml", false, true,MAX_SPREAD,9,8);
	spConfigDestroy(conf);

	return true;
}//TextConfigFile3

static bool checkErr()
{
	SP_CONFIG_MSG conf_msg;
	//Case A- file name is NULL
	SPConfig conf = spConfigCreate(NULL, &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(conf == NULL);
	//Case B- file not exists
	conf = spConfigCreate("noy", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_CANNOT_OPEN_FILE);
	ASSERT_TRUE(conf == NULL);
	conf = spConfigCreate("boll", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_CANNOT_OPEN_FILE);
	return true;
}//checkErr

static bool defaultcheck()
{
	SP_CONFIG_MSG conf_msg;
	SPConfig conf = spConfigCreate("unit_tests/Configs/DefaultCheck.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(conf != NULL);
	CheckWithConfigFile(conf,"./images/imag1.png",1,100,27, 20, "./images/pca.yml", true, false,MAX_SPREAD,1,1);
	if(conf != NULL)
	{
		spConfigDestroy(conf);
	}//if
	return true;
}//checkDefaultValues

static bool checkLineErr()
{
	SP_CONFIG_MSG conf_msg;
	SPConfig conf;

	//line no has '='
	conf= spConfigCreate("unit_tests/Configs/Invalid1Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//line as space
	conf = spConfigCreate("unit_tests/Configs/Invalid2Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//line has no spLoggerFilename value
	conf = spConfigCreate("unit_tests/Configs/Invalid3Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//line has no spImagesPrefix
	conf = spConfigCreate("unit_tests/Configs/Invalid4Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//directive not exists
	conf = spConfigCreate("unit_tests/Configs/Invalid5Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//line has no spSplitMethod
	conf = spConfigCreate("unit_tests/Configs/Invalid6Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	//line has no spLoggerFileName
	conf = spConfigCreate("unit_tests/Configs/Invalid7Test.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	return true;
}//checkLineErr

/**
 * Check invalid config parameters
 */
static bool checkParametersErr()
{
	SP_CONFIG_MSG conf_msg;
	SPConfig conf;

	// non numeric value
	conf = spConfigCreate("unit_tests/Configs/test1.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// double instead of int
	conf = spConfigCreate("unit_tests/Configs/test2.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// negative value
	conf = spConfigCreate("unit_tests/Configs/test3.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// int with spaces
	conf = spConfigCreate("unit_tests/Configs/test4.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	// value with spaces
	conf = spConfigCreate("unit_tests/Configs/test5.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	// dim - above upper bound
	conf = spConfigCreate("unit_tests/Configs/test6.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// dim - below lower bound
	conf = spConfigCreate("unit_tests/Configs/test7.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// file format is not one of : .jpg , .png , .bmp , .gif
	conf = spConfigCreate("unit_tests/Configs/test8.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	// invalid boolean variable is not true or false
	conf = spConfigCreate("unit_tests/Configs/test9.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	// logger level - above upper bound
	conf = spConfigCreate("unit_tests/Configs/test10.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// logger level - below lower bound
	conf = spConfigCreate("unit_tests/Configs/test11.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(conf == NULL);

	// invalid KDTree Split Method
	conf = spConfigCreate("unit_tests/Configs/test12.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(conf == NULL);

	return true;
}//checkParametersErr

static bool checkMissingLines()
{
	SP_CONFIG_MSG conf_msg;
	SPConfig conf;

	//Miss- Number of images
	conf = spConfigCreate("unit_tests/Configs/checkMissingLines1.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_MISSING_NUM_IMAGES);
	ASSERT_TRUE(conf == NULL);

	//Miss- Image directory
	 conf = spConfigCreate("unit_tests/Configs/checkMissingLines2.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_MISSING_DIR);
	ASSERT_TRUE(conf == NULL);

	//Miss- Image suffix
	conf = spConfigCreate("unit_tests/Configs/checkMissingLines3.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_MISSING_SUFFIX);
	ASSERT_TRUE(conf == NULL);

	//Miss- Image prefix
	conf = spConfigCreate("unit_tests/Configs/checkMissingLines4.config", &conf_msg);
	ASSERT_TRUE(conf_msg == SP_CONFIG_MISSING_PREFIX);
	ASSERT_TRUE(conf == NULL);

	return true;
}//checkMissingLines

void spRunConfig()
{
	RUN_TEST(TextConfigFile1);
	RUN_TEST(TextConfigFile2);
	RUN_TEST(TextConfigFile3);
	RUN_TEST(checkErr);
	RUN_TEST(defaultcheck);
	RUN_TEST(checkLineErr);
	RUN_TEST(checkParametersErr);
	RUN_TEST(checkMissingLines);
}//spRunConfig



