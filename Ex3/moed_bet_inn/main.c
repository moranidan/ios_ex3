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
		return_code = ERR_CODE_DEFAULT;
		return ERR_CODE_DEFAULT;
	}
	int *rooms_num = 0;
	int *residents_num = 0;
	//HANDLE p_rooms_handles[MAX_ROOM_NUM];	// array for rooms handles
	//DWORD p_thread_ids[MAX_ROOM_NUM];		// array for rooms id's
	ROOM *p_rooms;	// array for room struct
	p_rooms = (ROOM *)calloc(MAX_ROOM_NUM, sizeof(ROOM)); // allocate memory for room parameters
	if (NULL == p_rooms) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	HANDLE p_resident_thread_handles[MAX_RESIDENT_NUM];	// array for residents handles
	DWORD p_thread_ids[MAX_RESIDENT_NUM];		// array for therad id's
	RESIDENT *p_residents;	// array for resident struct
	p_residents = (RESIDENT *)calloc(MAX_RESIDENT_NUM, sizeof(RESIDENT)); // allocate memory for resident parameters
	if (NULL == p_residents) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	return_code = initialization_rooms(main_folder_path,p_rooms,&rooms_num);
	return_code = initialization_names(main_folder_path,p_residents,p_rooms,rooms_num,&residents_num);
	p_resident_therad_params  *p_params;	// array for residents therad parameters struct
	p_params = (p_resident_therad_params *)calloc(residents_num, sizeof(p_resident_therad_params)); // allocate memory for resident therad parameters
	if (NULL == p_params) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return_code = ERR_CODE_ALLOCCING_MEMORY;
		goto SKIP;
	}
	initialization_p_params(main_folder_path, p_residents, p_rooms,p_params,residents_num);
	return_code = create_main_therad();
	return_code = create_resident_threads(p_resident_thread_handles, p_thread_ids,residents_num,p_params);
	
SKIP:
	//printf(days);
	return return_code;
}
