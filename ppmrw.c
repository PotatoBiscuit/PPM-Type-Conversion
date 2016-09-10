#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	if (argc != 4){ /*If the user tries to put in too many/too little arguments, slap their wrist*/
		fprintf(stderr, "Error: Incorrect amount of arguments");
		return 1;
	}
	
	if (*argv[1] == '3'){	/*If the user types a '3' in the command line, we must convert to P3 PPM format*/
		printf("Converting to P3 format...\n\n");
		return 0;
	}
	
	if(*argv[1] == '6'){	/*If the user types a '6' in the command line, we must convert to P6 PPM format*/
		printf("converting to P6 format...\n\n");
		return 0;
	}
	
	/*If neither a 3 nor 6 were entered in the command line, the program was used incorrectly*/
	fprintf(stderr, "\nYou did not provide valid command line arguments\nRemember: ppmrw 3/6(Type of PPM file) input.ppm output.ppm");
	return 1;
}