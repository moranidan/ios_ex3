//main_therad_function.c

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
	p_main_therad_params *p_main_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread");
		return ERR_CODE_THERAD;
	}
	p_main_params = (p_main_therad_params *)lpParam;
	return_code = main_therad_function(p_main_params);
	return return_code;
}

int main_therad_function() {
	//TODO
}