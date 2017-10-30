compile: ProcOrderBook.cpp
	g++ -c ProcOrderBook.cpp
	g++ -o ProcOrderBook ProcOrderBook.cpp
	./ProcOrderBook
	rm *.o
