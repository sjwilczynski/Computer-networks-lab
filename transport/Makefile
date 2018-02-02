Flags = --std=gnu++11 -Wall -Wextra
CPP = g++

transport :  transport.o functions.o wrapper.o
	$(CPP) $(Flags) -o transport  transport.o functions.o wrapper.o 
transport.o : transport.cpp  functions.h wrapper.h
	$(CPP) -c $(Flags) transport.cpp -o transport.o
functions.o : functions.cpp functions.h wrapper.h 
	$(CPP) -c $(Flags) functions.cpp -o functions.o 
wrapper.o : wrapper.h wrapper.cpp
	$(CPP) -c $(Flags) wrapper.cpp -o wrapper.o 
clean:
	rm -f *.o *~
distclean:
	rm -f *.o *~ transport *.log