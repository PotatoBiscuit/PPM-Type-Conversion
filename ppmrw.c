#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char* removeComments(char* input);

int main(int argc, char *argv[]){
	
	FILE * testOpen;	/*This pointer will test to see if the input file exists*/
	char* periodPointer;	/*This pointer will help us split filenames by '.', so that was can see if they are ppm files*/
	const char delim[2] = ".";	/*This will be our delimiter to split filenames with*/
	const char extension[4] = "ppm";	/*This will be the desired file ending for our input and output file*/
	
	if (argc != 4){ /*If the user tries to put in too many/too little arguments, slap their wrist*/
		fprintf(stderr, "\n\nError: Incorrect amount of arguments\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	

	if(!isdigit(*argv[1])){	/*If the first character of argument 2 is not a number, throw an error*/
		fprintf(stderr, "\n\nError: Second argument must be a single digit number [3/6]\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	
	if((testOpen = fopen(argv[2], "rb")) == NULL){	/*Try opening the input file to see if it exists, if it doesn't exist, throw error*/
		fprintf(stderr, "\n\nError: Input file must exist\n\n");
		return 1;
	}
	fclose(testOpen);	/*Close input file if it does exist*/
	
	periodPointer = strtok(argv[2], delim);	/*Points to the input filename before the '.'*/
	periodPointer = strtok(NULL, delim);	/*Points to the file extension of our input file*/
	if(periodPointer == NULL){	/*If the input file is not .ppm, throw and error*/
		fprintf(stderr, "\n\nError: Output file must be .ppm format\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	if(strcmp(periodPointer, extension) != 0){	/*If the input file is not .ppm, throw and error*/
		fprintf(stderr, "\n\nError: Input file must be .ppm format\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	
	periodPointer = strtok(argv[3], delim);	/*Check to see if the output file has the extension 'ppm'*/
	periodPointer = strtok(NULL, delim);
	if(periodPointer == NULL){
		fprintf(stderr, "\n\nError: Output file must be .ppm format\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	if(strcmp(periodPointer, extension) != 0){
		fprintf(stderr, "\n\nError: Output file must be .ppm format\nRemember: ppmrw [3/6](Type of PPM file) input.ppm output.ppm\n\n");
		return 1;
	}
	
	if (*argv[1] == '3'){	/*If the user types a '3' in the command line, we must convert to P3 PPM format*/
		printf("Converting to P3 format...\n\n");
		if(ppmConversionHandler(strcat(argv[2], ".ppm"), strcat(argv[3], ".ppm"), 3))
			return 1;
		return 0;
	}
	if(*argv[1] == '6'){	/*If the user types a '6' in the command line, we must convert to P6 PPM format*/
		printf("Converting to P6 format...\n\n");
		if(ppmConversionHandler(strcat(argv[2], ".ppm"), strcat(argv[3], ".ppm"), 6))
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
	int numDigitsColor = 0;			/*This will be the digit count of the file Max Color field*/
	char *widthPointer;				/*This will point to the Width string we pull from the file*/
	char *heightPointer;			/*This will point to the Width string we pull from the file*/
	char *colorPointer;				/*This will point to the Max Color string we pull from the file*/
	int width;						/*This is the numeric Width value*/
	int height;						/*This is the numeric Height value*/
	char ppmType;					/*Type of PPM file that we need to convert*/
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	

	if(fgetc(inputPointer) != 'P'){	/*Move file pointer one byte to the right, we want the number after the P*/
		fprintf(stderr, "Error: Header is not in the correct format");	/*Throw error if the first input file character is not 'P'*/
		return 1;
	}
	ppmType = fgetc(inputPointer);			/*Grab the format number of file, which should be 3 or 6*/
	if(ppmType != '3' && ppmType != '6'){	/*If the ppm format number is not 3 or 6, throw an error*/
		fprintf(stderr, "Error: Sorry, but this software only converts P3 and P6 ppm files");
		return 1;
	}

	if(!isspace(fgetc(inputPointer))){		/*Advance file pointer one byte, if it isn't whitespace, throw an error*/
		fprintf(stderr, "Error: Header is not in the correct format");
		return 1;
	}
	while(isspace(fgetc(inputPointer))){}	/*Advance through concurrent whitespace*/
	fseek(inputPointer, -1, SEEK_CUR);

	
	while(!isspace(fgetc(inputPointer))){	/*Iterate through the Width field*/
		if(filePointerCounter >= 10){	/*If Width field is more than ten digits, or the end of the file is reached, throw an error*/
			fprintf(stderr, "Error: Width field is limited to 10 digits, or Header info is incomplete");
			return 1;
		}
		filePointerCounter++;
	}

	widthPointer = malloc(sizeof(char)*(filePointerCounter + 1));		/*Create space for the Width field on this computer*/
	numDigitsWidth = filePointerCounter;						/*Store the digit amount of the Width field*/

	
	fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);		/*Move pointer back to the start of the Width field*/
	while(filePointerCounter){									/*Record each digit of the Width field*/
		*widthPointer = fgetc(inputPointer);
		if(!isdigit(*widthPointer)){							/*If there is non-number in the Width field, throw an error*/
			fprintf(stderr, "Error: Width field contains something other than a number");
			return 1;
		}
		filePointerCounter--;
		widthPointer++;
	}
	*widthPointer = '\0';
	
	while(isspace(fgetc(inputPointer))){}	/*Advance through concurrent whitespace*/
	fseek(inputPointer, -1, SEEK_CUR);
	
	widthPointer -= numDigitsWidth;								/*Move pointer pointing to Width info, back to its origin*/

	
	while(!isspace(fgetc(inputPointer))){						/*Do what we did for the Width field, for the Height field*/
		if(filePointerCounter >= 10){
			fprintf(stderr, "Error: Height field is limited to 10 digits, or Header info is incomplete");
			return 1;
		}
		filePointerCounter++;
	}
	heightPointer = malloc(sizeof(char)*(filePointerCounter + 1));
	numDigitsHeight = filePointerCounter;
	fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);

	
	while(filePointerCounter){
		*heightPointer = fgetc(inputPointer);
		if(!isdigit(*heightPointer)){
			fprintf(stderr, "Error: Height field contains something other than a number");
			return 1;
		}
		filePointerCounter--;
		heightPointer++;
	}
	*heightPointer = '\0';
	
	while(isspace(fgetc(inputPointer))){}	/*Advance through concurrent whitespace*/
	fseek(inputPointer, -1, SEEK_CUR);
	
	heightPointer -= numDigitsHeight;
	
	
	while(!isspace(fgetc(inputPointer))){	/*We will also do the same for the Max Color field*/
		if(filePointerCounter >= 3){	/*Max color field can be no more than 3 digits*/
			fprintf(stderr, "Error: Max color field is limited to 3 digits, or Header info is incomplete");
			return 1;
		}
		filePointerCounter++;
	}
	colorPointer = malloc(sizeof(char)*(filePointerCounter + 1));
	numDigitsColor = filePointerCounter;
	fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);
	
	while(filePointerCounter){
		*colorPointer = fgetc(inputPointer);
		if(!isdigit(*colorPointer)){
			fprintf(stderr, "Error: Max color field contains something other than a number");
			return 1;
		}
		filePointerCounter--;
		colorPointer++;
	}
	*colorPointer = '\0';
	colorPointer -= numDigitsColor;

	if(atoi(colorPointer) != 255){	/*If the Max Color field is not 255, the file is not in one byte format*/
		fprintf(stderr, "Error: Files must be in one byte format");
		return 1;
	}
	
	
	fclose(inputPointer);			/*Close input file*/
	width = atoi(widthPointer);
	height = atoi(heightPointer);
	numBytes = 3 * width * height;	/*Compute number of bytes in our image using our recorded Height and Width fields*/
	free(widthPointer);
	free(heightPointer);
	
	if(ppmType != '3' && ppmType != '6'){		/*If the file is not P3 or P6 format, throw an error*/
		fprintf(stderr, "Error: Input file must be P3 or P6 PPM format");
		return 1;
	}
	
	if(conversionType == 3){	/*If you want to convert the file to a P3 file...*/
		if(ppmType == '3'){	/*The file is P3, so convert P3 to P3*/
			if(p3toP3(input, output, numBytes, width))
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

/*This function is not being used*/
char* removeComments(char* input){	/*Attempted to create a function that removes comments, it did not work out*/
	FILE *inputPointer = fopen(input, "rb");
	FILE *tempInputPointer = fopen("tempInput.ppm", "wb");
	char bufferCharacter;
	char previousBuffer;
	int whitespaceCount = 0;
	int possibleComment = 0;
	
	while(whitespaceCount < 4){
		bufferCharacter = fgetc(inputPointer);
		if(bufferCharacter == -1){
			fprintf(stderr, "Error: Input header formatted improperly");
			return NULL;
		}
		if(isspace(bufferCharacter)){
			if(bufferCharacter == '\n'){
				possibleComment = 1;
			}
			while(1){
				bufferCharacter = fgetc(inputPointer);
				if(!isspace(bufferCharacter)){
					break;
				}
				if(bufferCharacter == '\n'){
					possibleComment = 1;
					printf("\n%c\n", bufferCharacter);
					previousBuffer = bufferCharacter;
					continue;
				}
				possibleComment = 0;
				printf("\n%c\n", bufferCharacter);
				previousBuffer = bufferCharacter;
			}
			if(bufferCharacter == -1){
				fprintf(stderr, "Error: Input header formatted improperly");
				return NULL;
			}
			if(bufferCharacter == '#' && possibleComment == 1){
					while((bufferCharacter = fgetc(inputPointer)) != '\n'){
						if(bufferCharacter == -1){
							fprintf(stderr, "Error: Input header formatted improperly");
							return NULL;
						}
					}
			}
			
			if(bufferCharacter == '#' && possibleComment == 0){
				fprintf(stderr, "Error: Comments must always begin at the start of a newline");
				return NULL;
			}
			
			fseek(inputPointer, -1, SEEK_CUR);
			whitespaceCount++;
			fprintf(tempInputPointer, "\n");
			continue;
		}
		fprintf(tempInputPointer, "%c", bufferCharacter);
	}
	fclose(inputPointer);
	fclose(tempInputPointer);
	return "tempInput.ppm";
}

int p3toP3(char *input, char *output, int numBytes, int width){
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "wb");	/*Open the output file*/
	
	char bufferCharacter;	/*This is a single character buffer for reading and writing the header*/
	int whitespaceCount = 0;	/*This will help us figure out when the header ends*/
	int totalWhitespaces = 0;	/*This is the amount of white spaces in the body of the file to be converted*/
	while(whitespaceCount < 4){		/*Until the end of the header is reached, copy header info to the output file*/
		bufferCharacter = fgetc(inputPointer);
		if(isspace(bufferCharacter)){
			while(isspace(fgetc(inputPointer))){}
			fseek(inputPointer, -1, SEEK_CUR);
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	whitespaceCount = 0;	/*Reset whitespace count, we will use it later*/
	
	/*Fancy arithmetic to calculate total whitespaces in the body of the input file*/
	totalWhitespaces = numBytes;
	
	while(whitespaceCount < totalWhitespaces){	/*Write the entire input file body to output, until all whitespace have been scanned*/
		bufferCharacter = fgetc(inputPointer);
		if(bufferCharacter == -1){	/*If the end of the file is reached before the expected whitespaces are scanned, file is not large enough*/
			fprintf(stderr, "Error: Input file is not as large as its width and height fields indicate");
			return 1;
		}
		if(isspace(bufferCharacter)){	/*More than one whitespace may be back to back*/
			whitespaceCount++;
			while(isspace(fgetc(inputPointer))){}
			fseek(inputPointer, -1, SEEK_CUR);
			fprintf(outputPointer, "\n");	/*Write a single \n for every group of whitespace*/
			continue;
		}
		if(!isdigit(bufferCharacter)){	/*If there is ever a non-whitespace non-number in the body of this P3 file, throw an error*/
			fprintf(stderr, "Error: Input file information formatted improperly");
			return 1;
		}
		fprintf(outputPointer, "%c", bufferCharacter);	/*Print characters to the output file*/
	}
	
	fclose(inputPointer);	/*Close input file*/
	fclose(outputPointer);	/*Close output file*/
	printf("P3 to P3 Conversion Complete!\n\n");	/*Show the user that the task was completed!*/
	return 0;	/*Return a successful number*/
}

int p6toP3(char *input, char *output, int numBytes, int width){
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "wb");	/*Open the output file*/
	
	unsigned char *buffer = malloc(sizeof(char)*numBytes);	/*Create buffer for later reading and writing use*/
	int sizeOfRead;		/*This will keep track of the total number of bytes read, in case something goes wrong*/
	char bufferCharacter;	/*This is a single character buffer for reading and writing the header*/
	int bufferNumber;	/*This will hold the color values to be inserted into the new P3 file*/
	int whitespaceCount = 0;	/*This will help us figure out when the header ends*/
	int genericCounter = 0;		/*This is a generic counter that will help print our values correctly*/
	int genericCounter1 = 0;	/*Same here*/
	
	fprintf(outputPointer, "P3\n");	/*Put the 'magic numbers' into our newly created P3 file*/
	fseek(inputPointer, 3, SEEK_SET);	/*Move the input file pointer past the input 'magic numbers', we don't need those*/
	
	while(isspace(fgetc(inputPointer))){}
	fseek(inputPointer, -1, SEEK_CUR);
	
	while(whitespaceCount < 3){		/*Until the end of the header is reached, copy header info to the output file*/
		bufferCharacter = fgetc(inputPointer);
		if(isspace(bufferCharacter)){
			while(isspace(fgetc(inputPointer))){}
			fseek(inputPointer, -1, SEEK_CUR);
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	
	sizeOfRead = fread(buffer, 1, numBytes, inputPointer);	/*Read non-header info from the input file into our buffer*/
	if(sizeOfRead < numBytes){	/*If the read function did not get enough bytes of info, warn the user*/
		fprintf(stderr, "Warning: Input file is not as large as its width and height fields indicate\n\n");
	}
	
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
	FILE *inputPointer = fopen(input, "rb");	/*Open the input file*/
	FILE *outputPointer = fopen(output, "wb");	/*Open the output file*/
	
	char *buffer = malloc(sizeof(char)*numBytes);	/*Create buffer for later reading and writing use*/
	char bufferCharacter;	/*This is a single character buffer for reading and writing the header*/
	char *valuePointer;		/*This will hold the string that contains individual color values for each pixel*/
	int filePointerCounter = 0;	/*Counter that will help us count digit length of each color value*/
	int numDigitsValue = 0;		/*Will hold the digit length of each color value*/
	int whitespaceCount = 0;	/*Counts total whitespaces for header info*/
	int sizeChecker = 0;		/*Makes sure each color values is at most 3 digits*/
	int valueChecker = 0;		/*Checks the amount of each color value, to ensure that it is in 1 byte format*/
	int numBytes1 = numBytes;	/*Seperate variable to record numBytes, so that the original can be used as a counter*/
	
	fprintf(outputPointer, "P6\n");	/*Put the 'magic numbers' into our newly created P6 file*/
	fseek(inputPointer, 3, SEEK_SET);	/*Move the input file pointer past the input 'magic numbers', we don't need those*/
	
	while(isspace(fgetc(inputPointer))){}
	fseek(inputPointer, -1, SEEK_CUR);
	
	while(whitespaceCount < 3){		/*Until the end of the header is reached, copy header info to the output file*/
		bufferCharacter = fgetc(inputPointer);
		if(isspace(bufferCharacter)){
			while(isspace(fgetc(inputPointer))){}
			fseek(inputPointer, -1, SEEK_CUR);
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	
	while(numBytes){	/*Convert the three digit numbers in the input file to a char, then store in a buffer*/
		while(!isspace(bufferCharacter = fgetc(inputPointer))){	/*Iterate through the color value*/
			filePointerCounter++;
			sizeChecker++;
			if(bufferCharacter == -1){	/*If end of file is reached, the file is too small, throw an error*/
				fprintf(stderr, "Error: Input file is not as large as its width and height fields indicate");
				return 1;
			}
			if(sizeChecker >= 4){	/*If an color value has more than three digits, throw an error*/
				fprintf(stderr, "Error: Input File has improperly formatted data");
				return 1;
			}
		}
		sizeChecker = 0;
		if(filePointerCounter == 0){	/*If an color value was not found, end of file has been reached, throw an error*/
			fprintf(stderr, "Error: Input file is not as large as its width and height fields indicate");
			return 1;
		}

		valuePointer = malloc(sizeof(char)*(filePointerCounter + 1));		/*Create space for the color value on this computer*/
		numDigitsValue = filePointerCounter;						/*Store the digit amount of the color value*/

		
		fseek(inputPointer, -1 - filePointerCounter, SEEK_CUR);		/*Move pointer back to the start of the color value*/
		while(filePointerCounter){									/*Record each digit of the color value*/
			*valuePointer = fgetc(inputPointer);
			filePointerCounter--;
			valuePointer++;
		}
		*valuePointer = '\0';
		
		valuePointer -= numDigitsValue;
		valueChecker = atoi(valuePointer);
		if(valueChecker > 255 || valueChecker < 0){				/*Ensure that the color value is between 0-255*/
			fprintf(stderr, "Error: Input File has improperly formatted data");
			return 1;
		}
		*buffer = valueChecker;	/*Store color value in buffer*/
		buffer++;
		while(isspace(fgetc(inputPointer))){}	/*Iterate through all concurrent whitespace*/
		fseek(inputPointer, -1, SEEK_CUR);
		free(valuePointer);		/*Free previously used memory*/
		numBytes--;
	}
	buffer -= (numBytes1);	/*Reset buffer pointer*/
	fwrite(buffer, sizeof(char), numBytes1, outputPointer);	/*Write buffer info into output file*/
	
	
	
	printf("P3 to P6 Conversion Complete!\n\n");	/*Notify user of success*/
	return 0;	/*Return a successful number*/
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
			while(isspace(fgetc(inputPointer))){}
			fseek(inputPointer, -1, SEEK_CUR);
			whitespaceCount++;
		}
		fprintf(outputPointer, "%c", bufferCharacter);
	}
	
	sizeOfRead = fread(buffer, 1, numBytes, inputPointer);	/*Read non-header info from the input file into our buffer*/
	if(sizeOfRead < numBytes){
		fprintf(stderr, "Warning: Input file is not as large as its width and height fields indicate\n\n");
	}
	fwrite(buffer, sizeof(char), sizeOfRead, outputPointer);	/*Write the data from our buffer into the output file*/
	
	free(buffer);
	fclose(inputPointer);	/*Close input file*/
	fclose(outputPointer);	/*Close output file*/
	printf("P6 to P6 Conversion Complete!\n\n");	/*Notify user of their success!*/
	return 0;	/*Return a successful number*/
}