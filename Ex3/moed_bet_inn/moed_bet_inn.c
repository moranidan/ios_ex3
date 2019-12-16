// Includes --------------------------------------------------------------------

#include "moed_bet_inn.h"
#include "HardCodedData.h"
#include <stdio.h>

// Function Definitions --------------------------------------------------------
int initialization_names(char *main_folder_path) {
	FILE *pfl_names = NULL;
	char *file_path;
	int MAX_PATH_LEN = strlen(main_folder_path) + LEN_FILE_NAME_RESIDENTS_NAMES;
	file_path = (char *)malloc(MAX_PATH_LEN * sizeof(char));
	char line[MAX_INPUT_LINE_LENGTH] = "\0";
	char name[MAX_NAME_INPUT] = "\0";
	strcpy_s(file_path, MAX_PATH_LEN, main_folder_path);
	strcat_s(file_path,MAX_PATH_LEN, FILE_NAME_RESIDENTS_NAMES);
	fopen_s(&pfl_names, file_path, "r");
	if (pfl_names == NULL) {
		printf("cannot open names file");
		return ERR_CODE_OPEN_FILE;
	}
	while (fgets(line, MAX_INPUT_LINE_LENGTH, pfl_names)) {
		char delim[] = " ";		// split the path by this char
		strtok_s(line, MAX_INPUT_LINE_LENGTH, delim, name);

	}
	
}