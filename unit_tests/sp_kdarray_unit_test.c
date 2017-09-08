/*
 * sp_kdarray_unit_test.c
 *
 *  Created on: Mar 24, 2017
 *      Author: NofarE
 */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "main_test.h"
#include "../KDArray.h"
#include "unit_test_util.h"

#define POINT_SIZE_CHECK1		6
#define POINT_DIM_CHECK1 		2
#define POINT_SIZE_CHECK2		6
#define POINT_SIZE_CHECK3		6
#define POINT_SIZE_CHECK4		5

//Check if two points are identical
static bool comparePoints(SPPoint* p1, SPPoint* p2)
{
	int i;
	ASSERT_TRUE(spPointGetIndex(p1) == spPointGetIndex(p2));
	ASSERT_TRUE(spPointGetDimension(p1) == spPointGetDimension(p2));
	int dim = spPointGetDimension(p1);
	for (i = 0; i < dim ; i++)
	{
		ASSERT_TRUE(spPointGetAxisCoor(p1, i) == spPointGetAxisCoor(p2, i));
	}
	return true;
}
//Checks the kdarray with the expected data
static bool test_validate(SPKDArray kd, SPPoint *point_for_compare, int size_kd, int dimension)
{
	ASSERT_TRUE(spKDArrGetDim(kd) == dimension);
	ASSERT_TRUE(spKDArrGetSize(kd) == size_kd);
	SPPoint* point_1 = spKDArrGetFirstPoint(kd);
	ASSERT_TRUE(point_1 != NULL);
	ASSERT_TRUE(comparePoints(point_for_compare,point_1));
	spPointDestroy(point_1);
	return true;
}//test_validate

//Check init
static bool create_check()
{
	//var deceleration
	int i;
	SPKDArray kd;
	SPPoint** points_arr=NULL;
	kd=Init(points_arr,POINT_SIZE_CHECK1+1);
	ASSERT_TRUE(kd == NULL);

	points_arr = (SPPoint**)malloc(sizeof(SPPoint*)*POINT_SIZE_CHECK1);
	double data[POINT_SIZE_CHECK1][POINT_DIM_CHECK1] = {{2,7},{35,87},{40,92},{91,87},{65,78},{7,6}};

	for (i = 0; i < POINT_SIZE_CHECK1; i++)
	{
		points_arr[i] = spPointCreate(data[i],POINT_DIM_CHECK1,i);
	}//for

	kd = Init(points_arr, POINT_SIZE_CHECK1);
	//check if kd is not NULL and id the data is vallid
	ASSERT_TRUE(kd != NULL);
	ASSERT_TRUE(test_validate(kd,points_arr[0],POINT_SIZE_CHECK1,POINT_DIM_CHECK1));

	for (i = 0; i < POINT_SIZE_CHECK1; i++)
	{
		spPointDestroy(points_arr[i]);
	}//for
	free(points_arr);
	spKDArrayDestroy(kd);
	return true;
}//create_check

//Check destroy kdarray
void free_check2(SPKDArray kd,SPPoint* pnt_1,SPPoint* pnt_2,SPPoint** pnt_arr, SPKDArray left_kd, SPKDArray right_kd)
{
    	spKDArrayDestroy(left_kd);
    	spKDArrayDestroy(right_kd);
	    spKDArrayDestroy(kd);
	    spPointDestroy(pnt_1);
	    spPointDestroy(pnt_2);
	    free(pnt_arr);
}//free_check2

//Checks splits
static bool Split_check()
{
    // var deceleration
    SPKDArray kd;
    SPKDArray right_kd;
    SPKDArray left_kd;
    SPPoint* pnt_1;
    SPPoint* pnt_2;
    double median_split;

    SPPoint** pnt_arr = (SPPoint**)malloc(sizeof(SPPoint*)*2);
    double pointsData[][POINT_SIZE_CHECK2] = {{2,9,54,65,43,24},{43,2,8,67,45,23}};

    pnt_1 = spPointCreate(pointsData[0],POINT_SIZE_CHECK2,0);
    pnt_2 = spPointCreate(pointsData[1],POINT_SIZE_CHECK2,1);

    pnt_arr[0] = pnt_1;
    pnt_arr[1] = pnt_2;

    kd=Init(NULL,POINT_SIZE_CHECK2);
    ASSERT_TRUE(kd == NULL);
    kd = Init(pnt_arr, 2);
    ASSERT_TRUE(kd != NULL);

    //Split number 2
      Split(kd, 1, &right_kd, &left_kd, &median_split);
      ASSERT_TRUE(left_kd != NULL);
      ASSERT_TRUE(right_kd != NULL);
      ASSERT_TRUE(median_split == 2);
      // validating data
      if (left_kd != NULL && right_kd != NULL)
      {
          // left side
          ASSERT_TRUE(test_validate(left_kd,pnt_2,1,POINT_SIZE_CHECK2));
          // right side
          ASSERT_TRUE(test_validate(right_kd,pnt_1,1,POINT_SIZE_CHECK2));
      }//if

     spKDArrayDestroy(left_kd);
     spKDArrayDestroy(right_kd);

    //Split number 1
    Split(kd, 0, &right_kd, &left_kd, &median_split);
    ASSERT_TRUE(right_kd != NULL);
    ASSERT_TRUE(left_kd != NULL);
    ASSERT_TRUE(median_split == 2);

    if (left_kd != NULL && right_kd != NULL)
    {
        ASSERT_TRUE(test_validate(left_kd, pnt_1,1,POINT_SIZE_CHECK2));
        ASSERT_TRUE(test_validate(right_kd,pnt_2,1,POINT_SIZE_CHECK2));
    }//if

    free_check2(kd,pnt_1,pnt_2,pnt_arr,left_kd,right_kd);
    return true;
}//Split_check

//Check destroy kdarray
void free_check3(SPKDArray kd,SPPoint** pnt_arr)
{
	for (int i = 0; i < POINT_SIZE_CHECK3; i++)
	{
		spPointDestroy(pnt_arr[i]);
	}//for
	spKDArrayDestroy(kd);
	free(pnt_arr);
}//free_check3

//Check getMaxSpread function
static bool Spread_check()
{
	//var deceleration
	SPPoint* pnt_arr[POINT_SIZE_CHECK4];
	SPKDArray kd;
	int i;

	double data_pnt[][2] = {{1,3},{150,90},{8,7},{10,25},{3,23}};
	double new[] = {2,90};
	double new1[] = {400,2};
	double new2[] = {400,1000};

	for (i = 0; i < POINT_SIZE_CHECK4; i++)
	{
		pnt_arr[i] = spPointCreate(data_pnt[i],2,i);
	}//for

	kd = Init(pnt_arr, POINT_SIZE_CHECK4);
	if (kd != NULL)
	{
		//max spread is x coordinate - pnt_arr x = 1, point[1] x = 150)
		ASSERT_TRUE(spKDArrGetMaxSpread(kd) == 0);
		spKDArrayDestroy(kd);
	}//if

	spPointDestroy(pnt_arr[1]);
	pnt_arr[1] = spPointCreate(new, 2, 1);

	kd = Init(pnt_arr, POINT_SIZE_CHECK4);
	if (kd != NULL)
	{
		//max spread is the y coordinate pnt_arr y = 2, pnt_arr y = 90)
		ASSERT_TRUE(spKDArrGetMaxSpread(kd) == 1);
		spKDArrayDestroy(kd);
	}//if
	spPointDestroy(pnt_arr[1]);
	pnt_arr[1] = spPointCreate(new1, 2, 1);
	kd = Init(pnt_arr, POINT_SIZE_CHECK4);
	if (kd != NULL)
	{
		//max spread is the x coordinate pnt_arr x = 1, pnt_arr x=400)
		ASSERT_TRUE(spKDArrGetMaxSpread(kd) == 0);
		spKDArrayDestroy(kd);
	}//if
	spPointDestroy(pnt_arr[1]);
	pnt_arr[1] = spPointCreate(new2, 2, 1);
	kd = Init(pnt_arr, POINT_SIZE_CHECK4);
	if (kd != NULL)
	{
		//max spread is the x coordinate pnt_arr y = 3, pnt_arr y=1000)
		ASSERT_TRUE(spKDArrGetMaxSpread(kd) == 1);
		spKDArrayDestroy(kd);
	}//if

	for (i = 0; i < POINT_SIZE_CHECK4; i++)
	{
		spPointDestroy(pnt_arr[i]);
	}//for
	return true;
}//Spread_check

//Check kdarray function
static bool General_check()
{
	SPKDArray kd=NULL;
	SPPoint* singlePoint=NULL;
	int flag;
	int i;

	//NULL Checks
	singlePoint =spKDArrGetFirstPoint(kd);
	ASSERT_TRUE(singlePoint==NULL);

	flag=spKDArrGetMaxSpread(kd);
	ASSERT_TRUE(flag==-1);

	flag=spKDArrGetDim(kd);
	ASSERT_TRUE(flag==-1);

	flag=spKDArrGetSize(kd);
	ASSERT_TRUE(flag==-1);

	SPPoint** pnt_arr = (SPPoint**)malloc(sizeof(SPPoint*)*2);
	double pointsData[2][4] = {{2,9,54,65},{43,2,8,68}};
	SPPoint* pnt_1 = spPointCreate(pointsData[0],4,0);
	SPPoint* pnt_2 = spPointCreate(pointsData[1],4,1);
	pnt_arr[0] = spPointCopy(pnt_1);
	pnt_arr[1] =  spPointCopy(pnt_2);
    kd = Init(pnt_arr, 2);
    ASSERT_TRUE(kd != NULL);
    spPointDestroy(pnt_1);
    spPointDestroy(pnt_2);

    for (i = 0; i < 2; i++)
    {
    	spPointDestroy(pnt_arr[i]);
    }//for
    free(pnt_arr);
    spKDArrayDestroy(kd);

	return true;
}//General_check

void spRunKdArray()
{
	RUN_TEST(create_check);
	RUN_TEST(Split_check);
	RUN_TEST(Spread_check);
	RUN_TEST(General_check);
}//spRunKdArray
