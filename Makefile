all:test Jarvis
test:test.cc
	g++ -o $@ $^ -ljsoncpp -lcurl -lcrypto -std=c++11 
Jarvis:Jarvis.cpp
	g++ -o $@ $^ -ljsoncpp -lcurl -lcrypto -std=c++11 
.PHONY: all clean
clean:
	rm -rf Jarvis test
