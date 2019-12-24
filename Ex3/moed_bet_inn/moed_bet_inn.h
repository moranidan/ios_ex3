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
	int *p_days;
} resident_thread_params;

typedef struct  //TODO
{
	int *p_days;
	ROOM *p_rooms;
	RESIDENT *p_residents;
	int residents_num;
	//need to think
} main_thread_params;



// Function Declarations -------------------------------------------------------
int initialization_names(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, int rooms_num, int *residents_num);
int initialization_rooms(char *main_folder_path, ROOM *p_rooms, int *rooms_num);
void initialization_p_resident_thread_params(char *main_folder_path, RESIDENT *p_residents, ROOM *p_rooms, resident_thread_params *p_params, int residents_num);
void initialization_p_main_thread_params(RESIDENT *p_residents, ROOM *p_rooms, main_thread_params p_main_thread_params, int residents_num);
int create_resident_threads(HANDLE *p_resident_thread_handles, DWORD *p_thread_ids, int *residents_num, resident_thread_params *p_params);
int create_main_thread(HANDLE *p_main_thread_handle, DWORD *p_main_thread_id, main_thread_params *p_main_thread_params);
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,LPVOID p_thread_parameters,LPDWORD p_thread_id);
DWORD WINAPI Promote_days(LPVOID lpParam);
int main_thread_function(main_thread_params *p_main_thread_params);  //TODO
DWORD WINAPI resident_enter_thread(LPVOID lpParam);
int thread_function(resident_thread_params *p_params);   //TODO
