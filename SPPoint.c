/*
 * SPPoint.c
 *
 *  Created on: Dec 13, 2016
 *      Author: Noy Pitterman
 */
#include "SPPoint.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct  sp_point_t
{
    double* data;
    int index;
    int dim;
};

SPPoint* spPointCreate(double* data, int dim, int index)
{
    int i;
    //* NULL in case  data is NULL OR dim <=0 OR index <0
    if (data==NULL || dim <= 0 || index < 0)
    {
            return NULL;
    }//if
    SPPoint* point=(SPPoint*)malloc(sizeof(*point));

    //* NULL in case allocation failure
    if(point==NULL )
    {
        return NULL;
    }//if

    point->data = (double*) malloc(dim * sizeof(double));
    //* NULL in case allocation failure
    if (point->data == NULL)
    {
        free(point);
        return NULL;
    }//if

    point->dim = dim;
    point->index = index;
    for (i = 0; i < dim; i++)
    {
        point->data[i] = data[i];
    }//for
    return point;
}//spPointCreate

SPPoint* spPointCopy(SPPoint* source)
{
    //Check if the source is NULL
    assert(source != NULL);
    SPPoint* point=spPointCreate(source->data, source->dim, source->index);
    return point;
}//spPointCopy

void spPointDestroy(SPPoint* point)
{
    //Check if the point is NULL
    if(point!=NULL)
    {
        free(point->data);
        free(point);
    }//if
}//spPointDestroy
int spPointGetDimension(SPPoint* point)
{
    //Check if the point is NULL
    assert(point != NULL);
    return (point->dim);
}//spPointGetDimension

double spPointGetAxisCoor(SPPoint* point, int axis)
{
    //Check if  aix>=0 && aix< point dimension && point is not NULL
    assert(axis >= 0);
    assert( axis < point->dim);
    assert (point!=NULL);

    return (point->data[axis]);
}//spPointGetAxisCoor

int spPointGetIndex(SPPoint* point)
{
    assert(point != NULL);
        return (point->index);
}//spPointGetIndex

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q)
{
    int i; //for index
    double temp; //contain the squared distance between p and q
    assert(p!=NULL && q!=NULL && p->dim==q->dim);
    double distance=0;
    for(i=0;i<p->dim;i++)
    {
        temp=(p->data[i]- q->data[i])*(p->data[i] - q->data[i]);
        distance=distance+temp;
    }//for
    return distance;
}//spPointL2SquaredDistance
void sppointPrinty(SPPoint* p)
{
    int i;
    printf("dim: ");
    printf("%d",p->dim);
    printf("    ");
    for(i=0;i<p->dim;i++)
    {
        printf("%lf",p->data[i]);
        printf(" ");
    }
    printf("/n");
}
