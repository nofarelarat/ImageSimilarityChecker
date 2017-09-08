#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>

//Check create correct point
bool spPointBasicCreateTest() {
	int i, dim, index;
	SPPoint* p;
	double data[5] = {1.2, 2.3, 3.4, 4.5, 5.6};
	dim = 5;
	index = 1;

	//NULL data
	p = spPointCreate(NULL, dim, index);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);

	//dim <= 0
	p = spPointCreate(data, -1, index);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);

	//index < 0
	p = spPointCreate(data, dim, -1);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);

	// Checking valid point creation
	p  = spPointCreate(data,dim,index);
	ASSERT_TRUE(p != NULL);
	ASSERT_TRUE(spPointGetIndex(p) == index);
	ASSERT_TRUE(spPointGetDimension(p) == dim);
	for (i = 0; i < spPointGetDimension(p); i++)
	{
		ASSERT_TRUE(spPointGetAxisCoor(p, i) == data[i]);
	}
	spPointDestroy(p);
	return true;
}//spPointBasicCreateTest

//Checks if copy Works
bool pointBasicCopyTest() {
	double data[2] = { 1.0, 1.0 };
	int dim = 2;
	int index = 1;
	SPPoint* p = spPointCreate(data, dim, index);
	SPPoint* q = spPointCopy(p);
	ASSERT_TRUE(spPointGetIndex(p) == spPointGetIndex(q));
	ASSERT_TRUE(spPointGetDimension(p) == spPointGetDimension(q));
	for (int i = 0; i < spPointGetDimension(p); i++)
	{
		ASSERT_TRUE(spPointGetAxisCoor(p, i) == spPointGetAxisCoor(q, i));
	}
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}//pointBasicCopyTest

//Checks if the distance calculate works
bool pointBasicL2DistanceTest() {
	double data1[2] = { 1.0, 1.0 };
	double data2[2] = { 1.0, 0.0 };
	int dim1 = 2;
	int dim2 = 2;
	int index1 = 1;
	int index2 = 1;
	SPPoint* p = spPointCreate(data1, dim1, index1);
	SPPoint* q = spPointCreate(data2, dim2, index2);
	ASSERT_TRUE(spPointL2SquaredDistance(p,p) == 0.0);
	ASSERT_TRUE(spPointL2SquaredDistance(q,q) == 0.0);
	ASSERT_FALSE(spPointL2SquaredDistance(p,q) == 0.0);
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}//pointBasicL2DistanceTest

void spRunPoint1()
{
	RUN_TEST(spPointBasicCreateTest);
	RUN_TEST(pointBasicCopyTest);
	RUN_TEST(pointBasicL2DistanceTest);
}
