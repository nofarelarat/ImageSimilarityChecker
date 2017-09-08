#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "main_test.h"
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPLogger.h"

// This is a helper function which checks if two files are identical
static bool identicalFiles(const char* fname1, const char* fname2)
{
	FILE *fp1, *fp2;
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");
	char ch1 = EOF, ch2 = EOF;
	if (fp1 == NULL) {
		return false;
	} else if (fp2 == NULL) {
		fclose(fp1);
		return false;
	} else {
		ch1 = getc(fp1);
		ch2 = getc(fp2);
		while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
			ch1 = getc(fp1);
			ch2 = getc(fp2);
		}
		fclose(fp1);
		fclose(fp2);
	}
	if (ch1 == ch2) {
		return true;
	} else {
		return false;
	}
}

//Logger is not defined
static bool basicLoggerTest() {
	ASSERT_TRUE(spLoggerPrintError("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	spLoggerDestroy();
	return true;
}

//Only Errors should be printed
static bool basicLoggerErrorTest() {
	const char* expectedFile = "basicLoggerErrorTestExp.log";
	const char* testFile = "basicLoggerErrorTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile,SP_LOGGER_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//All messages should be printed in debug level
static bool basicLoggerDebugTest() {
	const char* expectedFile = "basicLoggerDebugTestExp.log";
	const char* testFile = "basicLoggerDebugTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile,SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}
//common function for writing to log
static bool commonTest(const char* function, const char* expectedFile,
	const char* testFile, SP_LOGGER_LEVEL level) {
	bool result = true;
	ASSERT_TRUE(spLoggerCreate(testFile, level) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",function,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB","sp_logger_unit_test.c",function,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD","sp_logger_unit_test.c",function,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("MSGE") == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return result;
}
//Only Errors should be printed
static bool loggerErrorTest() {
	const char* expectedFile = "loggerErrorTestExp.log";
	const char* testFile = "loggerErrorTest.log";
	return commonTest(__func__, expectedFile, testFile, SP_LOGGER_ERROR_LEVEL);
}
//Only Warnings should be printed
static bool loggerWarningTest() {
	const char* expectedFile = "loggerWarningTestExp.log";
	const char* testFile = "loggerWarningTest.log";
	return commonTest(__func__, expectedFile, testFile, SP_LOGGER_WARNING_ERROR_LEVEL);
}
//Only Info should be printed
static bool loggerInfoTest() {
	const char* expectedFile = "loggerInfoTestExp.log";
	const char* testFile = "loggerInfoTest.log";
	return commonTest(__func__, expectedFile, testFile, SP_LOGGER_INFO_WARNING_ERROR_LEVEL);
}
//All messages should be printed in debug level
static bool loggerDebugTest() {
	const char* expectedFile = "loggerDebugTestExp.log";
	const char* testFile = "loggerDebugTest.log";
	return commonTest(__func__, expectedFile, testFile, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL);
}
//checking writing log to stdout
static bool checkWriteToScreen() {
	//Write to stdout
	ASSERT_TRUE(spLoggerCreate(NULL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("MSGE") == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	return true;
}
//checking writing to log with invalid arguments
static bool checkInvalidArguments() {
	ASSERT_TRUE(spLoggerCreate(NULL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	//Error
	ASSERT_TRUE(spLoggerPrintError(NULL,"sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSGA",NULL,__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",NULL,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSGA","sp_logger_unit_test.c",__func__,-1) == SP_LOGGER_INVAlID_ARGUMENT);
	//Warning
	ASSERT_TRUE(spLoggerPrintWarning(NULL,"sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSGA",NULL,__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSGA","sp_logger_unit_test.c",NULL,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSGA","sp_logger_unit_test.c",__func__,-1) == SP_LOGGER_INVAlID_ARGUMENT);
	//Info
	ASSERT_TRUE(spLoggerPrintInfo(NULL) == SP_LOGGER_INVAlID_ARGUMENT);
	//Debug
	ASSERT_TRUE(spLoggerPrintDebug(NULL,"sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSGA",NULL,__func__,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSGA","sp_logger_unit_test.c",NULL,__LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSGA","sp_logger_unit_test.c",__func__,-1) == SP_LOGGER_INVAlID_ARGUMENT);
	//Message
	ASSERT_TRUE(spLoggerPrintMsg(NULL) == SP_LOGGER_INVAlID_ARGUMENT);
	spLoggerDestroy();
	return true;
}
//Checking logger edge cases
static bool loggerFilesTest() {
	//Logger is not defined
	ASSERT_TRUE(spLoggerPrintError("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintWarning("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintInfo("A") == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintDebug("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintMsg("A") == SP_LOGGER_UNDIFINED);
	//Invalid file name
	ASSERT_TRUE(spLoggerCreate("", SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_CANNOT_OPEN_FILE);
	spLoggerDestroy();
	return true;
}

void spRunLogger()
{
	RUN_TEST(basicLoggerTest);
	RUN_TEST(basicLoggerErrorTest);
	RUN_TEST(basicLoggerDebugTest);
	RUN_TEST(loggerErrorTest);
	RUN_TEST(loggerWarningTest);
	RUN_TEST(loggerInfoTest);
	RUN_TEST(loggerDebugTest);
	RUN_TEST(checkInvalidArguments);
	RUN_TEST(loggerFilesTest);
	RUN_TEST(checkWriteToScreen);
}//spRunLogger
