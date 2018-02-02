//Stanisław Wilczyński 272955
#include "functions.h"
#include <cstdio>


int main(int argc, char **argv){

	if(!checkInput(argc,argv)){
		printf("Wrong program arguments, expected port, output file name, output file size\n");
		return EXIT_FAILURE;
	}
	if(!initialize()){
		printf("Initialization problems\n");
		return EXIT_FAILURE;
	}
	sendAndRead();
	closeFileAndSocket();
	return EXIT_SUCCESS;
}