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
	if (check_arguments(argc, &return_code) != ERR_CODE_DEFAULT){
		return return_code;
	}

	// veriables which must allocate memory
	ROOM *p_rooms = NULL;	// array for room struct
	RESIDENT *p_residents = NULL;	// array for resident struct
	char *main_folder_path = NULL;		// main foldar path string
	resident_thread_params *p_resident_thread_params = NULL;	// array for residents thread parameters struct
	main_thread_params *p_main_thread_params = NULL;	// parameters struct for main_thread

	// copy main folders path
	main_folder_path = (char *)malloc((strlen(argv[1])+1) * sizeof(char));
	if (NULL == main_folder_path) {		// check for error when allocating memory
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		return return_code;
	}
	strcpy_s(main_folder_path, (strlen(argv[1])+1), argv[1]);
	
	// open roomLog.txt file for appending
	FILE *pf_roomlog = NULL;
	return_code = open_roomLog_file(main_folder_path, &pf_roomlog);
	if (return_code != ERR_CODE_DEFAULT) {
		free(main_folder_path);
		return return_code;
	}

	int days = 1;

	// create days mutex
	HANDLE days_mutex_handle = NULL;
	if (create_and_check_mutex(&days_mutex_handle, NULL, FALSE, MUTEX_DAYS_NAME, &return_code) != ERR_CODE_DEFAULT) {
		goto SKIP;
	}

	// create roomLog.txt file mutex
	HANDLE file_mutex_handle = NULL;
	if (create_and_check_mutex(&file_mutex_handle, NULL, FALSE, MUTEX_ROOMLOG_FILE_NAME, &return_code) != ERR_CODE_DEFAULT) {
		goto SKIP;
	}

	// create exit_residents mutex
	HANDLE exit_residents_mutex_handle = NULL;
	if (create_and_check_mutex(&exit_residents_mutex_handle, NULL, FALSE, MUTEX_EXIT_RESIDENTS, &return_code) != ERR_CODE_DEFAULT) {
		goto SKIP;
	}

	// initialize rooms struct
	int rooms_num = 0;
	int residents_num = 0;
	int exits_residents = 0;
	p_rooms = (ROOM *)calloc(MAX_ROOM_NUM, sizeof(ROOM)); // allocate memory for room parameters
	if (NULL == p_rooms) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	// no error handeling!!! malchitz
	return_code = initialization_rooms(main_folder_path, p_rooms, &rooms_num); //TODO

	// initialize names struct
	p_residents = (RESIDENT *)calloc(MAX_RESIDENT_NUM, sizeof(RESIDENT)); // allocate memory for resident parameters
	if (NULL == p_residents) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	// no error handeling!!! malchitz
	return_code = initialization_names(main_folder_path, p_residents, p_rooms, rooms_num, &residents_num); //TODO

	// create "main_thread", which updates 'days' variable
	HANDLE p_main_thread_handle = NULL;
	DWORD p_main_thread_id = NULL;
	p_main_thread_params = (main_thread_params *)calloc(1, sizeof(main_thread_params));
	if (NULL == p_main_thread_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	initialization_p_main_thread_params(p_residents, p_rooms, p_main_thread_params, residents_num, &days, &exits_residents);
	return_code = create_main_thread(&p_main_thread_handle,p_main_thread_id, p_main_thread_params);
	
	// create "resident_thread"s, which represent residents at the hotel
	HANDLE p_resident_thread_handles[MAX_RESIDENT_NUM];	// array for residents handles
	DWORD p_thread_ids[MAX_RESIDENT_NUM];		// array for thread id's
	p_resident_thread_params = (resident_thread_params *)calloc(residents_num, sizeof(resident_thread_params)); // allocate memory for resident therad parameters
	if (NULL == p_resident_thread_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	initialization_p_resident_thread_params(
		main_folder_path,
		p_residents, p_rooms,
		p_resident_thread_params,
		residents_num,
		&days,
		&exits_residents,
		pf_roomlog
	);
	return_code = create_resident_threads(p_resident_thread_handles, p_thread_ids,&residents_num,p_resident_thread_params);

	// terminate main thread and exit
	return_code = terminate_main_thread(&p_main_thread_handle, p_main_thread_id, p_main_thread_params);
	printf("Total days: %d\n", days);

	// close mutex handles
	close_handle(days_mutex_handle);
	close_handle(file_mutex_handle);
	close_handle(exit_residents_mutex_handle);
	// close_semaphors(*p_rooms, &rooms_num); // TODO

SKIP:
	free(main_folder_path);
	free(p_rooms);
	free(p_residents);
	free(p_resident_thread_params);
	free(p_main_thread_params);
	fclose(pf_roomlog);
	return return_code;
}
