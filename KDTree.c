#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "SPBPriorityQueue.h"
#include "KDArray.h"
#include "KDTree.h"

/*Logger Messages*/
#define SPLIT_ERR "Error in splitting kd"
#define ALLOC_KD_ERR "Allocating kd error"
#define ALLOC_QUEUE_ERR "Allocating Queue error"
#define CREATE_KD_ERR "Failed create kd"
#define FINISH_CREATE_KD_INFO "Finish create kd"

//kdTreeNode struct
struct sp_kd_tree_node_t
{
    int dim;
    double value;
    SPKDTreeNode right;
    SPKDTreeNode left;
    SPPoint* data;
};

/**
 * Calculating split coordinate
 *
 * @param kd_arr - the array of points
 * @param splitM - the split method
 * @param level - current kd level
 *
 * @return the split coordinate according to the split
 *  method we defined before. return -1 in case of an error.
 */
int getSplitMethod(SPKDArray kd_arr, splitMethod splitM, int level)
{
    switch (splitM)
    {
        case MAX_SPREAD:
            return spKDArrGetMaxSpread(kd_arr);
            break;
        case RANDOM:
            return rand() %  spKDArrGetDim(kd_arr);
            break;
        case INCREMENTAL:
            return level % spKDArrGetDim(kd_arr);
        default:
            return -1; //invalid
    }
}//getSplitMethod


bool isLeaf(SPKDTreeNode node)
{
    if (node != NULL)
    {
            return node->data != NULL;
    }//if
    return false;
}//isLeaf

/**
 * Recursive method using by initKDTree
 * Initializes the KDTreeNode with the data given by kd_arr.
 * The complexity of this operation is O(d*nlogn)
 * where d is point's dimension and n is the size
 *
 * @param kd_arr - the array of points
 * @param splitM - the split method
 * @param level - current kd level
 *
 * @return  - pointer to the created KDTreeNode
 *          - NULL in case of a failure
 */
SPKDTreeNode initKDTreeNodeRec(SPKDArray kd_arr, splitMethod splitM, int level)
{
    SPKDTreeNode treeN = NULL;
    treeN = (SPKDTreeNode) malloc(sizeof(struct sp_kd_tree_node_t));
    if(treeN==NULL)
    {
        spLoggerPrintError(ALLOC_KD_ERR, __FILE__, __func__, __LINE__);
        return NULL;
    }//if
    if (spKDArrGetSize(kd_arr) == 1)
    {
    	treeN->dim = -1;
        treeN->value = -1;
        treeN->right = NULL;
        treeN->left = NULL;
        treeN->data = spKDArrGetFirstPoint(kd_arr);
        if (treeN->data == NULL)
        {
            treeN = NULL;
        }//if
    }//if
    else
    {
        SPKDArray right;
        SPKDArray left;
        double middle;
        int splitCoor = getSplitMethod(kd_arr, splitM, level);
        Split(kd_arr,splitCoor,&right,&left, &middle);
        if (right == NULL || left == NULL)
        {
        	spLoggerPrintError(SPLIT_ERR, __FILE__, __func__, __LINE__);
        }//if
        else
        {
        	treeN->dim = splitCoor;
            treeN->value = middle;
            treeN->right = initKDTreeNodeRec(right, splitM, level + 1);
            if(treeN->right != NULL)
            {
            	treeN->left = initKDTreeNodeRec(left, splitM, level + 1);
            }//if
            else
            {
            	treeN->left = NULL;
            }//else
            treeN->data = NULL;
            if (treeN->left == NULL || treeN->right == NULL)
            {
            	spKDTreeDestroy(treeN);
                treeN = NULL;
            }//if
            spKDArrayDestroy(left);
            spKDArrayDestroy(right);
        }//else
    }//else
    return treeN;
}//initKDTreeNodeRec


SPKDTreeNode initKDTree(SPPoint** arr, int size, splitMethod splitM)
{
    // var deceleration
    SPKDArray kd_arr = NULL;
    SPKDTreeNode treeN = NULL;
    kd_arr = Init(arr,size);
    if (kd_arr != NULL)
    {
        srand(time(NULL));
        treeN =  initKDTreeNodeRec(kd_arr,splitM, 0);
        spKDArrayDestroy(kd_arr);
        spLoggerPrintInfo(FINISH_CREATE_KD_INFO);
    }//if
    else
    {
        spLoggerPrintError(CREATE_KD_ERR, __FILE__, __func__, __LINE__);
    }//else
    return treeN;
}//initKDTree

/**
 * Recursive method using by spKDTreeKNearestNeighbors
 * Finding k nearest neighbors of the given q_point
 *
 * @param curr - the current node of the kdTree to be searched
 * @param q - a BPQueue store closest neighbors
 * @param q_point - the query point
 *
 */
void findKNearestNeighborsRec(SPKDTreeNode curr, SPBPQueue* q, SPPoint* q_point)
{
    if(curr == NULL)
    {
        return;
    }//if
    if(isLeaf(curr))
    {
        //var deceleration
        int index;
        double distance;
        index = spPointGetIndex(curr->data);
        distance = spPointL2SquaredDistance(curr->data, q_point);
        spBPQueueEnqueue(q,index,distance);
    }//if
    else
    {
        double pointAxisCoor;
        double change;

        pointAxisCoor = spPointGetAxisCoor(q_point, curr->dim);
        change = curr->value - pointAxisCoor;
        //stage 3 - recursively search the left subtree
        if(pointAxisCoor <= curr->value)
        {
            findKNearestNeighborsRec(curr->left, q, q_point);
        }//if
        else
        {
            //recursively search the right subtree
            findKNearestNeighborsRec(curr->right, q, q_point);
        }//else

        if(!spBPQueueIsFull(q) || change*change < spBPQueueMaxValue(q))
        {
            //stage - 4 recursively search the other subtree on the next axis
            if(pointAxisCoor <= curr->value)
            {
                findKNearestNeighborsRec(curr->right, q, q_point);
            }//if
            else
            {
                findKNearestNeighborsRec(curr->left, q, q_point);
            }//else
        }//if
    }//else
}//findKNearestNeighborsRec


bool spKDTreeKNearestNeighbors(SPKDTreeNode kd_tree, SPPoint* q_point, int spknn, int* nknn)
{

    if(kd_tree == NULL || q_point ==NULL || nknn == NULL || spknn <= 0)
    {
        return false;
    }//if

    //stage one - init
    SPBPQueue* knnBPQueue = spBPQueueCreate(spknn);

    if(knnBPQueue == NULL)
    {
        spLoggerPrintError(ALLOC_QUEUE_ERR, __FILE__, __func__, __LINE__);
        return false;
    }//if
    int i;
    //stage two - find k nearest neighbors
    findKNearestNeighborsRec(kd_tree, knnBPQueue, q_point);
    for(i = 0; i < spknn; i++)
    {
        nknn[i] = spBPQueueMinIndex(knnBPQueue);
        spBPQueueDequeue(knnBPQueue);
    }//for
    spBPQueueDestroy(knnBPQueue);
    return true;
}//spKDTreeKNearestNeighbors


void spKDTreeDestroy(SPKDTreeNode kdTreeNode)
{
    if (kdTreeNode != NULL)
    {
        spKDTreeDestroy(kdTreeNode->left);
        spKDTreeDestroy(kdTreeNode->right);
        spPointDestroy(kdTreeNode->data);
        free(kdTreeNode);
    }//if
}//spKDTreeDestroy
