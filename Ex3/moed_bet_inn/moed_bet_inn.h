// moed_bet_inn.h

// This header consists the structs and function declerations for all the functions used in the project, and their documentation.

#pragma once

#include "HardCodedData.h"
#include <windows.h>
#include <math.h>
#include <stdio.h>

// Types -----------------------------------------------------------------------

typedef struct
{
	char name[MAX_NAME_INPUT];
	int price_per_night;
	int nums_of_same_room;
	HANDLE room_full;
} ROOM;


typedef struct
{
	char name[MAX_NAME_INPUT];
	int my_budget;
	int my_room_num;  //the location for the room that the resident can go to in the p_rooms array
	int room_days;   //number of days that the resident can pay for the room
} RESIDENT;


typedef struct
{
	ROOM *p_rooms;
	RESIDENT p_resident;
	char *main_folder_path;
} p_resident_therad_params;

typedef struct  //TODO
{
	int max; //delete after thinking
	//need to think
} p_main_therad_params;



// Function Declarations -------------------------------------------------------
int initialization_names(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, int rooms_num, int *residents_num);
int initialization_rooms(char *main_folder_path, ROOM *p_rooms, int *rooms_num);
void initialization_p_params(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, p_resident_therad_params *p_params, int residents_num);
int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, p_resident_therad_params *p_params);
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,LPVOID p_thread_parameters,LPDWORD p_thread_id);
DWORD WINAPI Promote_days(LPVOID lpParam);
int main_therad_function();  //TODO
DWORD WINAPI resident_enter_therad(LPVOID lpParam);
int therad_function(p_resident_therad_params *p_params);   //TODO
void creat_book_log_line(p_resident_therad_params *p_params, char *write_enter, char *status, char *day);
