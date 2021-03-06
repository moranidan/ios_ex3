//promote_days_thread_function.c

// Description - This module contain the function called when promote days thread is created.

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.

// Function Definitions --------------------------------------------------------
DWORD WINAPI Promote_days(LPVOID lpParam)
{
	Sleep(10);								// as required
	int return_code = SUCCESS_CODE;
	promote_days_thread_params *p_promote_days_thread_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread");
		return ERR_CODE_THREAD;
	}
	p_promote_days_thread_params = (promote_days_thread_params *)lpParam;
	return_code = promote_days_thread_function(p_promote_days_thread_params);
	return return_code;
}

int promote_days_thread_function(promote_days_thread_params *p_promote_days_thread_params) {
	int return_code = SUCCESS_CODE;

	// open mutexes
	HANDLE days_mutex_handle = NULL;
	if (open_and_check_mutex(&days_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_DAYS_NAME, &return_code) != SUCCESS_CODE) {
		return ERR_CODE_MUTEX;
	}
	HANDLE exit_residents_mutex_handle = NULL;
	if (open_and_check_mutex(&exit_residents_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_EXIT_RESIDENTS, &return_code) != SUCCESS_CODE) {
		close_handle(&days_mutex_handle);
		return ERR_CODE_MUTEX;
	}

	while (TRUE)
	{
		Sleep(70);

		// lock days mutex
		if (lock_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
			break;
		}

		// critical zone
		*(p_promote_days_thread_params->p_days) = *(p_promote_days_thread_params->p_days) + 1;

		// release days mutex
		if (release_mutex(&days_mutex_handle, &return_code) != SUCCESS_CODE) {
			break;
		}

		Sleep(70);

		// lock exit_residents mutex
		if (lock_mutex(&exit_residents_mutex_handle, &return_code) != SUCCESS_CODE) {
			break;
		}

		//critical zone
		if (*(p_promote_days_thread_params->p_exits_residents) == p_promote_days_thread_params->residents_num) {
			// release mutex on exit residents
			release_mutex(&exit_residents_mutex_handle, &return_code);
			break;
		}
		//release mutex on exit residents
		if (release_mutex(&exit_residents_mutex_handle, &return_code) != SUCCESS_CODE) {
			break;
		}
	}
	close_handle(&days_mutex_handle);
	close_handle(&exit_residents_mutex_handle);
	return return_code;
}