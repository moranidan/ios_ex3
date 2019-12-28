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
	int path_len = strlen(main_folder_path) + LEN_FILE_NAME_RESIDENTS_NAMES;
	char line[MAX_LINE_LENGTH] = "\0";
	file_path = (char *)malloc(path_len * sizeof(char));
	if (file_path == NULL) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return ERR_CODE_ALLOCCING_MEMORY;
	}
	strcpy_s(file_path, path_len, main_folder_path);
	strcat_s(file_path,path_len, FILE_NAME_RESIDENTS_NAMES);
	fopen_s(&pfl_names, file_path, "r");
	if (pfl_names == NULL) {
		printf("cannot open names file");
		free(file_path);
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
			if (p_residents[i].my_budget % p_rooms[j].price_per_night == 0) {
				p_residents[i].my_room_num = j;
				p_residents[i].room_days = p_residents[i].my_budget / p_rooms[j].price_per_night;
			}
		}
		i += 1;
	}
	*residents_num = i;
	fclose(pfl_names);
	free(file_path);
	return ERR_CODE_DEFAULT;
}

int initialization_rooms(char *main_folder_path, ROOM *p_rooms,int *rooms_num) {
	FILE *pfl_rooms = NULL;
	char *file_path;
	int MAX_PATH_LEN = strlen(main_folder_path) + LEN_FILE_NAME_RESIDENTS_NAMES;
	char line[MAX_LINE_LENGTH] = "\0";
	file_path = (char *)malloc(MAX_PATH_LEN * sizeof(char));
	if (file_path == NULL) {			// cheack if memery allocation was successful
		printf("Error when allocating memory");
		return ERR_CODE_ALLOCCING_MEMORY;
	}
	strcpy_s(file_path, MAX_PATH_LEN, main_folder_path);
	strcat_s(file_path, MAX_PATH_LEN, FILE_NAME_ROOMS_NAMES);
	fopen_s(&pfl_rooms, file_path, "r");
	if (pfl_rooms == NULL) {
		printf("cannot open rooms file\n");
		free(file_path);
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
			p_rooms[i].nums_of_same_room,		/* Initial Count - no slots are full */
			p_rooms[i].nums_of_same_room,		/* Maximum Count */
			NULL); /* un-named */

		if (room_full == NULL) {
			printf("cannot open rooms semaphore\n");
			return ERR_CODE_SEMAPHORE;
		}
		p_rooms[i].room_full = room_full;
		i += 1;
	}
	*rooms_num = i;
	fclose(pfl_rooms);
	free(file_path);
	return ERR_CODE_DEFAULT;
}

void initialization_p_resident_thread_params(char *main_folder_path,RESIDENT *p_residents,ROOM *p_rooms,resident_thread_params *p_resident_thread_params,int residents_num, int days, int *exits_residents, FILE *pf_roomlog) {
	for (int i = 0; i < residents_num; i++) {
		p_resident_thread_params[i].main_folder_path = main_folder_path;
		p_resident_thread_params[i].p_resident = p_residents[i];
		p_resident_thread_params[i].p_rooms = p_rooms;
		p_resident_thread_params[i].p_days = days;
		p_resident_thread_params[i].p_exits_residents = exits_residents;
		p_resident_thread_params[i].pf_roomlog = pf_roomlog;
	}
}

void initialization_p_main_thread_params(RESIDENT *p_residents, ROOM *p_rooms, main_thread_params *p_main_thread_params,int residents_num, int *days, int *exits_residents) {
	p_main_thread_params->p_days = days;
	p_main_thread_params->p_residents = p_residents;
	p_main_thread_params->p_rooms = p_rooms;
	p_main_thread_params->residents_num = residents_num;
	p_main_thread_params->p_exits_residents = exits_residents;
}

int open_roomLog_file(char *main_folder_path, FILE **pf_roomlog) {
	char *file_path;
	int file_path_len = strlen(main_folder_path) + BOOKLOG_FILE_ADDRESS_LEN;
	file_path = (char *)malloc(file_path_len * sizeof(char));
	if (NULL == file_path) {		// check for error when allocating memory
		printf("Error when allocating memory");
		return ERR_CODE_ALLOCCING_MEMORY;
	}
	strcpy_s(file_path, file_path_len, main_folder_path);
	strcat_s(file_path, file_path_len, FILE_NAME_BOOK_LOG);
	fopen_s(pf_roomlog, file_path, "a");
	if (*pf_roomlog == NULL) {
		printf("cannot open BookLog file");
		free(file_path);
		return ERR_CODE_OPEN_FILE;
	}
	free(file_path);
	return ERR_CODE_DEFAULT;
}

//create threads functions -----------------------------------------------------------------------------------------

int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_resident_thread_params){
	int return_code = ERR_CODE_DEFAULT;		// if everything works properly err code default will be returned
	BOOL ret_val;

	for (int i = 0; i < *residents_num; i++)			// create thread for every resident
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

	wait_code = WaitForMultipleObjects(*residents_num, p_resident_thread_handles, TRUE, INFINITE);	// wait for threads to run
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting: %d\n", wait_code);
		return_code = ERR_CODE_THREAD;
	}
	
	LPDWORD thread_exit_code = 0;
	for (int j = 0; j < *residents_num; j++)	// loop for  closing threads handles
	{
		if (GetExitCodeThread(p_resident_thread_handles[j], &thread_exit_code) == 0) {
			printf("Error when trying to get threads exit code");
			return_code = ERR_CODE_THREAD;
		}
		if (thread_exit_code != ERR_CODE_DEFAULT) {
			return_code = ERR_CODE_THREAD;
		}
		ret_val = CloseHandle(p_resident_thread_handles[j]);
		if (FALSE == ret_val)
		{
			printf("Error when closing thread: %d\n", GetLastError());
			return_code = ERR_CODE_CLOSE_THREAD;
		}
	}
	return return_code;
}

int create_main_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params) {
	int return_code = ERR_CODE_DEFAULT;		// if everything works properly err code default will be returned
	BOOL ret_val;
	*p_main_thread_handle = CreateThreadSimple(Promote_days, p_main_thread_params, &p_main_thread_id);	// create thread
	if (NULL == *p_main_thread_handle)	// check for errors
		{
		printf("Error when creating main thread: %d\n", GetLastError());
		return_code = ERR_CODE_CREAT_THEARD;
		return return_code;		// return immediately before creating more threads
	}
	return return_code;
}

int terminate_main_thread(HANDLE **p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params) {
	int return_code = ERR_CODE_DEFAULT;
	DWORD wait_code;
	BOOL ret_val;
	wait_code = WaitForSingleObject(*p_main_thread_handle, INFINITE);	// wait for threads to run
	if (WAIT_OBJECT_0 != wait_code)			// check for errors
	{
		printf("Error when waiting\n");
		return_code = ERR_CODE_THREAD;
	}

	ret_val = CloseHandle(*p_main_thread_handle);
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
//-----------------------------------------------
int check_arguments(int argc, int *return_code) {
	if (argc < 2) {    //check if there are enough arguments
		printf("No folder name entered");
		*return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	return *return_code;
}

int create_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
) {
	*mutex_handle = CreateMutex(lpMutexAttributes, bInitialOwner, lpName);
	if (*mutex_handle == NULL) {
		printf("Error when creating mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
	}
	return *return_code;
}

int open_and_check_mutex(
	HANDLE *mutex_handle,
	LPSECURITY_ATTRIBUTES lpMutexAttributes,
	BOOL bInitialOwner,
	LPCTSTR lpName,
	int *return_code
) {
	*mutex_handle = OpenMutex(lpMutexAttributes, bInitialOwner, lpName);
	if (*mutex_handle == NULL) {
		printf("Error when opening mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
	}
	return *return_code;
}

int lock_mutex(HANDLE *mutex_handle, int *return_code) {
	DWORD wait_code;
	wait_code = WaitForSingleObject(*mutex_handle, INFINITE);
	if (wait_code != WAIT_OBJECT_0) {
		printf("Error when locking mutex\n");
		*return_code = ERR_CODE_MUTEX;
		return ERR_CODE_MUTEX;
	}
	return ERR_CODE_DEFAULT;
}

int release_mutex(HANDLE *mutex_handle, int *return_code) {
	BOOL ret_val;
	ret_val = ReleaseMutex(*mutex_handle);
	if (ret_val == FALSE) {
		printf("Error when releasing mutex: %d\n", GetLastError());
		*return_code = ERR_CODE_MUTEX;
		return ERR_CODE_MUTEX;
	}
	return ERR_CODE_DEFAULT;
}

void close_handle(HANDLE handle) {
	BOOL ret_val;
	ret_val = CloseHandle(handle);
	if (FALSE == ret_val)
	{
		printf("Error when closing handle: %d\n", GetLastError());
	}
}