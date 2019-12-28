// moed_bet_inn.h

// This header consists the structs and function declerations for all the functions used in the project, and their documentation.

#pragma once

#include "HardCodedData.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>

// Types -----------------------------------------------------------------------


/* struct for rooms list
	-> char name[] - Room's name. initialized when opening rooms.txt file.
	-> int price_per_night - integer for the cost for a night in the room.  initialized when opening rooms.txt file.
	-> int nums_of_same_room - integer for the number of rooms for the same type.  initialized when opening rooms.txt file.
	-> HANDLE room_full - a semaphore that contain the number of people in the room.  created in main, initialized when open file. 
	*/
typedef struct
{
	char name[MAX_NAME_INPUT];
	int price_per_night;
	int nums_of_same_room;
	HANDLE room_full;
} ROOM;

/* struct for residents list
	-> char name[] - Resident's name. initialized when opening names.txt file.
	-> int  my_budget - integer for the budget of each resident.  initialized when opening names.txt file.
	-> int my_room_num - integer for the location to the room that the resident can go to in the p_rooms array.  initialized when opening names.txt file.
	-> int room_days - integer for the number of days that the resident can pay for the room.  initialized when opening names.txt file.
	*/
typedef struct
{
	char name[MAX_NAME_INPUT];
	int my_budget;
	int my_room_num; 
	int room_days;  
} RESIDENT;


/* struct for residents therad parameters
	-> ROOM p_rooms - a pointer to the struct that containts the list of rooms.
	-> RESIDENT p_resident - a pointer to an object (a resident) in the struct that contaits the list of residents.
	-> char main_folder_path - a string contains the path to the main folder.
	-> int p_days - a pointer to the global varibel that contains the day number.
	-> int p_exits_residents - a pointer to the global varibel that contains the number of residents that exit from the hotel.
	-> FILE pf_roomlog - a pointer to room log file.
	*/
typedef struct
{
	ROOM *p_rooms;
	RESIDENT p_resident;
	char *main_folder_path;
	int *p_days;
	int *p_exits_residents;
	FILE *pf_roomlog;
} resident_thread_params;

/* struct for promote days therad parameters
	-> ROOM p_rooms - a pointer to the struct that containts the list of rooms.
	-> RESIDENT p_resident - a pointer to an object (a resident) in the struct that contaits the list of residents.
	-> int residents_num - integer for residents number.
	-> int p_exits_residents - a pointer to the global varibel that contains the number of residents that exit from the hotel.
	*/
typedef struct  
{
	ROOM *p_rooms;
	RESIDENT *p_residents; int *p_days;
	int residents_num;
	int *p_exits_residents;
} promote_days_thread_params;



// Function Declarations -------------------------------------------------------

/*
This function initialize the residents list by reading from names.txt file.
Input:  char *main_folder_path - a string contains the path to the folder with names.txt file.
		RESIDENT *p_residents - a pointer to the resident struct that will contains all the information about the residents.
		ROOM *p_rooms - a pointer to the room struct that will contains all the information about the rooms.
		int rooms_num - integar for the number of rooms.
		int *residents_num - a pointer to the resident num integer that contains the number of residnts.
Output: int, success or error code
*/
int initialization_residents(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, int rooms_num, int *residents_num);

/*
This function initialize the rooms list by reading from names.txt file.
Input:  char *main_folder_path - a string contains the path to the folder with names.txt file.
		RESIDENT *p_residents - a pointer to the resident struct that will contains all the information about the residents.
		ROOM *p_rooms - a pointer to the room struct that will contains all the information about the rooms.
		int rooms_num - integar for the number of rooms.
		int *residents_num - a pointer to the resident num integer that contains the number of residnts.
Output: int, success or error code
*/
int initialization_rooms(char *main_folder_path, ROOM *p_rooms, int *rooms_num);

/*
This function initialize the resident thread parmeters.
Input:  char *main_folder_path - a string contains the path to the folder with names.txt file.
		RESIDENT *p_residents - a pointer to the resident struct that will contains all the information about the residents.
		ROOM *p_rooms - a pointer to the room struct that will contains all the information about the rooms.
		resident_thread_params *p_resident_thread_params - a pointer to the resident_therad parameters.
		int residents_num - integer that contains the number of residnts.
		int *days - a pointer to the days integer that contains the day 'today' .
		int *exits_residents - a pointer to the exit residents integer that contains the number of residents that left the hotel.
Output: void
*/
void initialization_p_resident_thread_params(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, resident_thread_params *p_resident_thread_params, int residents_num, int days, int *exits_residents, FILE *pf_roomlog); int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_params);

/*
This function initialize the promote days therad parameters.
Input:  RESIDENT *p_residents - a pointer to the resident struct that will contains all the information about the residents.
		ROOM *p_rooms - a pointer to the room struct that will contains all the information about the rooms.
		promote_days_thread_params *p_promote_days_thread_params - a pointer to the promote_days_therad parameters.
		int residents_num - integer that contains the number of residnts.
		int *days - a pointer to the days integer that contains the day 'today' .
		int *exits_residents - a pointer to the exit residents integer that contains the number of residents that left the hotel.
Output: void
*/
void initialization_p_promote_days_thread_params(RESIDENT *p_residents, ROOM *p_rooms, promote_days_thread_params *p_promote_days_thread_params, int residents_num, int *days, int *exits_residents);

/*
This function opens room log file.
Input:  char *main_folder_path - a string contains the path to the folder with names.txt file.
		FILE **pf_roomlog -  a pointer to the pointer to roomlog file.
Output: int, success or error code
*/
int open_roomLog_file(char *main_folder_path, FILE **pf_roomlog);

/*
This function create thread for each resident.
Input:  HANDLE *p_resident_thread_handles - list of handles for each resident thread.
		DWORD *p_thread_ids - list for thread ids for each resident. 
		resident_thread_params *p_resident_thread_params - a pointer to the resident_therad parameters.
Output: int, success or error code
*/
int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_resident_thread_params);

/*
This function create the promote days thread.
Input:  HANDLE **p_main_thread_handle - a handle for promote days thread.
		DWORD *p_main_thread_id - the promote thread id.
		promote_days_thread_params *p_promote_days_thread_params - a pointer to the promote_days_therad parameters.
Output: int, success or error code
*/
int create_promote_days_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, promote_days_thread_params *p_main_thread_params);

/*
This function close the promote days thread.
Input:  HANDLE **p_main_thread_handle - a handle for promote days thread.
		DWORD *p_main_thread_id - the promote thread id.
		promote_days_thread_params *p_promote_days_thread_params - a pointer to the promote_days_therad parameters.
Output: int, success or error code
*/
int terminate_promote_days_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, promote_days_thread_params *p_main_thread_params);

/*
This function creats a thread.
Input:  LPTHREAD_START_ROUTINE p_start_routine - function to be called in the new thread created
		LPVOID p_thread_parameters - pointer to parameters for the new thread, only one is permitted so we send struct
		LPDWORD p_thread_id - pointer for thread id to be written
Output: void, creates thread
*/
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,LPVOID p_thread_parameters,LPDWORD p_thread_id);

/*
This function is called when creating the promote days thread, promote the day and check to see
if all the resident left the hotel.
Input:  LPVOID lpParam - parameters for the thread. later we cast them as 'promote_days_thread_params' struct we defined
Output: DWORD, success or error code
*/
DWORD WINAPI Promote_days(LPVOID lpParam);

/*
This function called by 'DWORD WINAPI Promote_day' when creating the promote days thread.
Input:  promote_days_thread_params *p_promote_days_thread_params - a pointer to the promote_days_therad parameters.
Output: DWORD, success or error code
*/
int promote_days_thread_function(promote_days_thread_params *p_main_thread_params); 

/*
This function is called when creating the a resident thread, check if the room that the resident want is free 
if so he get in to the room and if not he waits, all tha activity of the residnt will be write in the roomlog file.
Input:  LPVOID lpParam - parameters for the thread. later we cast them as 'resident_thread_params' struct we defined
Output: DWORD, success or error code
*/
DWORD WINAPI resident_enter_thread(LPVOID lpParam);

/*
This function called by 'DWORD WINAPI resident_enter_thread' when creating the resident thread.
Input:  resident_thread_params *p_resident_thread_params - a pointer to the resident_therad parameters.
Output: DWORD, success or error code
*/
int resident_thread_function(resident_thread_params *p_params);

/*
This function checks if enough arguments were given.
Input:  int argc - number of arguments
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code of given enough arguments, error code else
*/
int check_arguments(int argc, int *return_code);

/*
This function creates mutex by name, and checks if it opened correctly.
Input:  HANDLE *mutex_handle - pointer to handle who will hold the mutex
		LPSECURITY_ATTRIBUTES lpMutexAttributes - access mode for the mutex
		BOOL bInitialOwner - initial state of the mutex, locked or released
		LPCTSTR lpName - mutex unique name
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex created successfully, error code else
*/
int create_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code);

/*
This function opens mutex by name, and checks if it opened correctly.
Input:  HANDLE *mutex_handle - pointer to handle who will hold the mutex
		LPSECURITY_ATTRIBUTES lpMutexAttributes - access mode for the mutex
		BOOL bInitialOwner - initial state of the mutex, locked or released
		LPCTSTR lpName - mutex unique name
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex created successfully, error code else
*/
int open_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
);

/*
This function locks a given mutex.
Input:  HANDLE *mutex_handle - pointer to handle of the mutex to be locked
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex locked successfully, error code else
*/
int lock_mutex(HANDLE *mutex_handle, int *return_code);

/*
This function releases a given mutex.
Input:  HANDLE *mutex_handle - pointer to handle of the mutex to be released
		int *return_code - pointer to the return code, to be updated in case of an error
Output: int, success code if mutex released successfully, error code else
*/
int release_mutex(HANDLE *mutex_handle, int *return_code);

/*
This function closes given handle, in case of an error prints it.
Input:  HANDLE *handle - pointer to the handle to be closed
Output: void
*/
void close_handle(HANDLE *handle);

/*
This function closes all the semaphores opend in 'initialization_rooms', in case of an error prints it.
Input:  ROOM *p_rooms - pointer to the rooms array
		int *rooms_num - pointer to the number of rooms in the hotel
Output: void
*/
void close_semaphors(ROOM *p_rooms, int *rooms_num);

