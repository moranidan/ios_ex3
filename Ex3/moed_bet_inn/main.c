// main.c

/*
Authors –
	Moran Idan - 315239079
	Ofer Bear - 207943366
Project - moed_bet_inn
Description - This project calculats the grade of indiviual student, given the path for his grades folder
*/

// Includes --------------------------------------------------------------------

#include "HardCodedData.h"
#include <stdio.h>
#include <windows.h>

// Function Definitions --------------------------------------------------------

int main(int argc, char *argv[]) {
	int return_code = ERR_CODE_DEFAULT;
	char *main_folder_path;
	main_folder_path = (char *)malloc((strlen(argv[1])+1) * sizeof(char));
	strcpy_s(main_folder_path, (strlen(argv[1])+1), argv[1]);
	if (argc < 2) {    //check if there are enough arguments
		printf("No folder name entered");
		return_code = ERR_CODE_NOT_ENOUGH_ARGUMENTS;
	}
	if (NULL == main_folder_path) {		// check for error when allocating memory
		printf("Error when allocating memory");
		return_code = ERR_CODE_DEFAULT;
		return ERR_CODE_DEFAULT;
	}
	return_code = initialization_names(main_folder_path);
	//return_code = initialization_rooms(main_folder_path);
	return return_code;



}
