/*
 * main_test.c
 *
 *  Created on: Apr 2, 2017
 *      Author: Noy Pitterman
 */
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "main_test.h"

void spRunTest()
{
	//Run points Tester
	printf("***************RUN POINTS TEST***************\n");
	spRunPoint1();
	//Run Queue Tester
	printf("***************RUN Queue TEST***************\n");
	spRunQueue1();
	//Run Logger Tester
	printf("***************RUN Logger TEST******************\n");
	spRunLogger();
	//Run Config Tester
	printf("***************RUN Config TEST******************\n");
	spRunConfig();
	//Run KD Array Tester
	printf("***************RUN Array TEST******************\n");
	spRunKdArray();
	//Run KD Tree Tester
	printf("***************RUN Tree TEST******************\n");
	spRunKDTree();
}//spRunTest

int main()
{
	spRunTest();
	return 0;
}//main
