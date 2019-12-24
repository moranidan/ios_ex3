//moed_bet_inn.c

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.
//#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */

// Function Definitions --------------------------------------------------------
int initialization_names(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, int rooms_num, int *residents_num) {
	FILE *pfl_names = NULL;
	char *file_path;
	int MAX_PATH_LEN = strlen(main_folder_path) + LEN_FILE_NAME_RESIDENTS_NAMES;
	char line[MAX_LINE_LENGTH] = "\0";
	file_path = (char *)malloc(MAX_PATH_LEN * sizeof(char));
	//char name[MAX_NAME_INPUT] = "\0";
	strcpy_s(file_path, MAX_PATH_LEN, main_folder_path);
	strcat_s(file_path,MAX_PATH_LEN, FILE_NAME_RESIDENTS_NAMES);
	fopen_s(&pfl_names, file_path, "r");
	if (pfl_names == NULL) {
		printf("cannot open names file");
		return ERR_CODE_OPEN_FILE;
	}
	int i = 0;
	while (fgets(line, MAX_LINE_LENGTH, pfl_names)!=NULL) {
		char delim[] = " ";		// split the path by this char
		char *resident_name = strtok(line, delim);
		strcpy_s(p_residents[i].name, MAX_NAME_INPUT, resident_name);
		char *str_my_budget = strtok(NULL, delim);
		p_residents[i].my_budget = atoi(str_my_budget);
		for (int j = 0; j < rooms_num; j++) {
			if (p_residents->my_budget % p_rooms[j].price_per_night == 0) {
				p_residents->my_room_num = j;
				p_residents->room_days = p_residents->my_budget / p_rooms[j].price_per_night;
			}
		}
		i += 1;
	}
	*residents_num = i - 1;
	return ERR_CODE_DEFAULT;
}

int initialization_rooms(char *main_folder_path, ROOM *p_rooms,int *rooms_num) {
	FILE *pfl_rooms = NULL;
	char *file_path;
	int MAX_PATH_LEN = strlen(main_folder_path) + LEN_FILE_NAME_RESIDENTS_NAMES;
	char line[MAX_LINE_LENGTH] = "\0";
	file_path = (char *)malloc(MAX_PATH_LEN * sizeof(char));
	strcpy_s(file_path, MAX_PATH_LEN, main_folder_path);
	strcat_s(file_path, MAX_PATH_LEN, FILE_NAME_ROOMS_NAMES);
	fopen_s(&pfl_rooms, file_path, "r");
	if (pfl_rooms == NULL) {
		printf("cannot open rooms file");
		return ERR_CODE_OPEN_FILE;
	}
	int i = 0;
	while (fgets(line, MAX_LINE_LENGTH, pfl_rooms) != NULL) {
		char delim[] = " ";		// split the path by this char
		char *room_name = strtok(line, delim);
		strcpy_s(p_rooms[i].name, MAX_NAME_INPUT, room_name);
		char *str_price_per_night = strtok(NULL, delim);
		p_rooms[i].price_per_night = atoi(str_price_per_night);
		char *str_nums_of_same_room = strtok(NULL, delim);
		p_rooms[i].nums_of_same_room = atoi(str_nums_of_same_room);
		static HANDLE room_full;
		room_full = CreateSemaphore(
			NULL,	/* Default security attributes */
			0,		/* Initial Count - no slots are full */
			p_rooms[i].nums_of_same_room,		/* Maximum Count */
			NULL); /* un-named */

		if (room_full == NULL) {
			printf("cannot open rooms semaphore");
			return ERR_CODE_OPEN_SEMAPHORE;
		}
		p_rooms[i].room_full = room_full;
		i += 1;
	}
	*rooms_num = i - 1;
	return ERR_CODE_DEFAULT;
}

void initialization_p_resident_thread_params(char *main_folder_path,RESIDENT *p_residents,ROOM *p_rooms,resident_thread_params *p_resident_thread_params,int residents_num, int days) {
	for (int i = 0; i < residents_num; i++) {
		p_resident_thread_params[i].main_folder_path = main_folder_path;
		p_resident_thread_params[i].p_resident = p_residents[i];
		p_resident_thread_params[i].p_rooms = p_rooms;
		p_resident_thread_params[i].p_days = &days;
	}
}

void initialization_p_main_thread_params(RESIDENT *p_residents, ROOM *p_rooms, main_thread_params p_main_thread_params,int residents_num, int *days) {
	p_main_thread_params.p_days = &days;
	p_main_thread_params.p_residents = p_residents;
	p_main_thread_params.p_rooms = p_rooms;
	p_main_thread_params.residents_num = residents_num;
}

//create threads functions -----------------------------------------------------------------------------------------

int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num,resident_thread_params *p_resident_thread_params){
	int return_code = ERR_CODE_DEFAULT;		// if everything works properly err code default will be returned
	BOOL ret_val;

	for (int i = 0; i < residents_num; i++)			// create thread for every resident
	{
		p_resident_thread_handles[i] = CreateThreadSimple(resident_enter_thread, &p_resident_thread_params[i], &p_thread_ids[i]);	// create thread
		if (NULL == p_resident_thread_handles[i])	// check for errors
		{
			printf("Error when creating thread: %d\n", GetLastError());
			return_code = ERR_CODE_CREAT_THEARD;
			for (int j = 0; j < i; j++) {		// close handles that were already opened
				ret_val = CloseHandle(p_resident_thread_handles[j]);
				if (FALSE == ret_val)
				{
					printf("Error when closing thread: %d\n", GetLastError());
				}
			}
			return return_code;		// return immediately before creating more threads
		}
	}

	DWORD wait_code;

	wait_code = WaitForMultipleObjects(residents_num, p_resident_thread_handles, TRUE, THREAD_TIMEOUT_IN_MILLISECONDS);	// wait for threads to run
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting\n");
		return_code = ERR_CODE_THREAD;
	}
	
	for (int j = 0; j < residents_num; j++)	// loop for  closing threads handles
	{
		ret_val = CloseHandle(p_resident_thread_handles[j]);
		if (FALSE == ret_val)
		{
			printf("Error when closing thread: %d\n", GetLastError());
			return_code = ERR_CODE_CLOSE_THREAD;
		}
	}
	return return_code;
}

int create_main_thread(HANDLE *p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params) {
	int return_code = ERR_CODE_DEFAULT;		// if everything works properly err code default will be returned
	BOOL ret_val;
	p_main_thread_handle = CreateThreadSimple(Promote_days,&p_main_thread_params ,&p_main_thread_id);	// create thread
	if (NULL == p_main_thread_handle)	// check for errors
		{
		printf("Error when creating main thread: %d\n", GetLastError());
		return_code = ERR_CODE_CREAT_THEARD;
		return return_code;		// return immediately before creating more threads
	}

	DWORD wait_code;

	wait_code = WaitForSingleObjects(p_main_thread_handle, THREAD_TIMEOUT_IN_MILLISECONDS);	// wait for threads to run
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting\n");
		return_code = ERR_CODE_THREAD;
	}

	ret_val = CloseHandle(p_main_thread_handle);
	if (FALSE == ret_val)
	{
		printf("Error when closing thread: %d\n", GetLastError());
		return_code = ERR_CODE_CLOSE_THREAD;
	}
	return return_code;
}


static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine, LPVOID p_thread_parameters, LPDWORD p_thread_id){

	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		return NULL;		// return NULL, error will be caught in "create_threads"
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		return NULL;		// return NULL, error will be caught in "create_threads"
	}

	thread_handle = CreateThread(
		NULL,                /*  default security attributes */
		0,                   /*  use default stack size */
		p_start_routine,     /*  thread function */
		p_thread_parameters, /*  argument to thread function */
		0,                   /*  use default creation flags */
		p_thread_id);        /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
	}

	return thread_handle;
}

