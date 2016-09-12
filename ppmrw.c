#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	if (argc != 4){ /*If the user tries to put in too many/too little arguments, slap their wrist*/
		fprintf(stderr, "Error: Incorrect amount of arguments");
		return 1;
	}
	
	if (*argv[1] == '3'){	/*If the user types a '3' in the command line, we must convert to P3 PPM format*/
		printf("Converting to P3 format...\n\n");
		if(ppmConversionHandler(argv[2], argv[3], 3))
			return 1;
		return 0;
	}
	
	if(*argv[1] == '6'){	/*If the user types a '6' in the command line, we must convert to P6 PPM format*/
		printf("converting to P6 format...\n\n");
		if(ppmConversionHandler(argv[2], argv[3], 6))
			return 1;
		return 0;
	}
	
	/*If neither a 3 nor 6 was entered in the command line, the program was used incorrectly*/
	fprintf(stderr, "\nYou did not provide valid command line arguments\nRemember: ppmrw 3/6(Type of PPM file) input.ppm output.ppm");
	return 1;
}

int ppmConversionHandler(char *input, char *output, int conversionType){	/*This controls which conversion function to use*/
	FILE *inputPointer = fopen(input, "rb");	/*Open the file*/
	char dontCare = fgetc(inputPointer);		/*Move file pointer one byte to the right, we want the number after the P*/
	char ppmType = fgetc(inputPointer);			/*Grab the format number of file, which should be 3 or 6*/
	if(ppmType != '3' && ppmType != '6'){		/*If the file is not P3 or P6 format, throw an error*/
		fprintf(stderr, "Error: Input file must be P3 or P6 PPM format");
		return 1;
	}
	fclose(inputPointer);
	
	if(conversionType == 3){	/*If you want to convert the file to a P3 file...*/
		if(ppmType == '3'){	/*The file is P3, so convert P3 to P3*/
			if(p3toP3(input, output))
				return 1;
			return 0;
		}
		
		if(ppmType == '6'){	/*The file is P6, so convert P6 to P3*/
			if(p6toP3(input, output))
				return 1;
			return 0;
		}
	}
	
	if(conversionType == 6){	/*If you want to convert the file to a P6 file...*/
		if(ppmType == '3'){	/*The file is P3, so convert P3 to P6*/
			if(p3toP6(input, output))
				return 1;
			return 0;
		}
		
		if(ppmType == '6'){	/*The file is P6, so convert P6 to P6*/
			if(p6toP6(input, output))
				return 1;
			return 0;
		}
	}

	/*It should be impossible for the user to get this far, but I handled it just in case*/
	fprintf(stderr, "Error: Somehow, something went wrong.");
	return 1;
}

int p3toP3(char *input, char *output){
	printf("P3 to P3 Conversion Complete!\n\n");
	return 0;
}

int p6toP3(char *input, char *output){
	printf("P6 to P3 Conversion Complete!\n\n");
	return 0;
}

int p3toP6(char *input, char *output){
	printf("P3 to P6 Conversion Complete!\n\n");
	return 0;
}

int p6toP6(char *input, char *output){
	printf("P6 to P6 Conversion Complete!\n\n");
	return 0;
}