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
	FILE *pf_booklog;
	ROOM *p_rooms = p_resident_thread_params->p_rooms;
	RESIDENT *p_resident = &(p_resident_thread_params->p_resident);
	DWORD wait_res;
	DWORD release_res;
	char *file_path;
	file_path = (char *)malloc((strlen(p_resident_thread_params->main_folder_path) + BOOKLOG_FILE_ADDRESS_LEN) * sizeof(char));
	strcpy_s(file_path, sizeof(file_path), p_resident_thread_params->main_folder_path);
	strcat_s(file_path, sizeof(file_path), FILE_NAME_BOOK_LOG);
	wait_res = WaitForSingleObject(p_rooms[p_resident->my_room_num].room_full, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();  //what to do
	fopen_s(&pf_booklog, file_path, "a");
	if (pf_booklog = NULL) {
		printf("cannot open BookLog file");
		return ERR_CODE_OPEN_FILE;
	}
	//mutex on days
	int *day_in = p_resident_thread_params->p_days;
	//free mutex
	//mutex on pf_booklog
	fprintf(pf_booklog,"%s %s IN %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, day_in);
	//mutex on pf_booklog
	int day_out = *day_in + p_resident->room_days;
	while (TRUE) {
		//mutex on days
		if (p_resident_thread_params->p_days == day_out) {
			//mutex on days
			//mutex on pf_booklog
			fprintf(pf_booklog, "%s %s OUT %d\n", p_rooms[p_resident->my_room_num].name, p_resident->name, day_out);
			//mutex on pf_booklog
			release_res = ReleaseSemaphore(
				p_rooms[p_resident->my_room_num].room_full,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) ReportErrorAndEndProgram();// what to do
			break;
		}
		else {
			//free mutex on days
		}
	}
	fclose(pf_booklog);
	// אם יש עוד הנדלים שצריך לסגור פה? את ההנדל של החוט אולי?
}
