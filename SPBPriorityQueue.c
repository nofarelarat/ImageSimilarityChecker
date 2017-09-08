/*
 * SPBPriorityQueue.c
 *
 *  Created on: Dec 16, 2016
 *      Author: NofarE
 */

#include <stdbool.h>
#include <assert.h>
#include "SPBPriorityQueue.h"
#include <stdlib.h>


struct sp_bp_queue_t
{

	BPQueueElement* arr;
	int maxSize;
	int realSize;
	int realStart;
};
SPBPQueue* spBPQueueCreate(int maxSize)
{
	int i;
	//Argument's Check
	if (maxSize <= 0)
	{
		return NULL;
	}//if

	//Creating the Queue
	SPBPQueue* res;
	res = (SPBPQueue*) malloc(sizeof(*res));
	//Allocation failure - need to free(res)
	if (res == NULL)
	{
		free(res);
		return NULL;
	}//if
	res->arr = (BPQueueElement*) malloc(sizeof(BPQueueElement) * maxSize);
	//Allocation failure - need to free(res)
	if (res->arr == NULL)
	{
		free(res);
		return NULL;
	}//if
	res->maxSize  = maxSize;
	res->realSize = 0;
	res->realStart= 0;

	//Initialize Arr
	for(i=0;i<maxSize;i++)
	{
		res->arr[i].index=-1;
		res->arr[i].value=-1;
	}//for
return res;
}//spBPQueueCreate

SPBPQueue* spBPQueueCopy(SPBPQueue* source)
{
	int i;
	//Argument's Check
	if (source == NULL)
	{
		return NULL;
	}//if
	SPBPQueue* copy = spBPQueueCreate(source->maxSize);

	for(i=source->realStart;i<source->realSize;i++)
	{
		copy->arr[i].index = source->arr[i].index;
		copy->arr[i].value = source->arr[i].value;
	}//for
	copy->realSize = source->realSize;
	copy->realStart = source->realStart;
	return copy;
}//spBPQueueCopy

void spBPQueueDestroy(SPBPQueue* source)
{
	//Argument's Check
	if(source == NULL)
	{
		return ;
	}//if

	free(source->arr);
	free(source);
}//spBPQueueDestroy

void spBPQueueClear(SPBPQueue* source)
{
	int i=0;
	//Argument's Check
	if(source == NULL)
	{
		return;
	}//if
	if(source->arr == NULL)
	{
		return;
	}//if
	//Initialize Arr
	for (i=0;i<source->realSize;i++)
	{
		source->arr[i].index=-1;
		source->arr[i].value=-1;
	}//for
	source->realSize =0;
	source->realStart=0;
}//spBPQueueClear

int spBPQueueSize(SPBPQueue* source)
{
	//Argument's Check
	if(source == NULL)
	{
		return -1;
	}//if
	//Calculate the Real size
	int result=source->realSize - source->realStart;
	return (result);
}//spBPQueueSize

int spBPQueueGetMaxSize(SPBPQueue* source)
{
	//Argument's Check
	if(source == NULL)
	{
		return -1;
	}//if
	return source->maxSize;
}//pBPQueueGetMaxSize

/**
 * Insert the element in the correct place in the arr
 * by shifting the arr left until we get to the right place
 * to insert the elemnt
 *
 * @param source	- The target queue
 * @param index 	- The index of the new element
 * @param value 	- The value of the new element
 * @param start 	- That start point of the shift
 * @param end 		- That end point of the shift
 * @return
 * No return value, just updating the queue
 *
 */
void spBPQueueValueismin(SPBPQueue* source, int index, double value, int end, int start)
{
	int j;
	for(j=end;j>start;j--)
	{
		source->arr[j] = source->arr[j-1];
	}//for
	source->arr[start].index = index;
	source->arr[start].value = value;
}//spBPQueueValueismin

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value)
{
	int location=0; //location in the queue
	int end=0;      //where the queue ends
	int k=0;        //index that is used in loop
	int i = 0;     //check the "real size" after the shift

	//Argument's Check
	if(!source)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}//if
	if(value < 0)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}//if
	SP_BPQUEUE_MSG msg= SP_BPQUEUE_SUCCESS;

	//case a: move left in case that there was a delete
	 if(source->realStart!=0)
		{
			SPBPQueue* copy = spBPQueueCreate(source->maxSize);
			for(k=source->realStart;k<source->realSize;k++)
			{
				copy->arr[i].index = source->arr[k].index;
				copy->arr[i].value = source->arr[k].value;
				i = i+1;
			}//for
			spBPQueueClear(source);
			for(k=0;k<i;k++)
			{
				source->arr[k].index = copy->arr[k].index;
				source->arr[k].value = copy->arr[k].value;
			}//for
			source->realSize = i;
			source->realStart=0;
			spBPQueueDestroy(copy);
		}//if

	//case b: insert at the top if the array is empty
	 if (source->realSize == 0 || source->realSize==source->realStart)
	{
		source->arr[0].index = index;
		source->arr[0].value = value;
		source->realSize = 1;
		source->realStart = 0;
	}
	//case c: if value is bigger then the biggest
	else if (value >= source->arr[source->realSize-1].value)
	{
		//case c1: If the array is full
		if(source->realSize == source->maxSize)
		{
			msg = SP_BPQUEUE_FULL;
		}//if
		//case c2: if array is not full
		else
		{
			source->arr[source->realSize].index = index;
			source->arr[source->realSize].value = value;
			source->realSize = source->realSize +1;
		}//else
	}
	//case d: the value should be insert to the middle
	else
	{
		//iterate the array to find where to insert
		while (value > source->arr[location].value)
		{
			 location=location+1;
		}//while

		//checking if there is more than one elemnt with the same value and
		//sort by index
		while (location < source->maxSize && value == source->arr[location].value && index > source->arr[location].index)
		{
			location++;
		}//while

		//case d1: if the array is full and we don't need to insert the new element
		//location mean where to insert
		if(location == source->maxSize)
		{
			msg = SP_BPQUEUE_FULL;
		}//if

		//case d2:if the array is full(realSize=maxSize)
		else if (source->realSize == source->maxSize)
		{
			end = source->realSize-1;
		}//else if

		//case d3: if the array is not full
		else
		{
			end =source->realSize;
		}//else

		spBPQueueValueismin(source, index, value,end,location);
		//we did switch when the array wasn't full
		if(source->realSize!=source->maxSize)
		{
			source->realSize = source->realSize+1;
		}//if
	}//else
	return msg;
}//spBPQueueEnqueue

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source)
{
	//Argument's Check
	if(source!=NULL)
	{
		if(source->arr!=NULL)
		{
			if(spBPQueueIsEmpty(source))
			{
				return SP_BPQUEUE_EMPTY;
			}//if
			else
			{
				//mark as deleted
				source->arr[source->realStart].index=-1;
				source->arr[source->realStart].value=-1;
				//move the pointer
				source->realStart=source->realStart+1;
				return SP_BPQUEUE_SUCCESS;
			}//else
		}//if
	}//if
	return SP_BPQUEUE_INVALID_ARGUMENT;
}//spBPQueueDequeue

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res)
{
	//Argument's Check
	if(source == NULL)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	//case a: queue is empty
	if(spBPQueueIsEmpty(source))
	{
		return SP_BPQUEUE_EMPTY;
	}//if

	//case b: queue is not empty and we successfully indicate to the min value.
	else
	{
		res->index=source->arr[source->realStart].index;
		res->value=source->arr[source->realStart].value;
		return SP_BPQUEUE_SUCCESS;
	}//else
	return SP_BPQUEUE_INVALID_ARGUMENT;
}//spBPQueuePeek

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res)
{
	//Argument's Check
	if(source == NULL)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}//if
	if(source->arr != NULL)
	{
		//case a:queue is empty
		if(spBPQueueIsEmpty(source))
		{
			return SP_BPQUEUE_EMPTY;
		}//if

		//case b:queue is not empty and we successfully indicate to the max value.
		else
		{
			res->index=source->arr[source->realSize-1].index;
			res->value=source->arr[source->realSize-1].value;
			return SP_BPQUEUE_SUCCESS;
		}//else
	}//if
	return SP_BPQUEUE_INVALID_ARGUMENT;
}//spBPQueuePeekLast

double spBPQueueMinValue(SPBPQueue* source)
{
	//Argument's Check
	if (source != NULL)
	{
		if (spBPQueueIsEmpty(source))
		{
			return -1.0;
		}//if
		else
		{
			return(source->arr[source->realStart].value);
		}//else
	}//if
	return -1.0;
}//spBPQueueMinValue

/**
 * Returns the index of the element with the lowest value
 *
 * @param source - the queue (must be initialized)
 *
 * @return	-  -1 in case of invalid argument or empty queue. We assume that the values are positive.
 * 			-  else the index of the element with the lowest value
 */
int spBPQueueMinIndex(SPBPQueue* source)
{
	if (source != NULL)
	{
		if (spBPQueueIsEmpty(source))
		{
			return -1;
		}//if
		else
		{
			return(source->arr[source->realStart].index);
		}//else
	}//if
	return -1;
}//spBPQueueMinIndex

double spBPQueueMaxValue(SPBPQueue* source)
{
	//Argument's Check
	if (source != NULL)
	{
		if (spBPQueueIsEmpty(source))
		{
			return -1;
		}//if
		else
		{
			return(source->arr[source->realSize-1].value);
		}//else
	}//if
	return -1;
}

bool spBPQueueIsFull(SPBPQueue* source)
{
	//Argument's Check
	assert(source != NULL);
	int tmp = source->realSize - source->realStart;
	if(tmp ==source->maxSize)
	{
		return true;
	}//if
	return false;
}//spBPQueueIsFull

bool spBPQueueIsEmpty(SPBPQueue* source)
{
	//Argument's Check
	assert(source != NULL);
	return ((source->realSize-source->realStart) == 0);
}//spBPQueueIsEmpty
