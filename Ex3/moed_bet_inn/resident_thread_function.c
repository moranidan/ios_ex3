//resident_thread_function.c

// Description - This module contain the function called when new resident thread is created.

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#include <time.h>

#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.

// Function Definitions --------------------------------------------------------
DWORD WINAPI resident_enter_thread(LPVOID lpParam)
{
	Sleep(10);								// as required
	int return_code = SUCCESS_CODE;
	resident_thread_params *p_resident_thread_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread\n");
		return ERR_CODE_THREAD;
	}
	p_resident_thread_params = (resident_thread_params *)lpParam;
	return_code = resident_thread_function(p_resident_thread_params);
	return return_code;
}

int resident_thread_function(resident_thread_params *p_resident_thread_params) {
	char *name = p_resident_thread_params->p_resident.name;
	int return_code = SUCCESS_CODE;
	ROOM *p_rooms = p_resident_thread_params->p_rooms;
	RESIDENT *p_resident = &(p_resident_thread_params->p_resident);

	DWORD wait_res;
	DWORD release_res;

	// open mutex handles
	HANDLE days_mutex_handle = NULL;
	if (open_and_check_mutex(&days_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_DAYS_NAME, &return_code) != SUCCESS_CODE) {
		return return_code;
	}
	HANDLE file_mutex_handle = NULL;
	if (open_and_check_mutex(&file_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_ROOMLOG_FILE_NAME, &return_code) != SUCCESS_CODE) {
		close_handle(&days_mutex_handle);
		return return_code;
	}
	HANDLE exit_residents_mutex_handle = NULL;
	if (open_and_check_mutex(&exit_residents_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_EXIT_RESIDENTS, &return_code) != SUCCESS_CODE) {
		close_handle(&days_mutex_handle);
		close_handle(&file_mutex_handle);
		return return_code;
	}
	
	// wait on semaphore
	wait_res = WaitForSingleObject(p_rooms[p_resident->my_room_num].room_full, INFINITE);
	if (wait_res != WAIT_OBJECT_0) {
		printf("Error when waiting on semaphore\n");
		return_code =  ERR_CODE_SEMAPHORE;
		goto ERROR_OCCURRED;
	}
	//lock mutex on days
	if (lock_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
		goto ERROR_OCCURRED;
	}

	// critical zone
	int *day_in = p_resident_thread_params->p_days;
	
	//release mutex on days
	if (release_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
		goto ERROR_OCCURRED;
	}

	//lock mutex on pf_booklog
	if (lock_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
		goto ERROR_OCCURRED;
	}

	// critical zone
	fprintf(p_resident_thread_params->pf_roomlog,"%s %s IN %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, *day_in);
	
	//release mutex on pf_booklog
	if (release_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
		goto ERROR_OCCURRED;
	}

	int day_out = *day_in + p_resident->room_days;
	while (TRUE) {
		//lock mutex on days
		if (lock_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
			break;
		}
		if (*(p_resident_thread_params->p_days) == day_out) {
			//release mutex on days
			if (release_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}
			//lock mutex on pf_booklog
			if (lock_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}

			// critical zone
			fprintf(p_resident_thread_params->pf_roomlog, "%s %s OUT %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, day_out);
			
			//release mutex on pf_booklog
			if (release_mutex(&file_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}
			//lock mutex on exit residents
			if (lock_mutex(&exit_residents_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}
			// critical zone
			int already_entered = *(p_resident_thread_params->p_exits_residents) + 1; //update the exit_residents number
			*(p_resident_thread_params->p_exits_residents) = already_entered;
			//release mutex on exit residents
			if (release_mutex(&exit_residents_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}
			// release semaphore
			release_res = ReleaseSemaphore(
				p_rooms[p_resident->my_room_num].room_full,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) {
				printf("Error when releasing\n");
				return_code = ERR_CODE_SEMAPHORE;
				break;
			}
			break;
		}
		else {
			//release mutex on days
			if (release_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
				break;
			}
		}
	}
ERROR_OCCURRED:
	close_handle(&days_mutex_handle);
	close_handle(&file_mutex_handle);
	close_handle(&exit_residents_mutex_handle);
	return return_code;
}
