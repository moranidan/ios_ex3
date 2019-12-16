// HardCodedData.h

#pragma once

// Constants -------------------------------------------------------------------

#define MAX_INPUT_LINE_LENGTH 52
#define LEN_FILE_NAME_RESIDENTS_NAMES 11
#define MAX_NAME_INPUT 21
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55
#define RELATIV_PATH_PREFIX "./"
#define FILE_NAME_RESIDENTS_NAMES "/names.txt"
#define FILE_NAME_FINAL_GRADES "/final_grades.txt"
#define FILE_NAME_STUDENT_GRADE "/final_"
#define FOLDER_NAME_FOR_STUDENT  "/grades_"
#define CALL_TO_TEST_GRADE "TestGrade.exe "
#define TEXT_FILE_SUFFIX ".txt"
static const int ERR_CODE_DEFAULT = 0;
static const int ERR_CODE_NOT_ENOUGH_ARGUMENTS = -5;
static const int ERR_CODE_CANT_FIND_DIR = -6;
static const int ERR_CODE_OPEN_FILE = -7;
static const int ERR_CODE_OPEN_PROCESS = -8;