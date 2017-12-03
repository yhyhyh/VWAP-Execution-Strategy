compile: vwap_strategy.cpp
	g++ -std=c++11 -c vwap_strategy.cpp
	g++ -std=c++11 -o vwap_strategy vwap_strategy.cpp
	./vwap_strategy
	rm *.o