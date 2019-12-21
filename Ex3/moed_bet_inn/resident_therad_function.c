//resident_therad_function.c

// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>
#pragma warning(disable:4996) // in order to avoid warning about fopen being unsafe function.
//#define _CRT_SECURE_NO_WARNINGS /* to suppress compiler warnings (VS 2010 ) */

// Function Definitions --------------------------------------------------------
DWORD WINAPI resident_enter_therad(LPVOID lpParam)
{
	Sleep(10);								// as required
	int return_code = ERR_CODE_DEFAULT;
	p_resident_therad_params *p_params;			// pointer for the parameters
	if (NULL == lpParam) {					// check if NULL was received instead of parameter
		printf("Error declaring parameters in Thread");
		return ERR_CODE_THERAD;
	}
	p_params = (p_resident_therad_params *)lpParam;
	return_code = therad_function(p_params);
	return return_code;
}

int therad_function(p_resident_therad_params *p_params) {
	FILE *pf_booklog;
	ROOM *p_rooms = p_params->p_rooms;
	RESIDENT *p_resident = &(p_params->p_resident);
	DWORD wait_res;
	DWORD release_res;
	char *file_path;
	file_path = (char *)malloc((strlen(p_params->main_folder_path) + BOOKLOG_FILE_ADDRESS_LEN) * sizeof(char));
	strcpy_s(file_path, sizeof(file_path), p_params->main_folder_path);
	strcat_s(file_path, sizeof(file_path), FILE_NAME_BOOK_LOG);
	wait_res = WaitForSingleObject(p_rooms[p_resident->my_room_num].room_full, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();  //what to do
	fopen_s(&pf_booklog, file_path, "a");
	if (pf_booklog = NULL) {
		printf("cannot open BookLog file");
		return ERR_CODE_OPEN_FILE;
	}
	//mutex on days
	char day_in = itoa(days);
	int int_day_out = days + p_resident->room_days;
	//free mutex
	char day_out = itoa(int_day_out);
	char write_line_to_booklog[MAX_LINE_LENGTH];
	creat_book_log_line(p_params, &write_line_to_booklog,ENTER_ROOM, day_in);
	//mutex on pf_booklog
	fputs(write_line_to_booklog, &pf_booklog);
	//mutex on pf_booklog
	while (TRUE) {
		if (days == day_out) {
			creat_book_log_line(p_params, &write_line_to_booklog, EXIT_ROOM ,day_out);
			//mutex on pf_booklog
			fputs(write_line_to_booklog, &pf_booklog);
			//mutex on pf_booklog
			release_res = ReleaseSemaphore(
				p_rooms[p_resident->my_room_num].room_full,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) ReportErrorAndEndProgram();// what to do
			break;
		}
	}
	fclose(pf_booklog);
	// אם יש עוד הנדלים שצריך לסגור פה? את ההנדל של החוט אולי?
}

void creat_book_log_line(p_resident_therad_params *p_params, char *write_enter ,char *status, char *day) {
	ROOM *p_rooms = p_params->p_rooms;
	RESIDENT *p_resident = &(p_params->p_resident);
	strcpy_s(write_enter, MAX_LINE_LENGTH, p_rooms[p_resident->my_room_num].name);
	strcat_s(write_enter, MAX_LINE_LENGTH, " ");
	strcat_s(write_enter, MAX_LINE_LENGTH, p_resident->name);
	strcat_s(write_enter, MAX_LINE_LENGTH, status);
	strcat_s(write_enter, MAX_LINE_LENGTH, day);
	strcat_s(write_enter, MAX_LINE_LENGTH, "\n");
}

