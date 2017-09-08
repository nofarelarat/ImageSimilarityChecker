/*
 * SPKDTree.h
 *
 *  Created on: 17.3.2017
 *      Author: NofarE
 */


#include "SPConfig.h"
#include "SPPoint.h"

#ifndef KDTREE_H_
#define KDTREE_H_

// defining the KD Tree
typedef struct sp_kd_tree_node_t* SPKDTreeNode;

/**
 * Creates a KD-Tree with arr, and returns the root node.
 * The complexity of this operation is O(d*nlogn) had expected
 * where d is point's dimension and n is the size
 *
 * @param arr - the data
 * @param size - the size of the array
 * @param splitM - the split method
 *
 * @return  - pointer to KDTreeNode
 *          - NULL in case of a failure
 */
SPKDTreeNode initKDTree(SPPoint** arr, int size, splitMethod splitM);

/**
 * Freeing all memory related to kdTree
 *
 * @param kdTreeNode - the root of the kdTree we want to destroy
 */
void spKDTreeDestroy(SPKDTreeNode kdTreeNode);

/**
 * Finding k nearest neighbors of the given query point
 *
 * @param kd_tree - the root node of the kdTree to be searched
 * @param q_point - the query point
 * @param spknn - the number of the nearest neighbors we find
 * @paran nknn - the array to contain the indexes of the found nearest points
 *
 * @return
 * - if one of the parameters is invalid return false (kdTree, query or knnIndexes = NULL or knn_num <= 0)
 */
bool spKDTreeKNearestNeighbors(SPKDTreeNode kd_tree, SPPoint* q_point, int spknn, int* nknn);

/**
 * @param node - the current node we want to check
 * Returns true if the given node is a leaf and false otherwise
 */
bool isLeaf(SPKDTreeNode node);

#endif /* KDTREE_H_ */
