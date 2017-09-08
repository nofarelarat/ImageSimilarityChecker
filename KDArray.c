#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "KDArray.h"
#include "SPLogger.h"

/* logger messages */
#define ALLOC_MAT_ARR_ERR           "Allocation of mat or arr of KdArr failed"
#define ALLOC_MAT_ROW_ERR           "Allocation of mat row failed"
#define ALLOC_KDARRAY_ERR           "Allocation of kdArray failed"
#define ALLOC_RIGHT_KDARRAY_ERR     "Allocation of kdArray right failed"
#define ALLOC_LEFT_KDARRAY_ERR      "Allocation of kdArray left failed"
#define ALLOC_SORTED_ARR_ERR        "Allocation of sortedArray failed"
#define FAILED_TO_COPY_ERR          "Failed to copy point"
#define ALLOC_MAP_ERR               "Allocation of map failed"
#define COPY_POINT_SPLIT_ERR        "Failed to copy point while splitting the kdArray"
#define EPSILON                         0.000001

//KDArray struct.
struct sp_kd_array_t {
    SPPoint** points;       //Points arr
    int** mat;              //matix in which the i-th row holds indexes of arr elements ordered by the i-th coordinat //the dimension of the points
    int size;               //The size of arr
    int dim;                //dimension
};

/* Definition Sort struct.
 * We will use it in order to create the mat in kd_arr struct
 */
typedef struct sorted_aix_t
{
    int index;          //element index in kdArray->points
    int coor;           //the coordinate to sort by
    SPPoint* pnt;       //A pointer to the element in kdArray->points
} SortedAix;


/**
 * Free all memory related to Kd_Arr
 *
 * @param kd_arr - the Kdarray to be freed.
 * @param mat_index - the last index of kdArray.mat that need to be freed
 * @param arr_index - the last index of kdArray.points that need to be freed
 */
void destroyKDArray(SPKDArray kd_arr, int mat_index, int arr_index)
{
    int i;
    for(i = 0; i < arr_index; i++)
    {
        spPointDestroy(kd_arr->points[i]);
    }//for
    free(kd_arr->points);
    for(i = 0; i < mat_index; i++)
    {
        free(kd_arr->mat[i]);
    }//for
    free(kd_arr->mat);
    free(kd_arr);
}//destroyKDArray

/**
 * Memory allocation for the new kd_arr and all is fields
 *
 * @param size_of_points - The number of points that need to be allocated
 * @param point_dimension - The points dimension that need to be allocated
 *
 * @return allocated kdArray in case of success and NULL in case of allocation failure
 */
SPKDArray KDArrayAllocation(int size_of_points, int point_dimension)
{
    int i;
    SPKDArray kd_arr = (SPKDArray) malloc(sizeof(struct sp_kd_array_t));
    if(kd_arr==NULL)
    {
        spLoggerPrintError(ALLOC_KDARRAY_ERR , __FILE__, __func__, __LINE__);
        return kd_arr;
    }//if
    kd_arr->size = size_of_points;
    kd_arr->points = (SPPoint**) malloc(sizeof(SPPoint*) * size_of_points);

    kd_arr->mat = (int**) malloc(sizeof(int*) * point_dimension);
    kd_arr->dim = point_dimension;
    //cheack if mem allocation succeded
    if(kd_arr->points == NULL || kd_arr->mat == NULL)
    {
        destroyKDArray(kd_arr, 0, 0);
        kd_arr = NULL;
        spLoggerPrintError(ALLOC_MAT_ARR_ERR, __FILE__, __func__, __LINE__);
        return kd_arr;
    }//if
    //allocate matrix row

    for(i = 0; i < point_dimension; i++)
    {
        kd_arr->mat[i] = (int*) malloc(sizeof(int) * size_of_points);
        //check if allocation failed and release allocated memory if so
        if(kd_arr->mat[i] == NULL)
        {
            destroyKDArray(kd_arr, i, 0);
            kd_arr = NULL;
            spLoggerPrintError( ALLOC_MAT_ROW_ERR, __FILE__, __func__, __LINE__);
            return kd_arr;
        }//if
    }//for
    return kd_arr;
}//KDArrayAllocation

int pointComparator(const void *p, const void *q)
{
    // var deceleration
    int cor;
    SortedAix pointA;
    SortedAix pointB;
    double p_1;
    double p_2;

    // getting elements
    pointA = *(SortedAix*)p;
    pointB = *(SortedAix*)q;
    cor = pointA.coor;
    p_1 = spPointGetAxisCoor(pointA.pnt, cor);
    p_2 = spPointGetAxisCoor(pointB.pnt, cor);

    // sorting in ascending order, if points have the same coordinate value sorting by image index
    if (p_1 - p_2 == 0)
    {
        return spPointGetIndex(pointA.pnt) - spPointGetIndex(pointB.pnt);
    }//if
    return (p_1 - p_2);
}//pointComparator

/**
 * Fill kd_arr->mat with data so that the i(rows) is the point's indexes ordered by the i'th coordinate
 *
 * @param kd_arr - the relevant KdArray that we need to create the matrix for
 *
 * @return true if succeeded and false otherwise:
 *  if kd_arr== NULL
 *
 */
bool initMat(SPKDArray kd_arr)
{
    if(kd_arr==NULL)
    {
        return false;
    }//if
    SortedAix* sort_arr = (SortedAix*) malloc(sizeof(SortedAix) * kd_arr->size);
    if(sort_arr==NULL)
    {
        spLoggerPrintError(ALLOC_SORTED_ARR_ERR, __FILE__, __func__, __LINE__);
        return false;
    }//if
    int i, j;
    int point_dim = spPointGetDimension(kd_arr->points[0]); //the dimension of kdArray->arr points
    for(i = 0; i < kd_arr->size; i++)
    {
        sort_arr[i].index = i;
        sort_arr[i].pnt = kd_arr->points[i];
    }//for
    for(i = 0; i < point_dim; i++)
    {
        // setting the coordinate to sort by
        for(j = 0; j < kd_arr->size; j++)
        {
                sort_arr[j].coor = i;
        }//for
        //sorting array by the i-th coordinate
        qsort(sort_arr, kd_arr->size, sizeof(SortedAix), pointComparator);
        //filling the i-th row in mat with data sorted by the i-th coordinate
        for(j = 0; j < kd_arr->size; j++)
        {
            kd_arr->mat[i][j] = sort_arr[j].index;
        }//for
    }//for
    free(sort_arr);
    return true;
}//initMat

/*
 * Cheack that all args are valid
 *
 * @param points_arr -   the data
 * @param size -         arr size
 *
 * @return true if data is valid:
 *      -points_arr is not NULL
 *      -points_arr[0] is not NULL
 *      -size>0
 *      -all points are in the same dimension.
 *
 *  and false otherwise
 */
bool cheackValid(SPPoint** points_arr, int size)
{
    //var deceleration
    int i=1;
    int points_dim=0;
    int cur_point_dim=0;
    if(points_arr == NULL || size <= 0 || points_arr[0] == NULL)
    {
        return false;
    }//if
    //checking dimension aspects
    points_dim = spPointGetDimension(points_arr[0]);
    for(i = 1; i < size; i++)
    {
        if( points_arr[i] ==NULL)
        {
            return false;
        }//if
        cur_point_dim=spPointGetDimension(points_arr[i]);
        if(cur_point_dim != points_dim)
        {
            return false;
        }//if
    }//for
    //data is valid
    return true;
}//cheackValid

SPKDArray Init(SPPoint** points_arr, int size)
{
    SPKDArray kd_arr;
    bool valid=cheackValid(points_arr, size);
    if(valid==false)
    {
        return NULL;
    }//if
    //get the dimension of kdArray->arr points from the first element
    int pointDim = spPointGetDimension(points_arr[0]);
    kd_arr = KDArrayAllocation(size, pointDim);
    if(kd_arr==NULL)
    {
        return NULL;
    }//if
    int i;
    bool flag = true;
    //copy points

    for(i = 0; (i < size) && flag; i++)
    {
        kd_arr->points[i] = spPointCopy(points_arr[i]);
        flag = kd_arr->points[i] != NULL;
        if (flag == false)
        {
            spLoggerPrintError(COPY_POINT_SPLIT_ERR, __FILE__, __func__, __LINE__);
        }//if
    }//for

    //mat creation failed
    if(flag==false || initMat(kd_arr)==false)
    {
        destroyKDArray(kd_arr, pointDim , --i);
        kd_arr = NULL;
    }//if
    return kd_arr;
}//Init

/**
 *
 * Copying points and setting map to indicate the new indexes
 * in split kdArrayes *  coordinate coor are in kd_Left , and the rest of the points are in kd_Right.
 *
 * @param kd_arr    - original kdArray that is being split
 * @param kd_Left   -  the left kdArray to be initiated
 * @param kd_Right  - the right kdArray to be initiated
 * @param dim       - the dimension of points in kd_arr
 * @param map       - a pointer to the map thats mapping between original point index and new one.
 *                  points in the left kdArray will get positive indexes while points in the right one will get negative indexes.
 *                  when filling kdArrays matrixes we will use these transformation:
 *                  kdLeft: newIndex = map[oldIndex] - 1
 *                  kdRight: newIndex = -(map[oldIndex] - 1
 * @param coor      - the coordinate to split by
 *
 * @return true if succeeded, otherwise false
 */
bool innerSplit(SPKDArray kd_arr, SPKDArray kd_Left, SPKDArray kd_Right, int** map,int dim, int coor)
{
    // var deceleration
    int i;
    bool flag = true;
    int left_Index=0;
    int right_Index=0;

    *map = (int*) calloc(sizeof(int) , kd_arr->size);
    //check if allocation failed
    if(map == NULL)
    {
        spLoggerPrintError(ALLOC_MAP_ERR, __FILE__, __func__, __LINE__);
        flag = false;
        return flag;
    }//if

    for(i = 0; i < kd_Right->size; i++)
    {
        (*map)[kd_arr->mat[coor][i + kd_Left->size]] = -1;
    }//for
    for(i = 0; i < kd_Left->size; i++)
    {
        (*map)[kd_arr->mat[coor][i]] = 0;
    }//for

    for (i =0; i <kd_arr->size && flag; i++)
    {
        SPPoint *point = spPointCopy(kd_arr->points[i]);
        if(point == NULL)
        {
            destroyKDArray(kd_Right, dim, right_Index);
            destroyKDArray(kd_Left, dim, left_Index);
            spLoggerPrintError(FAILED_TO_COPY_ERR, __FILE__, __func__, __LINE__);
            free(*map);
            flag = false;
            return flag;
        }//if
        else
        {
            if ((*map)[i] != 0) //rightKD array
            {
                kd_Right->points[right_Index] = point;
                (*map)[i] = - (right_Index + 1);
                right_Index++;
            }//if
            else //left KDArray
            {
                kd_Left->points[left_Index] = point;
                (*map)[i] = left_Index + 1;
                left_Index++;
            }//else
        }//else
    }//for
    return flag;
}//innerSplit

/**
 * Setting matrix for each kd Array
 *
 * When filling kdArrays matrixes we will use these transformation:
 * for kd_Right: newIndex = -(map[oldIndex] - 1)
 * for kd_Left: newIndex = map[oldIndex] - 1
 *
 * @param kd_arr - original kdArray that we are about to split
 * @param kd_Left - the left kdArray
 * @param kd_Right - the right kdArray
 * @param dim - the dimension of the points in kd_arr
 * @param map - this matrix is mapping between the index of the original point and new one
 */
void splitSetteingMat(SPKDArray kd_arr, SPKDArray kd_Left, SPKDArray kd_Right, int dim, int* map)
{
    //var deceleration
    int i;
    int j;
    int left_Index;
    int right_Index;
    int orig; //original index
    for(i = 0; i < dim; i++)
    {
        left_Index = 0;
        right_Index = 0;
        //filling kdLeft and kdRight matrixes. See map documentation above for more info
        for(j = 0; j < kd_arr->size; j++)
        {
            orig = kd_arr->mat[i][j];
            if(map[orig] < 0)
            {
                kd_Right-> mat[i][right_Index] = - map[orig] - 1;
                right_Index++;
            }//if
            else
            {
                kd_Left-> mat[i][left_Index] = map[orig] - 1;
                left_Index++;
            }//else
        }//for
    }//for
}//splitSetteingMat


void Split(SPKDArray kd_arr, int coor, SPKDArray* kd_Right, SPKDArray* kd_Left, double* middle)
{
    //var deceleration
    int point_dim;
    int right_S;
    int left_S;
    int middle_index;
    int actual_dim;
    bool flag;
    int* map;
    /* int* map use to mapping between original point index and new one.
     * points in the left kdArray will get positive indexes
     * while points in the right one will get negative indexes.
     * when filling kdArrays matrixes we will use these struct:
     *
     * kdLeft: new_Index = map[old_Index] - 1
     * kdRight: new_Index = -(map[old_Index] - 1)
     */

    if(kd_arr == NULL || coor < 0 )
    {
        return;
    }//if
    actual_dim=spPointGetDimension(kd_arr->points[0]);
    if(kd_arr->size <= 1 || coor >= (point_dim =  actual_dim))
    {
        return;
    }
    left_S = kd_arr->size/2;
    right_S = kd_arr->size - left_S;


    *kd_Left = KDArrayAllocation(left_S, point_dim);
    if(*kd_Left == NULL)
    {

        spLoggerPrintError(ALLOC_LEFT_KDARRAY_ERR, __FILE__, __func__, __LINE__);
        return;
    }//if
    *kd_Right = KDArrayAllocation(right_S, point_dim);
    if(*kd_Right == NULL)
    {

        destroyKDArray(*kd_Left, point_dim, left_S);
        *kd_Left = NULL;
        spLoggerPrintError(ALLOC_RIGHT_KDARRAY_ERR, __FILE__, __func__, __LINE__);
        return;
    }//if

    // setting the middle(median)
    middle_index = kd_arr->mat[coor][left_S - 1];

    *middle = spPointGetAxisCoor(kd_arr->points[middle_index], coor);
    // copying points and setting the map
    flag=innerSplit(kd_arr,*kd_Left, *kd_Right, &map, point_dim,coor);
    if (flag==false)
    {
        destroyKDArray(*kd_Left, point_dim, left_S);
        destroyKDArray(*kd_Right, point_dim, right_S);
        *kd_Right = NULL;
        *kd_Left = NULL;
        return;
    }//if
    // setting mat to kd_left and kd_right
    splitSetteingMat(kd_arr, *kd_Left, *kd_Right, point_dim, map);
    free(map);
}//Split

void spKDArrayDestroy(SPKDArray kd_arr)
{
    int index;
    if(kd_arr==NULL)
    {
        return;
    }//if
    index = 0;
    if(kd_arr->points != NULL)
    {
        index = spPointGetDimension(kd_arr->points[0]);
    }//if
    destroyKDArray(kd_arr, index, kd_arr->size);
}// spKDArrayDestroy

int spKDArrGetSize(SPKDArray kd_arr)
{
    if(kd_arr==NULL)
    {
        return -1;
    }//if
    return kd_arr->size;
}//spKDArrGetSize


int spKDArrGetDim(SPKDArray kd_arr)
{
    if (kd_arr == NULL)
    {
        return -1;
    }//if
    return kd_arr->dim;
}//spKDArrGetDim


int spKDArrGetMaxSpread(SPKDArray kd_arr)
{
    int flag = -1; // will hold the index of the coordinate with the max spread
    if(kd_arr==NULL)
    {
        return -1;
    }//if
    double max_spread;
    int i;
    int j;
    int current_point; //is used as temp var in the for
    double *min; 		// will save the minimum for each coor
    double *max; 		// will save the maximum for each coor
    double cor;
    min = (double *) malloc(sizeof(double) * kd_arr->dim);
    max = (double *) malloc(sizeof(double) * kd_arr->dim);
    if(min==NULL || max==NULL)
    {
        free(min);
        free(max);
        return flag;
    }//if
    // setting min and max to be the first in each coordinate
    for(i = 0; i < kd_arr->dim; i++)
    {
        cor=spPointGetAxisCoor(kd_arr->points[0],i);
        min[i] = cor;
        max[i] = cor;
    }//for

    for(j = 0; j < kd_arr->size; j++)
    {
        for (i = 0; i < kd_arr->dim; i++)
        {
            current_point = spPointGetAxisCoor(kd_arr->points[j],i);
            if (current_point > max[i])
            {
                max[i] = current_point;
            }//if
            if  (current_point < min[i])
            {
                min[i] = current_point;
            }//if
        }//for
    }//for
    // finding max spread - first setting to 0 coordinate
    flag = 0;
    double tmp;
    max_spread = max[0] - min[0];
    for(i = 0; i  < kd_arr->dim; i++)
    {
        tmp=(max[i] - min[i]);
        if (tmp > max_spread)
        {
            max_spread =tmp;
            flag = i;
        }//if
    }//for
// free allocated memory
    free(min);
    free(max);
    return flag;
}//spKDArrGetMaxSpread


SPPoint*  spKDArrGetFirstPoint(SPKDArray kd_arr)
{
    if (kd_arr == NULL)
    {
        return NULL;
    }//if
    return spPointCopy(kd_arr->points[0]);
}//spKDArrGetFirstPoint
