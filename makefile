CC = gcc
CPP = g++
#put all your object files here
OBJS = main.o SPImageProc.o SPPoint.o KDTree.o KDArray.o SPConfig.o SPLogger.o SPBPriorityQueue.o main_aux.o SPConfExtract.o
#The executabel filename
EXEC = SPCBIR
INCLUDEPATH=/usr/local/lib/opencv-3.1.0/include/
LIBPATH=/usr/local/lib/opencv-3.1.0/lib/
LIBS=-lopencv_xfeatures2d -lopencv_features2d \
-lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_core


CPP_COMP_FLAG = -std=c++11 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

C_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors -DNDEBUG

$(EXEC): $(OBJS)
	$(CPP) $(OBJS) -L$(LIBPATH) $(LIBS) -o $@
main.o: main.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPConfExtract.h KDTree.h unit_tests/main_test.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c++ source file
#use g++ -MM SPImageProc.cpp to see dependencies
SPImageProc.o: SPImageProc.cpp SPImageProc.h SPConfig.h SPPoint.h SPLogger.h SPConfExtract.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
#a rule for building a simple c source file
#use "gcc -MM SPPoint.c" to see the dependencies
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(C_COMP_FLAG) -c $*.c
KDTree.o: KDTree.c SPBPriorityQueue.h KDArray.h SPPoint.h KDTree.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c 
KDArray.o: KDArray.c KDArray.h SPPoint.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPLogger.o: SPLogger.c SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h
	$(CC) $(C_COMP_FLAG) -c $*.c
main_aux.o: main_aux.cpp main_aux.h SPImageProc.h SPConfig.h SPLogger.h SPPoint.h SPConfExtract.h KDTree.h unit_tests/main_test.h
	$(CPP) $(CPP_COMP_FLAG) -I$(INCLUDEPATH) -c $*.cpp
SPConfExtract.o: SPConfExtract.c SPConfExtract.h SPPoint.h SPConfig.h SPLogger.h
	$(CC) $(C_COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
