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
	fprintf(stderr, "\nYou did not provide valid command line arguments\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm");
	return 1;
}

int ppmConversionHandler(char *input, char *output, int conversionType){	/*This controls which conversion function to use*/
	int numBytes;					/*This will dictate how many bytes are used in the PPM files*/
	int filePointerCounter = 0;		/*This will help us gather the height and width file info*/
	int numDigitsWidth = 0;			/*This will be the digit count of the file Width field*/
	int numDigitsHeight = 0;		/*This will be the digit count of the file Height field*/
	char *widthPointer;				/*This will point to the Width string we pull from the file*/
	char *heightPointer;			/*This will point to the Width string we pull from the file*/
	int width;						/*This is the numeric Width value*/
	int height;						/*This is the numeric Height value*/
	char ppmType;					/*Type of PPM file that we need to convert*/
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	
	
	fseek(inputPointer, 1, SEEK_SET);		/*Move file pointer one byte to the right, we want the number after the P*/
	ppmType = fgetc(inputPointer);			/*Grab the format number of file, which should be 3 or 6*/

	fseek(inputPointer, 1, SEEK_CUR);		/*Advance file pointer one byte*/

	while(!isspace(fgetc(inputPointer))){	/*Iterate through the Width field*/
		filePointerCounter++;
	}

	widthPointer = malloc(sizeof(char)*filePointerCounter);		/*Create space for the Width field on this computer*/
	numDigitsWidth = filePointerCounter;						/*Store the digit amount of the Width field*/

	
	fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);		/*Move pointer back to the start of the Width field*/
	while(filePointerCounter){									/*Record each digit of the Width field*/
		*widthPointer = fgetc(inputPointer);
		filePointerCounter--;
		widthPointer++;
	}
	fseek(inputPointer, 1, SEEK_CUR);							/*Move file pointer to the start of the Height field*/
	widthPointer -= numDigitsWidth;								/*Move pointer pointing to Width info, back to its origin*/

	
	while(!isspace(fgetc(inputPointer))){						/*Do what we did for the Width field, for the Height field*/
		filePointerCounter++;
	}
	heightPointer = malloc(sizeof(char)*filePointerCounter);
	numDigitsHeight = filePointerCounter;
	fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);

	
	while(filePointerCounter){
		*heightPointer = fgetc(inputPointer);
		filePointerCounter--;
		heightPointer++;
	}
	heightPointer -= numDigitsHeight;
	
	
	fclose(inputPointer);			/*Close input file*/
	
	width = atoi(widthPointer);
	height = atoi(heightPointer);
	numBytes = 3 * width * height;	/*Computer number of bytes in our image using our recorded Height and Width fields*/
	
	if(ppmType != '3' && ppmType != '6'){		/*If the file is not P3 or P6 format, throw an error*/
		fprintf(stderr, "Error: Input file must be P3 or P6 PPM format");
		return 1;
	}
	
	if(conversionType == 3){	/*If you want to convert the file to a P3 file...*/
		if(ppmType == '3'){	/*The file is P3, so convert P3 to P3*/
			if(p3toP3(input, output, numBytes))
				return 1;
			return 0;
		}
		
		if(ppmType == '6'){	/*The file is P6, so convert P6 to P3*/
			if(p6toP3(input, output, numBytes, width))
				return 1;
			return 0;
		}
	}
	
	if(conversionType == 6){	/*If you want to convert the file to a P6 file...*/
		if(ppmType == '3'){	/*The file is P3, so convert P3 to P6*/
			if(p3toP6(input, output, numBytes))
				return 1;
			return 0;
		}
		
		if(ppmType == '6'){	/*The file is P6, so convert P6 to P6*/
			if(p6toP6(input, output, numBytes))
				return 1;
			return 0;
		}
	}

	/*It should be impossible for the user to get this far, but I handled it just in case*/
	fprintf(stderr, "Error: Somehow, something went wrong.");
	return 1;
}

int p3toP3(char *input, char *output, int numBytes){
	FILE *inputPointer = fopen(input, "r");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "w");	/*Open the output file*/
	
	char bufferCharacter;
	while(1){
		bufferCharacter = fgetc(inputPointer);
		if(bufferCharacter == EOF){
			break;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	fclose(inputPointer);
	fclose(outputPointer);
	printf("P3 to P3 Conversion Complete!\n\n");
	return 0;
}

int p6toP3(char *input, char *output, int numBytes, int width){
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "wb");	/*Open the output file*/
	
	unsigned char *buffer = malloc(sizeof(char)*numBytes);	/*Create buffer for later reading and writing use*/
	int sizeOfRead;		/*This will keep track of the total number of bytes read, in case something goes wrong*/
	char bufferCharacter;	/*This is a single character buffer for reading and writing the header*/
	long bufferNumber;	/*This will hold the color values to be inserted into the new P3 file*/
	int whitespaceCount = 0;	/*This will help us figure out when the header ends*/
	int genericCounter = 0;		/*This is a generic counter that will help print our values correctly*/
	int genericCounter1 = 0;	/*Same here*/
	
	fprintf(outputPointer, "P3\n");	/*Put the 'magic numbers' into our newly created P3 file*/
	fseek(inputPointer, 3, SEEK_SET);	/*Move the input file pointer past the input 'magic numbers', we don't need those*/
	
	while(whitespaceCount < 3){		/*Until the end of the header is reached, copy header info to the output file*/
		bufferCharacter = fgetc(inputPointer);
		if(isspace(bufferCharacter)){
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	
	sizeOfRead = fread(buffer, 1, numBytes, inputPointer);	/*Read non-header info from the input file into our buffer*/
	
	while(sizeOfRead){	/*While there are still bytes in the buffer to write, continue*/
		if(genericCounter == 2 && genericCounter1 == width - 1){	/*If final column of picture is reached, write color value, then new-line*/
			bufferNumber = *(buffer++);
			fprintf(outputPointer, "%d\n", bufferNumber);
			genericCounter = 0;
			genericCounter1 = 0;
			sizeOfRead--;
			continue;
		}
		if(genericCounter == 2){	/*If final value of an RGB triplet is reached, write color value, then two spaces*/
			bufferNumber = *(buffer++);
			fprintf(outputPointer, "%d  ", bufferNumber);
			genericCounter = 0;
			genericCounter1++;
			sizeOfRead--;
			continue;
		}
		bufferNumber = *(buffer++);		/*Write color value, then add a space*/
		fprintf(outputPointer, "%d ", bufferNumber);
		genericCounter++;
		sizeOfRead--;
	}
	
	fclose(inputPointer);	/*Close input file*/
	fclose(outputPointer);	/*Close output file*/
	
	printf("P6 to P3 Conversion Complete!\n\n");	/*Tell the user about the success!*/
	return 0;	/*Return successful int*/
}

int p3toP6(char *input, char *output, int numBytes){
	printf("P3 to P6 Conversion Complete!\n\n");
	return 0;
}

int p6toP6(char *input, char *output, int numBytes){
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "wb");	/*Open the output file*/
	
	char *buffer = malloc(sizeof(char)*numBytes);	/*Create buffer for later reading and writing use*/
	int sizeOfRead;		/*This will keep track of the total number of bytes read, in case something goes wrong*/
	char bufferCharacter;	/*This is a single character buffer for reading and writing the header*/
	int whitespaceCount = 0;	/*This will help us figure out when the header ends*/
	while(whitespaceCount < 4){		/*Until the end of the header is reached, copy header info to the output file*/
		bufferCharacter = fgetc(inputPointer);
		if(isspace(bufferCharacter)){
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	
	sizeOfRead = fread(buffer, 1, numBytes, inputPointer);	/*Read non-header info from the input file into our buffer*/
	fwrite(buffer, 1, sizeOfRead, outputPointer);	/*Write the data from our buffer into the output file*/
	
	
	fclose(inputPointer);	/*Close input file*/
	fclose(outputPointer);	/*Close output file*/
	printf("P6 to P6 Conversion Complete!\n\n");	/*Notify user of their success!*/
	return 0;	/*Return a successful number*/
}