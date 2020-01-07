run: switch.o api_listener.o run.cpp
	g++ -DRPI rc-switch/RCSwitch.cpp run.cpp switch.o api_listener.o -o run -lwiringPi -lcurl -ljsoncpp -lpthread -ldl -std=c++11

switch.o: modules/switch.cpp modules/switch.h
	g++ -DRPI rc-switch/RCSwitch.cpp -c modules/switch.cpp -lwiringPi

api_listener.o: modules/api_listener.cpp modules/api_listener.h
	g++ -c modules/api_listener.cpp

clean:
	rm *.o run
