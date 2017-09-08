/*
 * sp_bpqueue_unit_test.c
 *
 *  Created on: Dec 20, 2016
 *      Author: NofarE
 */
#include "../SPBPriorityQueue.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include "unit_test_util.h"
#include <stdlib.h>
#include "main_test.h"

static bool testQueueCreate()
{
    int size = 1;

    //Check NULL condition
    ASSERT_TRUE(spBPQueueCreate(-1) == NULL);
    //Create a new queue
    SPBPQueue* queue = spBPQueueCreate(size);
    ASSERT_TRUE(queue != NULL);
    //Check queue size and max size
    ASSERT_TRUE(spBPQueueGetMaxSize(queue) == size);
    ASSERT_TRUE(spBPQueueSize(queue) == 0);
    //Free queue memory
    spBPQueueDestroy(queue);
    return true;
}//testQueueCreate

static bool testQueueCopy()
{
    int size = 4;
    //Check NULL condition
    ASSERT_TRUE(spBPQueueCopy(NULL) == NULL);
    //Create queue and a copy of queue
    SPBPQueue* queue = spBPQueueCreate(size);
    SPBPQueue* copy = spBPQueueCopy(queue);
    ASSERT_TRUE(copy != NULL);

    //Check queues size and max size
    ASSERT_TRUE(spBPQueueSize(queue) == 0);
    ASSERT_TRUE(spBPQueueSize(copy) == 0);
    ASSERT_TRUE(spBPQueueGetMaxSize(queue) == size);
    ASSERT_TRUE(spBPQueueGetMaxSize(copy) == size);

    //Insert to queue
    spBPQueueEnqueue(queue, 1, 1.0);
    spBPQueueEnqueue(queue, 2, 2.0);
    spBPQueueEnqueue(queue, 3, 3.0);
    spBPQueueEnqueue(queue, 4, 4.0);

    //Check that copy size didn't change
    ASSERT_TRUE(spBPQueueSize(queue) == 4);
    ASSERT_TRUE(spBPQueueSize(copy)  == 0);

    //Check that copy size has changed
    spBPQueueDestroy(copy);
    copy = spBPQueueCopy(queue);
    ASSERT_TRUE(spBPQueueSize(queue) == 4);
    ASSERT_TRUE(spBPQueueSize(copy)  == 4);

    //Check that copy size didn't change
    spBPQueueDequeue(queue);
    ASSERT_TRUE(spBPQueueSize(queue) == 3);
    ASSERT_TRUE(spBPQueueSize(copy) == 4);

    //Check that both size have changed
    spBPQueueDequeue(copy);
    ASSERT_TRUE(spBPQueueSize(queue) == 3);
    ASSERT_TRUE(spBPQueueSize(copy) == 3);

    //Clear both queue
    spBPQueueClear(queue);
    spBPQueueClear(copy);

    //Check that queues were cleared
    ASSERT_TRUE(spBPQueueSize(queue) == 0);
    ASSERT_TRUE(spBPQueueSize(copy) == 0);
    ASSERT_TRUE(spBPQueueGetMaxSize(queue) == size);
    ASSERT_TRUE(spBPQueueGetMaxSize(copy) == size);

    //Destroy both queues
    spBPQueueDestroy(copy);
    spBPQueueDestroy(queue);

    return true;
}//testQueueCopy

static bool testQueueEnqueue()
{
    int size = 5;
    SPBPQueue* nullTestQueue = spBPQueueCreate(size);
    BPQueueElement* element1peek = malloc(sizeof(BPQueueElement));
    BPQueueElement* element2peek = malloc(sizeof(BPQueueElement));
    BPQueueElement* element1peeklast = malloc(sizeof(BPQueueElement));
    BPQueueElement* element2peeklast = malloc(sizeof(BPQueueElement));

    //Check NULL conditions
    ASSERT_TRUE(spBPQueueEnqueue(NULL,1,1.0) == SP_BPQUEUE_INVALID_ARGUMENT);
    ASSERT_TRUE(spBPQueueEnqueue(nullTestQueue, 7,-9.8) == SP_BPQUEUE_INVALID_ARGUMENT);

    //Clear null test objects
    spBPQueueDestroy(nullTestQueue);

    //Create 2 queues
    SPBPQueue* queue1 = spBPQueueCreate(size);
    SPBPQueue* queue2 = spBPQueueCreate(size);

    //Add the elements to queue1
    spBPQueueEnqueue(queue1,1, 1.0);
    spBPQueueEnqueue(queue1,2, 2.0);
    spBPQueueEnqueue(queue1,3, 3.0);
    spBPQueueEnqueue(queue1,4, 4.0);

    //Add the elements to queue2
    spBPQueueEnqueue(queue2,1, 1.0);
    spBPQueueEnqueue(queue2,2, 2.0);
    spBPQueueEnqueue(queue2,3, 3.0);
    spBPQueueEnqueue(queue2,4, 4.0);

    //Check sizes
    ASSERT_TRUE(spBPQueueSize(queue1) == 4 && spBPQueueSize(queue1) == spBPQueueSize(queue2));

    //Check min max value
    ASSERT_TRUE(spBPQueueMaxValue(queue1) == spBPQueueMaxValue(queue2));
    ASSERT_TRUE(spBPQueueMinValue(queue1) == spBPQueueMinValue(queue2));

    spBPQueuePeek(queue1, element1peek);
    spBPQueuePeek(queue2,element2peek);
    spBPQueuePeekLast(queue1,element1peeklast);
    spBPQueuePeekLast(queue2,element2peeklast);

    //Check that both queues are equal
    ASSERT_TRUE((element1peek->index) == (element2peek->index));
    ASSERT_TRUE((element1peek->value) == (element2peek->value));
    ASSERT_TRUE((element1peeklast->index) == (element2peeklast->index));
    ASSERT_TRUE((element1peeklast->value) == (element2peeklast->value));

    free(element1peek);
    free(element2peek);
    free(element1peeklast);
    free(element2peeklast);

    element1peek = malloc(sizeof(BPQueueElement));
    element2peek = malloc(sizeof(BPQueueElement));
    element1peeklast = malloc(sizeof(BPQueueElement));
    element2peeklast = malloc(sizeof(BPQueueElement));

    //Add min value to queue1
    spBPQueueEnqueue(queue1,1, 0.5);

    //Check min max value
    ASSERT_TRUE(spBPQueueMaxValue(queue1) == spBPQueueMaxValue(queue2));
    ASSERT_TRUE(spBPQueueMinValue(queue1) != spBPQueueMinValue(queue2));
    spBPQueuePeek(queue1,element1peek);
    spBPQueuePeek(queue2,element2peek);
    spBPQueuePeekLast(queue1,element1peeklast);
    spBPQueuePeekLast(queue2,element2peeklast);

    //Check that both queues are equal
    ASSERT_TRUE((element1peek->index) == (element2peek->index));
    ASSERT_TRUE((element1peek->value) != (element2peek->value));
    ASSERT_TRUE((element1peeklast->index) == (element2peeklast->index));
    ASSERT_TRUE((element1peeklast->value) == (element2peeklast->value));

    free(element1peek);
    free(element2peek);
    free(element1peeklast);
    free(element2peeklast);

    spBPQueueEnqueue(queue1,1, 0.2);

    element1peek = malloc(sizeof(BPQueueElement));
    element2peek = malloc(sizeof(BPQueueElement));
    element1peeklast = malloc(sizeof(BPQueueElement));
    element2peeklast = malloc(sizeof(BPQueueElement));

    //Check min max value
    ASSERT_TRUE(spBPQueueMaxValue(queue1) != spBPQueueMaxValue(queue2));
    ASSERT_TRUE(spBPQueueMinValue(queue1) != spBPQueueMinValue(queue2));

      spBPQueuePeek(queue1, element1peek);
      spBPQueuePeek(queue2,element2peek);
      spBPQueuePeekLast(queue1,element1peeklast);
      spBPQueuePeekLast(queue2,element2peeklast);

    //Check that both queues are equal
      ASSERT_TRUE((element1peek->index) == (element2peek->index));
      ASSERT_TRUE((element1peek->value) != (element2peek->value));
      ASSERT_TRUE((element1peeklast->index) != (element2peeklast->index));
      ASSERT_TRUE((element1peeklast->value) != (element2peeklast->value));
      free(element1peek);
      free(element2peek);
      free(element1peeklast);
      free(element2peeklast);

    //Free queues memory
    spBPQueueDestroy(queue1);
    spBPQueueDestroy(queue2);
    return true;
}//testQueueEnqueue

static bool testQueueCreateNegativeSize()
{
    ASSERT_TRUE(spBPQueueCreate(-1) == NULL);
    return true;
}//testQueueCreateNegativeSize

static bool testGetNullQueueSize()
{
    ASSERT_TRUE(spBPQueueSize(NULL) == -1);
    return true;
}// testGetNullQueueSize

static bool testQueueDequeue()
{
    SPBPQueue* queue;
    queue = spBPQueueCreate(2);

    ASSERT_TRUE(spBPQueueDequeue(NULL) == SP_BPQUEUE_INVALID_ARGUMENT);
    ASSERT_TRUE(spBPQueueDequeue(queue) == SP_BPQUEUE_EMPTY);

    ASSERT_TRUE(spBPQueueEnqueue(queue,1 ,1 ) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueSize(queue) == 1);
    ASSERT_TRUE(spBPQueueDequeue(queue) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueSize(queue) == 0);

    spBPQueueDestroy(queue);
    return true;
}//testQueueDequeue

static bool testQueuePeek()
{
    SPBPQueue* queue;
    BPQueueElement* res = malloc(sizeof(BPQueueElement));

    ASSERT_TRUE(spBPQueuePeek(NULL,res) ==SP_BPQUEUE_INVALID_ARGUMENT);

    queue = spBPQueueCreate(10);

    ASSERT_TRUE(spBPQueueEnqueue(queue, 2,2) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueEnqueue(queue, 1,1) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueEnqueue(queue, 3,3) == SP_BPQUEUE_SUCCESS);

    spBPQueuePeek(queue,res);

    ASSERT_TRUE(res->index==1&& res->value==1.0);
    free(res);
    res = malloc(sizeof(BPQueueElement));
    spBPQueueClear(queue);

    ASSERT_TRUE(spBPQueuePeek(queue,res) == SP_BPQUEUE_EMPTY);

    free(res);
    spBPQueueDestroy(queue);
    return true;
}//testQueuePeek

static bool testQueuePeekLast()
{
    SPBPQueue* queue;
    BPQueueElement* res = malloc(sizeof(BPQueueElement));
    //maybe NULL
    ASSERT_TRUE(spBPQueuePeekLast(NULL,res) == SP_BPQUEUE_INVALID_ARGUMENT);

    queue = spBPQueueCreate(10);

    ASSERT_TRUE(spBPQueueEnqueue(queue,1,1.0) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueEnqueue(queue,2,2.0) == SP_BPQUEUE_SUCCESS);
    ASSERT_TRUE(spBPQueueEnqueue(queue,3,3.0) == SP_BPQUEUE_SUCCESS);


    spBPQueuePeekLast(queue,res);
    ASSERT_TRUE(res->index==3&& res->value==3);

    free(res);
    res = malloc(sizeof(BPQueueElement));
    spBPQueueClear(queue);

    ASSERT_TRUE(spBPQueuePeekLast(queue,res) == SP_BPQUEUE_EMPTY);
    free(res);
    spBPQueueDestroy(queue);

    return true;
}//testQueuePeekLast

static bool testQueueCapacity()
{
    int size=4;

    //Create a new queue
    SPBPQueue* queue = spBPQueueCreate(size);

    //Check spBPQueueIsEmpty
    ASSERT_TRUE(spBPQueueIsEmpty(queue) == true);

    //Add the first 4 elements to the queue
    spBPQueueEnqueue(queue, 1, 1.0);
    spBPQueueEnqueue(queue, 2, 2.0);
    spBPQueueEnqueue(queue, 3, 3.0);
    spBPQueueEnqueue(queue, 4, 4.0);

    //Check spBPQueueIsFull
    ASSERT_TRUE(spBPQueueIsFull(queue) == true);

    //erase all the 4 in the queue and re-insert them
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);

    //Add the first 4 elements to the queue
    spBPQueueEnqueue(queue, 2, 2.0);
    spBPQueueEnqueue(queue, 3, 3.0);
    spBPQueueEnqueue(queue, 8, 3.0);
    spBPQueueEnqueue(queue, 4, 4.0);

    ASSERT_TRUE(spBPQueueIsFull(queue) == true);
    spBPQueueDestroy(queue);
    return true;
}//testQueueCapacity

static bool testQueueMinMax()
{
    int size=4;
    //Create a new queue
    SPBPQueue* queue = spBPQueueCreate(size);
    ASSERT_TRUE(spBPQueueMinValue(queue) == -1.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == -1.0);


    //Add the first 4 elements
    spBPQueueEnqueue(queue, 2, 2.0);
    spBPQueueEnqueue(queue, 3, 3.0);
    spBPQueueEnqueue(queue, 4, 4.0);
    spBPQueueEnqueue(queue, 5, 5.0);

    //Check Mix Max
    ASSERT_TRUE(spBPQueueMinValue(queue) == 2.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 5.0);

    //Add new min
    spBPQueueEnqueue(queue, 1 , 1.0);
    ASSERT_TRUE(spBPQueueMinValue(queue) == 1.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 4.0);

    //Add new max
    spBPQueueEnqueue(queue, 6, 3.5);
    ASSERT_TRUE(spBPQueueMinValue(queue) == 1.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 3.5);

    //Dequeue All elements
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);
    spBPQueueDequeue(queue);

    //check if the queue is really empty
    ASSERT_TRUE(spBPQueueMinValue(queue) == -1);
    ASSERT_TRUE(spBPQueueSize(queue) == false);

    //insert one element
    spBPQueueEnqueue(queue,2,4);

    ASSERT_TRUE(spBPQueueMinValue(queue) == 4.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 4.0);
    ASSERT_TRUE(spBPQueueSize(queue) == 1);

    //insert sec elemnt
    spBPQueueEnqueue(queue,1,1);
    ASSERT_TRUE(spBPQueueMinValue(queue) == 1.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 4.0);
    ASSERT_TRUE(spBPQueueSize(queue) == 2);

    //erase one element
    spBPQueueDequeue(queue);

    ASSERT_TRUE(spBPQueueSize(queue) == 1);
    ASSERT_TRUE(spBPQueueMinValue(queue) == 4.0);
    ASSERT_TRUE(spBPQueueMaxValue(queue) == 4.0);

    //Free queue
    spBPQueueDestroy(queue);
    return true;
}//testQueueMinMax

void spRunQueue1()
{
	RUN_TEST(testQueueCreate);
    RUN_TEST(testQueueCopy);
	RUN_TEST(testQueueEnqueue);
	RUN_TEST(testQueueCreateNegativeSize);
	RUN_TEST(testGetNullQueueSize);
    RUN_TEST(testQueueDequeue);
	RUN_TEST(testQueuePeek);
	RUN_TEST(testQueuePeekLast);
	RUN_TEST(testQueueCapacity);
	RUN_TEST(testQueueMinMax);
}//spRunQueue

