// main.c

/*
Authors –
	Moran Idan - 315239079
	Ofer Bear - 207943366
Project - moed_bet_inn
Description - This project TODO
*/

// Includes --------------------------------------------------------------------

#include "HardCodedData.h"
#include "moed_bet_inn.h"
#include <stdio.h>
#include <windows.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.
//#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */

// Function Definitions --------------------------------------------------------

int main(int argc, char *argv[]) {
	int return_code = ERR_CODE_DEFAULT;
	char *main_folder_path;
	main_folder_path = (char *)malloc((strlen(argv[1])+1) * sizeof(char));
	strcpy_s(main_folder_path, (strlen(argv[1])+1), argv[1]);
	if (argc < 2) {    //check if there are enough arguments
		printf("No folder name entered");
		return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	if (NULL == main_folder_path) {		// check for error when allocating memory
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	int days = 0;
	HANDLE days_mutex_handle = CreateMutex(NULL, FALSE, MUTEX_DAYS_NAME);
	if (days_mutex_handle == NULL) {
		printf("error when creating mutex", GetLastError());
		return_code = ERR_CODE_MUTEX;
		goto SKIP;
	}
	HANDLE file_mutex_handle = CreateMutex(NULL, FALSE, MUTEX_ROOMLOG_FILE_NAME);
	if (file_mutex_handle == NULL) {
		printf("error when creating mutex", GetLastError());
		return_code = ERR_CODE_MUTEX;
		goto SKIP;
	}
	int rooms_num = 0;
	int residents_num = 0;
	int exits_residents = 0;
	ROOM *p_rooms;	// array for room struct
	p_rooms = (ROOM *)calloc(MAX_ROOM_NUM, sizeof(ROOM)); // allocate memory for room parameters
	if (NULL == p_rooms) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	HANDLE p_resident_thread_handles[MAX_RESIDENT_NUM];	// array for residents handles
	DWORD p_thread_ids[MAX_RESIDENT_NUM];		// array for thread id's
	RESIDENT *p_residents;	// array for resident struct
	p_residents = (RESIDENT *)calloc(MAX_RESIDENT_NUM, sizeof(RESIDENT)); // allocate memory for resident parameters
	if (NULL == p_residents) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	return_code = initialization_rooms(main_folder_path,p_rooms,&rooms_num);
	return_code = initialization_names(main_folder_path,p_residents,p_rooms,rooms_num,&residents_num);
	resident_thread_params  *p_resident_thread_params;	// array for residents thread parameters struct
	p_resident_thread_params = (resident_thread_params *)calloc(residents_num, sizeof(resident_thread_params)); // allocate memory for resident therad parameters
	if (NULL == p_resident_thread_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	HANDLE p_main_thread_handle = NULL;
	DWORD p_main_thread_id = NULL;
	main_thread_params *p_main_thread_params;
	p_main_thread_params = (main_thread_params *)calloc(1,sizeof(main_thread_params));
	initialization_p_main_thread_params(p_residents, p_rooms, p_main_thread_params, residents_num, &days, &exits_residents);
	return_code = create_main_thread(p_main_thread_handle,p_main_thread_id, p_main_thread_params);
	initialization_p_resident_thread_params(main_folder_path, p_residents, p_rooms, p_resident_thread_params, residents_num,&days,&exits_residents);
	return_code = create_resident_threads(p_resident_thread_handles, p_thread_ids,residents_num,p_resident_thread_params);
	return_code = terminate_main_thread(p_main_thread_handle, p_main_thread_id, p_main_thread_params);
SKIP:
	//printf(days);
	return return_code;
}
