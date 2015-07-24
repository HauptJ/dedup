all: deduptest

deduptest: dedup.o msb.o rabinpoly.o
	g++ -o deduptest dedup.o msb.o rabinpoly.o -lcrypto 
	
dedup.o: dedup.cpp
	g++ -c dedup.cpp -std=c++0x
	
msb.o: msb.cpp
	g++ -c msb.cpp
	
rabinpoly.o: rabinpoly.cpp
	g++ -c rabinpoly.cpp