//Stanisław Wilczyński 272955
#ifndef FUN
#define FUN

#include "wrapper.h"
#include <string>
using namespace std;

const unsigned int ARGUMENTS = 4;
const unsigned int FRAME_LENGTH = 1000;
const unsigned int WINDOW_SIZE = 100;
const unsigned int MICROSECONDS_TO_WAIT = 5000;
const string SERVER_IP = "156.17.4.30";
const unsigned int SIZEOF_PACKAGE = 2024;

struct Frame{
	
	u_int8_t bytes[FRAME_LENGTH];
	unsigned int number,length;
	bool received;

	Frame();
	Frame(unsigned int number, unsigned int length);
};

bool checkInput(int argc, char **argv);
bool initialize();
void sendAndRead();
void closeFileAndSocket();
void updateFrame(char* buffer);
void updateWindow();
void printProgress();

#endif