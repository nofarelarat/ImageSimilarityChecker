/*
 * SPKDArray.h
 *
 *  Created on: 17.3.2017
 *      Author: NofarE
 */

#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"

/** Type for defining the KD Array */
typedef struct sp_kd_array_t* SPKDArray;

/**
 * Return the size of the kdArray or -1 in case kd_arr is  NULL
 *
 * @param kd_arr - the KDArray to be processed
 */
int spKDArrGetSize(SPKDArray kd_arr);

/**
 * This function will provide us a copy of the first point in the kd_Arr or -1 in case that
 * kd_arr is NULL;
 *
 * @param kd_arr - the relevant KDArray
 */
SPPoint*  spKDArrGetFirstPoint(SPKDArray kd_arr);

/**
 * Initializes the kd-array with the data given by points_arr (first param of the function)
 * The complexity of this operation is O(d*nlogn) where d is point's dimension and n is the size
 *
 * @param points_arr - the data
 * @param size - the size of points_arr
 *
 * @return  - pointer to the new  SPKDArray or NULL in the following cases
 *          - invalid arguments
 *          - checking if point_arr = NULL
 *          - size < 0
 *          - all points does not  have the same dimension
 *          - allocation memory failure
 */
SPKDArray Init(SPPoint** points_arr, int size);

/** Returns two kd-arrays (kdLeft, kdRight) such that the first [n/2]  points with
 * respect to the coordinate coor are in kdLeft , and the rest of the points are in kdRight.
 *
 * @param kd_Arr     - the KDArray to be split.
 * @param coor       - the coordinate to split by.
 * @param kd_Right  - pointer to the right KDArray (memory allocated inside the function)
 * @param kd_Left   - pointer to the left KDArray (memory allocated inside the function)
 * @param middle    - pointer to a double that will save the median's i-th dimension value
 *
 * In the following cases the function wont do nothing:
 * -kdArray size is 1 or less
 * - coor < 0 or coor is bigger than the points dim
 * - kd_arr is  NULL
 */
void Split(SPKDArray kd_arr, int coor, SPKDArray* kd_Right, SPKDArray* kd_Left, double* middle);

/**
 * Frees all memory related to KDArray
 *
 * @param kd_arr - the relevant kdarray that needs to be free
 */
void spKDArrayDestroy(SPKDArray kd_arr);

/**
 * Return points dimension in kd_arr or -1 in case kd_arr is null
 *
 * @param kd_arr - the KDArray to be processed
 * @return : points' dimension in kd_Arr
 */
int spKDArrGetDim(SPKDArray kd_arr);

/**
 * returns the dimension with the highest spread.
 * (if there are several dimension that suits,  choose the lowest one)
 * spread of the i-th dimension is the difference in all points between
 * the max and min of the i-th coordinate
 *
 *  @param kd_arr - the relevant KDArray
 *  @return: the dimension with the highest spread (flag variable) or -1 in the following cases:
 *          -kd_arr==NULL
 *          -allocation failed
 */
int spKDArrGetMaxSpread(SPKDArray kd_arr);

/*
 * Compares between two PointAxis objects based on their axis coordinate value.
 *
 * @param p         address of the first sortAxis point
 * @param q         address of the second sortAxis point
 *
 * @return 0 - if values are equal (difference is less than Epsilon)
 *         1 -  if value of p is bigger than q
 *        -1 -  if value of q is bigger than p
 */
int pointComparator(const void *p, const void *q);
#endif /* KDARRAY_H_ */
