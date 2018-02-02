//Stanisław Wilczyński 272955
#include "functions.h"
#include <cstring>
#include <deque>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <sstream>


using namespace std;

int PORT;
int FILE_SIZE;
int sockfd;
FILE* file;
unsigned int NUMBER_OF_FRAMES;
unsigned int framesReceived = 0;
unsigned int lastFrameLength;
string FILE_NAME;
deque< unique_ptr< Frame > > window;
fd_set masterfd;    
fd_set readfd; 
int packages_sent;
int packages_received;

Frame::Frame(){
	memset(bytes, 0, FRAME_LENGTH);
	received = false;
}
Frame::Frame(unsigned int number, unsigned int length): number(number), length(length){
	memset(bytes, 0, FRAME_LENGTH);
	received = false;
}


bool initialize(){

	sockfd = my_socket(AF_INET, SOCK_DGRAM, 0);
	NUMBER_OF_FRAMES = FILE_SIZE / FRAME_LENGTH + (FILE_SIZE % FRAME_LENGTH ? 1 : 0);
	window.resize(0);
	lastFrameLength = FILE_SIZE % FRAME_LENGTH ? FILE_SIZE % FRAME_LENGTH : FRAME_LENGTH;
	for( unsigned int i = 0; i < min( WINDOW_SIZE, NUMBER_OF_FRAMES ); i++ ){
		window.push_back(unique_ptr< Frame >(new Frame(i,FRAME_LENGTH)));
	}
	if(framesReceived + window.size() == NUMBER_OF_FRAMES){
		window[ window.size() - 1 ]->length = lastFrameLength;
	}
	file = fopen (FILE_NAME.c_str(),"wb");
	if(file == NULL){
		printf("Cannot open file");
		return false;
	}
	FD_ZERO(&masterfd);    
    FD_ZERO(&readfd);
    FD_SET(sockfd, &masterfd);
    printProgress();
	return true;
}

void sendAndRead(){

	while(framesReceived != NUMBER_OF_FRAMES){

		sockaddr_in destination;
		destination.sin_family = AF_INET;
		destination.sin_port = htons(PORT);
		my_inet_pton(AF_INET, SERVER_IP.c_str(), &destination.sin_addr.s_addr);
		packages_sent = 0;

		for(unsigned int i = 0; i < window.size(); i++){

			if(!window[i]->received){

				unsigned int length = framesReceived + i + 1 == NUMBER_OF_FRAMES ? lastFrameLength : FRAME_LENGTH;
				char datagram[24];
				length = sprintf(datagram, "GET %u %u\n", FRAME_LENGTH * (framesReceived+i), length);
				//printf("%s",datagram);
				if( sendto(sockfd, datagram, length, 0, (sockaddr*) &destination, sizeof(destination)) < 0){
					printf("send problem for frame %u\n", framesReceived + i);
				} else{
					packages_sent++;
				}
			}
		}
		packages_received = 0;

		struct timeval timeout; 
		timeout.tv_sec = 0; 
		timeout.tv_usec = MICROSECONDS_TO_WAIT;
		
		readfd = masterfd;
		int ready = my_select (sockfd+1, &readfd, NULL, NULL, &timeout);

		while(ready){

			struct sockaddr_in sender;
			socklen_t sender_len = sizeof(sender);
			char buffer[SIZEOF_PACKAGE];
			ssize_t packageLength = my_recvfrom (sockfd, buffer, SIZEOF_PACKAGE, 0, (struct sockaddr*) &sender, &sender_len);
			if(packageLength > 0){
				char ipStr[20]; 
				inet_ntop (AF_INET, &(sender.sin_addr), ipStr, sizeof(ipStr));
				string ip = ipStr;
				if(ip == SERVER_IP){
					updateFrame(buffer);
				}
				readfd = masterfd;
				ready = my_select (sockfd+1, &readfd, NULL, NULL, &timeout);
				if(packages_received == packages_sent){
					break;
				}
			}
		}
		if(packages_received > 0){
			updateWindow();
		}
	}

}

void updateFrame(char* buffer){

	char data[4];
	int start;
	int length;
	sscanf(buffer, "%s %u %u", data, &start, &length);
	unsigned int howMany = 0;
	while(buffer[howMany] != '\n'){
		howMany++;
	}
	howMany++;
	unsigned int frameIndex;
	if(start/FRAME_LENGTH < window[0] -> number){
		return;
	}
	frameIndex = start/FRAME_LENGTH - window[0] -> number;
	if(frameIndex < window.size()){
		Frame* frame = window[frameIndex].get();
		if(!frame->received){
			frame->received = true;
			frame->length = length;
			memcpy(frame->bytes, buffer+howMany, length);
			packages_received++;
		}
		
	}
}

void updateWindow(){
	if(window.empty()){
		return;
	}
	unsigned int framesToAdd = 0;
	unsigned int lastIndex = window[window.size() - 1]->number;
	while(!window.empty() && window.front()->received){
		Frame* frame = window.front().get();
		framesReceived++;
		framesToAdd++;
		if(fwrite(frame->bytes,sizeof(char), frame->length, file) != frame->length){
			printf("fwrite error\n");
		}
		window.pop_front();
	}

	unsigned int realToAdd = min(framesToAdd, NUMBER_OF_FRAMES - framesReceived - (unsigned int)window.size());
	for(unsigned int i = 0; i < realToAdd; i++){
		window.push_back(unique_ptr< Frame >(new Frame(lastIndex+1+i,FRAME_LENGTH)));
	}
	if(!window.empty() && framesReceived + window.size() == NUMBER_OF_FRAMES){
		window[ window.size() - 1 ]->length = lastFrameLength;
	}
	if(framesToAdd > 0){
		printProgress();
	}

}

void printProgress(){
	double res = (double) framesReceived/ (double) NUMBER_OF_FRAMES;
	printf("%.4lf%% done\n", res*100);
}

void closeFileAndSocket(){
	close(sockfd);
	fclose(file);
}

bool checkInput(int argc, char **argv){
	if(argc != ARGUMENTS){
		return false;
	}
	PORT = atoi(argv[1]);
	FILE_NAME = argv[2];
	FILE_SIZE = atoi(argv[3]);
	if(PORT < 1024 || FILE_SIZE < 0){
		return false;
	}
	return true;
}












