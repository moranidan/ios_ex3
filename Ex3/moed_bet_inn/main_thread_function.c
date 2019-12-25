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
	while (TRUE)
	{
		//for (int i; i < (p_main_thread_params->residents_num); i++) {

		//}

		Sleep(1000);
		HANDLE days_mutex_handle = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_DAYS_NAME);
		HANDLE exit_residents_mutex_handle = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_EXIT_RESIDENTS);
		DWORD wait_code;
		wait_code = WaitForSingleObject(days_mutex_handle, INFINITE);
		if (wait_code != WAIT_OBJECT_0) {
			printf("Error when open mutex\n");
			return ERR_CODE_MUTEX;
		}
		*(p_main_thread_params->p_days) = *(p_main_thread_params->p_days) + 1;
		BOOL ret_val;
		ret_val = ReleaseMutex(days_mutex_handle);
		if (ret_val == FALSE) {
			printf("Error when releasing\n");
			return ERR_CODE_MUTEX;
		}

		//lock mutex on exit residents
		wait_code = WaitForSingleObject(exit_residents_mutex_handle, INFINITE);
		if (wait_code != WAIT_OBJECT_0) {
			printf("Error when open mutex\n");
			return ERR_CODE_MUTEX;
		}
		if (*(p_main_thread_params->p_exits_residents) == p_main_thread_params->residents_num) {
			//free mutex on exit residents
			ret_val = ReleaseMutex(exit_residents_mutex_handle);
			if (ret_val == FALSE) {
				printf("Error when releasing\n");
				return ERR_CODE_MUTEX;
			}
			break;
		}
		//free mutex on exit residents
		ret_val = ReleaseMutex(exit_residents_mutex_handle);
		if (ret_val == FALSE) {
			printf("Error when releasing\n");
			return ERR_CODE_MUTEX;
		}
	}
}