#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 * *
 * A bounded priority queue is similar to a regular minimum priority queue,
 * except that there is a fixed upper bound on the number of elements that
 * can be stored in the BPQ. Whenever a new element is added to the queue,
 * if the queue is at capacity, the element with the highest priority value is
 * ejected from the queue. Elements in the queue will have an int index and a double value.
 *
 *
 * The following functions are supported:
 *
 * spBPQueueCreate			- creates an empty queue with a given maximum capacity
 * spBPQueueCopy			- creates a copy of a given queue
 * spBPQueueDestroy			- frees all memory allocation associated with the queue
 * spBPQueueClear			- removes all the elements in the queue
 * spBPQueueSize			- returns the number of elements in the queue
 * spBPQueueGetMaxSize		- returns the maximum capacity of the queue
 * spBPQueueEnqueue			- Inserts a NEW COPY (must be allocated) element to the queue
 * spBPQueueDequeue			- removes the element with the lowest value
 * spBPQueuePeek			- returns a NEW COPY of the element with the lowest value
 * spBPQueuePeekLast		– returns a NEW COPY of the element with the highest value
 * spBPQueueMinValue		- returns the minimum value in the queue
 * spBPQueueMaxValue		- returns the maximum value in the queue
 * spBPQueueIsEmpty 		– returns true if the queue is empty
 * spBPQueueIsFull			- returns true if the queue is full
 *
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Creates an empty queue with a given maximum capacity.
 * This function should be called once, prior to the usage
 * of any SP BPQueue functions. It is the responsibility of the user
 * to create the queue prior to usage, and the queue
 * must be destroyed at the end of usage.
 *
 * @param maxSize - An integer represents the maximum number of elements the queue should hold.
 * @return
 * NULL -  If allocations failed or maxSize has negative value.
 * A new Queue in case of success.
 */
SPBPQueue* spBPQueueCreate(int maxSize);

/**
 * Creates a copy of a given queue
 * The new copy will contain all the elements from the source queue in the same
 * order and the properties will be initialized with the same values.
 *
 * @param source - The target queue to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A Queue containing the same elements with same order as source otherwise.
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Frees all memory allocated for the queue.
 * Do nothing if source in null
 *
 * @param source - The target queue to destroy
 */
void spBPQueueDestroy(SPBPQueue* source);

/**
 * Set default value for the queue.
 * We will put the index and the value -1 - this is our convention for clearing the queue
 * Capacity will remain the same
 * Do nothing if source in null
 *
 * @param source - The target queue to clear
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * Get the current size of the queue
 *
 * @param source - The queue to get size of
 * @return
 * -1 if source is NULL
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * A getter for the value of the target queue max size.
 *
 * @param source - The target max size.
 * @return
 * -1 in case source == NULL
 * otherwise, the max size of the target queue
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * Inserts a new element to the queue, the new element will be placed
 * according to its internal value and index, while keeping the queue sorted
 * first by elements' value in ascending order
 * and later by their index in ascending order as well.
 * Trying to insert an element to a full queue, will result in throwing
 * the highest value element between the current elements in the queue and the new one.
 *
 * @param source - The queue for which to add an element.
 * @param index - The index to insert.
 * @param value - The value to insert.
 *
 * SP_BPQUEUE_INVALID_ARGUMENT - if a NULL was sent as source or element
 * SP_BPQUEUE_OUT_OF_MEMORY - if an allocation failed
 * SP_BPQUEUE_FULL - if the queue is full and the element's value is higher
 * than all the ones in the queue (indicates the element wasn't inserted).
 *
 * SP_BPQUEUE_SUCCESS - if the element has been inserted successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * Removes the currently lowest (first) element of the queue.
 *
 * @param source - The queue for which the lowest value element will be removed.
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if queue is NULL.
 * SP_BPQUEUE_EMPTY - if the queue is empty and hence there is no element to remove.
 * SP_BPQUEUE_SUCCESS the current element was removed successfully.
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Create  a copy of the element with the lowest value
 *
 * @param source: The queue for which to get the element with the lowest value.
 * @param res: the pointer which will contain the the smalles element in the queue
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if queue is NULL.
 * SP_BPQUEUE_EMPTY - if the queue is empty
 * SP_BPQUEUE_SUCCESS if we successfully update the pointer res to the element with the lowest value.
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Create  a copy of the element with the highest value
 *
 * @param source The queue for which to get the element with the highest value.
 * @param res: the pointer which will contain the the smalles element in the queue
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT - if queue is NULL.
 * SP_BPQUEUE_EMPTY - if the queue is empty
 * SP_BPQUEUE_SUCCESS if we successfully update the pointer res to the element with the highest value.
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);


/**
 * A getter for the min value of the target queue.
 *
 * @param source The target queue
 * @return
 * The min value of target source
 * -1 in case the source is NULL or queue is empty
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * A getter for the min index of the target queue.
 *
 * @param source - the target queue
 *
 * @return:
 *	 The min index of target source
 * 	-1 in case the source is NULL or queue is empty
 */
 int spBPQueueMinIndex(SPBPQueue* source) ;

/**
 * A getter for the max value of the target queue.
 *
 * @param source The target queue
 * @return
 * The max value of target source
 * -1 in case the source is NULL or queue is empty
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * Returns whether the target queue has elements or not.
 *
 * @param source - The queue for which to check emptiness.
 * @assert source!=NULL
 * @return
 * True if the queue is empty
 * False if the queue is not empty
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * Returns whether the target queue reached with its
 * elements capacity to the max size defined at its creation time or not.
 *
 * @param source - The queue for which to check fullness.
 * @assert source!=NULL
 * @return
 * True if the queue is full
 * False if the queue is not full
 */
bool spBPQueueIsFull(SPBPQueue* source);

#endif
