#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	/*my first program in C*/
	if (argc != 4){ /*If the user tries to put in too many arguments, slap their wrist*/
		printf("Error: Incorrect amount of arguments");
		return 1;
	}
	printf("%s", argv[1]);
	printf("\n");
	printf("%s", argv[2]);
	printf("\n");
	printf("%s", argv[3]);
	return 0;
}