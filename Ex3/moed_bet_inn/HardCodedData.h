// HardCodedData.h

#pragma once

// Constants -------------------------------------------------------------------

#define MAX_LINE_LENGTH 52
#define LEN_FILE_NAME_RESIDENTS_NAMES 11
#define MAX_NAME_INPUT 21
#define MAX_ROOM_NUM 5
#define MAX_RESIDENT_NUM 15
#define THERAD_TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55
#define BOOKLOG_FILE_ADDRESS_LEN 14
#define FILE_NAME_RESIDENTS_NAMES "/names.txt"
#define FILE_NAME_ROOMS_NAMES "/rooms.txt"
#define FILE_NAME_BOOK_LOG "/roomLog.txt"
#define ENTER_ROOM " IN "
#define EXIT_ROOM " OUT "
static const int ERR_CODE_DEFAULT = 0;
static const int ERR_CODE_NOT_ENOUGH_ARGUMENTS = -5;
static const int ERR_CODE_ALLOCCING_MEMORY = -6;
static const int ERR_CODE_OPEN_FILE = -7;
static const int ERR_CODE_OPEN_SEMAPHORE = -8;
static const int ERR_CODE_THERAD = -2;
static const int ERR_CODE_CREAT_THEARD = -3;
static const int ERR_CODE_CLOSE_THERAD = -4;
