#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "SPLogger.h"

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

#define PRINT_ERROR_LE {SP_LOGGER_ERROR_LEVEL, SP_LOGGER_WARNING_ERROR_LEVEL, SP_LOGGER_INFO_WARNING_ERROR_LEVEL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL}
#define PRINT_WARNING_LE {SP_LOGGER_WARNING_ERROR_LEVEL, SP_LOGGER_INFO_WARNING_ERROR_LEVEL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL}
#define PRINT_INFO_LE {SP_LOGGER_INFO_WARNING_ERROR_LEVEL, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL}
#define PRINT_DEBUG_LE {SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL}
#define PRINT_DEBUG_SE 1
#define PRINT_ERROR_SE 4
#define PRINT_WARNING_SE 3
#define PRINT_INFO_SE 2


#define INFO 		"INFO"
#define ERROR 		"ERROR"
#define WARNING 	"WARNING"
#define DEBUG	    "DEBUG"

#define DEF_FORMAT "---%s---\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n"
#define INF_FORMAT "---%s---\n- message: %s\n"
#define MSG_FORMAT "%s\n"


SP_LOGGER_MSG writeToLog(const char* level,const char* msg, const char* file,const char* function,
		const int line, SP_LOGGER_LEVEL* levels, int size);
bool isValidLoggerLevel(SP_LOGGER_LEVEL* levels, int size);

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

/**
 * Frees all memory allocated for the logger. If the logger is not defined
 * then nothing happens.
 */
void spLoggerDestroy()
{
	if (!logger)
	{
		return;
	}//if
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}//if
	free(logger);//free allocation
	logger = NULL;
}//destroy


SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line)
{
	SP_LOGGER_LEVEL validLogLevels[PRINT_ERROR_SE] = PRINT_ERROR_LE;
	return writeToLog(ERROR,msg,file, function, line,validLogLevels, PRINT_ERROR_SE);
}//spLoggerPrintError

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line)
{
	SP_LOGGER_LEVEL validLogLevels[PRINT_WARNING_SE] = PRINT_WARNING_LE;
	return writeToLog(WARNING, msg, file, function, line, validLogLevels, PRINT_WARNING_SE);
}//spLoggerPrintWarning

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg)
{
	SP_LOGGER_LEVEL validLogLevels[PRINT_INFO_SE] = PRINT_INFO_LE;
	if(logger == NULL)
	{
		return SP_LOGGER_UNDIFINED;
	}//if
	if(msg == NULL)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}//if
	if(isValidLoggerLevel(validLogLevels, PRINT_INFO_SE))
	{
		int result = fprintf(logger->outputChannel,INF_FORMAT,INFO,msg);
		if(result < 0)
		{
			return SP_LOGGER_WRITE_FAIL;
		}//if
		fflush(logger->outputChannel);
	}//if
	return SP_LOGGER_SUCCESS;
}//spLoggerPrintInfo

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line)
{
	SP_LOGGER_LEVEL validLogLevels[PRINT_DEBUG_SE] = PRINT_DEBUG_LE;
	return writeToLog(DEBUG, msg, file, function, line, validLogLevels, PRINT_DEBUG_SE);
}//spLoggerPrintDebug

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg)
{
	int result;
	if(logger == NULL)
	{
		return SP_LOGGER_UNDIFINED;
	}//if
	if(msg == NULL)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}//if
	result = fprintf(logger->outputChannel,MSG_FORMAT,msg);
	if(result < 0)
	{
		return SP_LOGGER_WRITE_FAIL;
	}//if
	fflush(logger->outputChannel);
	return SP_LOGGER_SUCCESS;
}//spLoggerPrintMsg

/**
 * get a set of valid levels, and  check if it is a valid one
 * @param levels 	- The relevant levels to check
 * @param size		- The size of levels array
 * @return true if the logger level is in the list otherwise return false
 */
bool isValidLoggerLevel(SP_LOGGER_LEVEL* levels, int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if(logger->level ==  levels[i])
		{
			return true;
		}//if
	}//for
	return false;
}//isValidLoggerLevel

/**
 * 	Prints some new line to log file
 *
 * @param level 	- The level of the log message
 * @param msg     	- The message to be printed
 * @param file    	- A string representing the filename that the error to be printed came from
 * @param function 	- A string representing the function name in which the error to be printed call ocurred
 * @param line		- A string representing the line number in which the function call occurred
 * @param size		- Represent the size of the level's array
 * @return SP_LOGGER_SUCCESS if succeeded ,SP_LOGGER_WRITE_FAIL otherwise
**/
SP_LOGGER_MSG writeToLog(const char* level,const char* msg, const char* file,const char* function,
		const int line, SP_LOGGER_LEVEL* levels, int size)
{
	if(logger == NULL)
	{
		return SP_LOGGER_UNDIFINED;
	}//if
	if(msg == NULL || file == NULL || function == NULL || line < 0)
	{
		return SP_LOGGER_INVAlID_ARGUMENT;
	}//if
	if(isValidLoggerLevel(levels, size))
	{
		int result = fprintf(logger->outputChannel,DEF_FORMAT,level,file,function,line,msg);
		if(result < 0)
		{
			return SP_LOGGER_WRITE_FAIL;
		}//if
		fflush(logger->outputChannel);
	}//if
	return SP_LOGGER_SUCCESS;
}//writeToLog
