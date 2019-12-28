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
	->int room_days - integer for the number of days that the resident can pay for the room.  initialized when opening names.txt file.
	*/
typedef struct
{
	char name[MAX_NAME_INPUT];
	int my_budget;
	int my_room_num; 
	int room_days;  
} RESIDENT;


/* struct for residents therad parameters
	-> ROOM p_rooms - a pointer to the struct that contaits the list of rooms.
	-> RESIDENT p_resident - a pointer to an object (a resident) in the struct that contaits the list of residents.
	-> char main_folder_path - 
	-> int p_days;
	-> int p_exits_residents;
	-> FILE pf_roomlog
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

typedef struct  
{
	int *p_days;
	ROOM *p_rooms;
	RESIDENT *p_residents;
	int residents_num;
	int *p_exits_residents;
} main_thread_params;



// Function Declarations -------------------------------------------------------
int initialization_names(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, int rooms_num, int *residents_num);
int initialization_rooms(char *main_folder_path, ROOM *p_rooms, int *rooms_num);
void initialization_p_resident_thread_params(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, resident_thread_params *p_resident_thread_params, int residents_num, int days, int *exits_residents, FILE *pf_roomlog); int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_params);
void initialization_p_main_thread_params(RESIDENT *p_residents, ROOM *p_rooms, main_thread_params *p_main_thread_params, int residents_num, int *days, int *exits_residents);
int open_roomLog_file(char *main_folder_path, FILE **pf_roomlog);
int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_resident_thread_params);
int create_main_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params);
int terminate_main_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params);
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,LPVOID p_thread_parameters,LPDWORD p_thread_id);
DWORD WINAPI Promote_days(LPVOID lpParam);
int main_thread_function(main_thread_params *p_main_thread_params);  
DWORD WINAPI resident_enter_thread(LPVOID lpParam);
int thread_function(resident_thread_params *p_params);

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

