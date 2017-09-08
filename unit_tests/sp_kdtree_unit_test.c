/*
 * sp_kdtree_unit_test.c
 *
 *  Created on: Mar 24, 2017
 *      Author: NofarE
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "unit_test_util.h"
#include "../KDArray.h"
#include "../KDTree.h"
#include "main_test.h"

#define POINTS 9
#define DIM1 2
#define DIM2 4
#define K_FORKNN 2


//Check KDTree methods

static bool checkKNN1()
{
	SPPoint** points = (SPPoint**) malloc(sizeof(SPPoint*)*POINTS);
	SPPoint* q;
	int near_i[K_FORKNN];
	int i;
	SPKDTreeNode root;
	bool flag = true;

	double data[][DIM1] = {{1,1},{4,2},{7,1},{2,3},{5,3},{9,3},{4,5.2},{8,6},{6,7}};
	double q_data[DIM1] = {5.1, 5};
	int the_nearest[K_FORKNN] = {6, 4};

	for (i = 0; i < POINTS; i++)
	{
		points[i] = spPointCreate(data[i],DIM1,i);
	}//for
	q = spPointCreate(q_data, DIM1, i);
	ASSERT_TRUE(q != NULL);
	root = initKDTree(points, POINTS, RANDOM);
	ASSERT_TRUE(root != NULL); //check init
	flag = spKDTreeKNearestNeighbors(root, q, K_FORKNN, near_i);
	ASSERT_TRUE(flag); //check spKDTreeKNearestNeighbors
	for(i = 0; i < K_FORKNN; i++)
	{
		ASSERT_TRUE(near_i[i] == the_nearest[i]);
	}//for
	//free allocated memory
	spKDTreeDestroy(root);

	//free allocated memory
	for (i = 0; i < POINTS; i++)
	{
		spPointDestroy(points[i]);
	}//for
	free(points);
	spPointDestroy(q);

	return flag;
}//checkKNN1

static bool checkKNN2()
{
	SPPoint** points = (SPPoint**) malloc(sizeof(SPPoint*)*POINTS);
	SPPoint* q;
	int near_i[2];
	int i;
	SPKDTreeNode root;
	bool flag = true;

	double data[][DIM2] = {{1,1,9,12},{4,2,5,99},{7,1,9,14},{2,3,99,300},{4.9,4.9,4.9,4.9},{5,5,5,5},{4,5.2,888,6},{8,6,10000,5},{6,7,99999999.6,55}};
	double q_data[DIM2] = {5,5,5,5};
	int the_nearest[K_FORKNN] = {5,4};

	for (i = 0; i < POINTS; i++)
	{
		points[i] = spPointCreate(data[i],DIM2,i);
	}//for
	q = spPointCreate(q_data, DIM2, i);
	ASSERT_TRUE(q != NULL);
	root = initKDTree(points, POINTS, MAX_SPREAD);
	ASSERT_TRUE(root != NULL); //check init
	flag = spKDTreeKNearestNeighbors(root, q, K_FORKNN, near_i);
	ASSERT_TRUE(flag); //check spKDTreeKNearestNeighbors
	for(i = 0; i < K_FORKNN; i++)
	{
		ASSERT_TRUE(near_i[i] == the_nearest[i]);
	}//for
	//free allocated memory
	spKDTreeDestroy(root);

	//free allocated memory
	for (i = 0; i < POINTS; i++)
	{
		spPointDestroy(points[i]);
	}//for
	free(points);
	spPointDestroy(q);

	return flag;
}//checkKNN2;

static bool General_check()
{
	bool flag;
	SPKDTreeNode root;
	SPPoint* q;
	double q_data[DIM1] = {5.1, 5};
	q = spPointCreate(q_data, DIM1, 0);
	int near_i[K_FORKNN];
	SPPoint** points = (SPPoint**) malloc(sizeof(SPPoint*));
	points[0] = spPointCopy(q);

	//Check init with NULL
	root = initKDTree(NULL, DIM1, INCREMENTAL);
	ASSERT_TRUE(root==NULL);

	//Check KNN with NULL
	flag = spKDTreeKNearestNeighbors(root,q, K_FORKNN, near_i);
	ASSERT_TRUE(flag==false);

	//Check isLeaf
	root = initKDTree(points, 1, INCREMENTAL);
	ASSERT_TRUE(root != NULL); //check init
	flag = isLeaf(root);
	ASSERT_TRUE(flag==true);

	//free allocated memory
	spKDTreeDestroy(root);
	spPointDestroy(points[0]);
	free(points);
	spPointDestroy(q);

	return true;
}//General_check

void spRunKDTree()
{
	RUN_TEST(checkKNN1);
	RUN_TEST(checkKNN2);
	RUN_TEST(General_check);
}//spRunKDTree
