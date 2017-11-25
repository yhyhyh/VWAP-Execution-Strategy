compile: ProcOrderBook.cpp
	g++ -std=c++ -c ProcOrderBook.cpp
	g++ -std=c++ -o ProcOrderBook ProcOrderBook.cpp
	./ProcOrderBook
	rm *.o
