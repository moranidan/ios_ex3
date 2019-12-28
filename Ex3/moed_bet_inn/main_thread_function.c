//main_thread_function.c

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.
//#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */

// Function Definitions --------------------------------------------------------
DWORD WINAPI Promote_days(LPVOID lpParam)
{
	Sleep(10);								// as required
	int return_code = ERR_CODE_DEFAULT;
	main_thread_params *p_main_thread_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread");
		return ERR_CODE_THREAD;
	}
	p_main_thread_params = (main_thread_params *)lpParam;
	return_code = main_thread_function(p_main_thread_params);
	return return_code;
}

int main_thread_function(main_thread_params *p_main_thread_params) {
	int return_code = ERR_CODE_DEFAULT;

	// open mutexes
	HANDLE days_mutex_handle = NULL;
	if (open_and_check_mutex(&days_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_DAYS_NAME, &return_code) != ERR_CODE_DEFAULT) {
		return ERR_CODE_MUTEX;
	}
	HANDLE exit_residents_mutex_handle = NULL;
	if (open_and_check_mutex(&exit_residents_mutex_handle, SYNCHRONIZE, FALSE, MUTEX_EXIT_RESIDENTS, &return_code) != ERR_CODE_DEFAULT) {
		close_handle(days_mutex_handle);
		return ERR_CODE_MUTEX;
	}

	while (TRUE)
	{
		Sleep(200);

		// lock days mutex
		if (lock_mutex(&days_mutex_handle, &return_code) != ERR_CODE_DEFAULT) {
			break;
		}

		// critical zone
		*(p_main_thread_params->p_days) = *(p_main_thread_params->p_days) + 1;

		// release days mutex
		if (release_mutex(&days_mutex_handle, &return_code) != ERR_CODE_DEFAULT) {
			break;
		}

		Sleep(200);

		// lock exit_residents mutex
		if (lock_mutex(&exit_residents_mutex_handle, &return_code) != ERR_CODE_DEFAULT) {
			break;
		}

		//critical zone
		if (*(p_main_thread_params->p_exits_residents) == p_main_thread_params->residents_num) {
			// release mutex on exit residents
			release_mutex(&exit_residents_mutex_handle, &return_code);
			break;
		}
		//release mutex on exit residents
		if (release_mutex(&exit_residents_mutex_handle, &return_code) != ERR_CODE_DEFAULT) {
			break;
		}
	}
	close_handle(days_mutex_handle);
	close_handle(exit_residents_mutex_handle);
	return return_code;
}