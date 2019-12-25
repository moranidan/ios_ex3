//resident_thread_function.c

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.
//#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */

// Function Definitions --------------------------------------------------------
DWORD WINAPI resident_enter_thread(LPVOID lpParam)
{
	Sleep(10);								// as required
	int return_code = ERR_CODE_DEFAULT;
	resident_thread_params *p_resident_thread_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread");
		return ERR_CODE_THREAD;
	}
	p_resident_thread_params = (resident_thread_params *)lpParam;
	return_code = thread_function(p_resident_thread_params);
	return return_code;
}

int thread_function(resident_thread_params *p_resident_thread_params) {
	char *name = p_resident_thread_params->p_resident.name;
	int return_code = ERR_CODE_DEFAULT;
	ROOM *p_rooms = p_resident_thread_params->p_rooms;
	RESIDENT *p_resident = &(p_resident_thread_params->p_resident);
	DWORD wait_res;
	DWORD release_res;
	HANDLE days_mutex_handle = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_DAYS_NAME);
	HANDLE file_mutex_handle = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_ROOMLOG_FILE_NAME);
	HANDLE exit_residents_mutex_handle = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_EXIT_RESIDENTS);
	DWORD wait_code;
	BOOL ret_val;
	wait_res = WaitForSingleObject(p_rooms[p_resident->my_room_num].room_full, INFINITE);
	if (wait_res != WAIT_OBJECT_0) {
		printf("Error when wait to semaphore\n");
		return_code =  ERR_CODE_SEMAPHORE;
		goto ERROR_OCCURRED;
	}
	char *name2 = p_resident_thread_params->p_resident.name;
	//lock mutex on days
	wait_code = WaitForSingleObject(days_mutex_handle, INFINITE);
	if (wait_code != WAIT_OBJECT_0) {
		printf("Error when open mutex\n");
		return_code = ERR_CODE_MUTEX;
		goto ERROR_OCCURRED;
	}
	int *day_in = p_resident_thread_params->p_days;
	//free mutex on days
	ret_val = ReleaseMutex(days_mutex_handle);
	if (ret_val == FALSE) {
		printf("Error when releasing\n");
		return_code = ERR_CODE_MUTEX;
		goto ERROR_OCCURRED;
	}


	//lock mutex on pf_booklog
	wait_code = WaitForSingleObject(file_mutex_handle, INFINITE);
	if (wait_code != WAIT_OBJECT_0) {
		printf("Error when open mutex\n");
		return_code = ERR_CODE_MUTEX;
		goto ERROR_OCCURRED;
	}
	fprintf(p_resident_thread_params->pf_roomlog,"%s %s IN %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, *day_in);
	//free mutex on pf_booklog
	ret_val = ReleaseMutex(file_mutex_handle);
	if (ret_val == FALSE) {
		printf("Error when releasing\n");
		return_code = ERR_CODE_MUTEX;
		goto ERROR_OCCURRED;
	}
	int day_out = *day_in + p_resident->room_days;
	while (TRUE) {
		//lock mutex on days
		wait_code = WaitForSingleObject(days_mutex_handle, INFINITE);
		if (wait_code != WAIT_OBJECT_0) {
			printf("Error when open mutex\n");
			return_code = ERR_CODE_MUTEX;
			goto ERROR_OCCURRED;
		}
		if (*(p_resident_thread_params->p_days) == day_out) {

			//free mutex on days
			ret_val = ReleaseMutex(days_mutex_handle);
			if (ret_val == FALSE) {
				printf("Error when releasing\n");
				return_code = ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
			//lock mutex on pf_booklog
			wait_code = WaitForSingleObject(file_mutex_handle, INFINITE);
			if (wait_code != WAIT_OBJECT_0) {
				printf("Error when open mutex\n");
				return_code = ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
			fprintf(p_resident_thread_params->pf_roomlog, "%s %s OUT %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, day_out);
			//free mutex on pf_booklog
			ret_val = ReleaseMutex(file_mutex_handle);
			if (ret_val == FALSE) {
				printf("Error when releasing\n");
				return_code = ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
			//lock mutex on exit residents
			wait_code = WaitForSingleObject(exit_residents_mutex_handle, INFINITE);
			if (wait_code != WAIT_OBJECT_0) {
				printf("Error when open mutex\n");
				return_code = ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
			int already_entered = *(p_resident_thread_params->p_exits_residents) + 1;
			*(p_resident_thread_params->p_exits_residents) = already_entered;
			//free mutex on exit residents
			ret_val = ReleaseMutex(exit_residents_mutex_handle);
			if (ret_val == FALSE) {
				printf("Error when releasing\n");
				return_code = ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
			release_res = ReleaseSemaphore(
				p_rooms[p_resident->my_room_num].room_full,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) {
				printf("Error when releasing\n");
				return_code = ERR_CODE_SEMAPHORE;
				goto ERROR_OCCURRED;
			}
			break;
		}
		else {
			//free mutex on days
			ret_val = ReleaseMutex(days_mutex_handle);
			if (ret_val == FALSE) {
				printf("Error when releasing\n");
				return_code =  ERR_CODE_MUTEX;
				goto ERROR_OCCURRED;
			}
		}
	}
ERROR_OCCURRED:
	return return_code;
}
