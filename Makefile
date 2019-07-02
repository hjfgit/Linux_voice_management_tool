Jarvis:Jarvis.cpp
	g++ -o $@ $^ -ljsoncpp -lcurl -lcrypto -std=c++11 
.PHONY:clean
clean:
	rm -rf Jarvis
