#include "switch.h"
#include "../rc-switch/RCSwitch.h"

RCSwitch mySwitch;

Switch::Switch(){
	int PIN = 0; 
	wiringPiSetup();

    mySwitch = RCSwitch();
    mySwitch.enableTransmit(PIN);
}

void Switch::RS_ON(int CS_ON){
    mySwitch.send(CS_ON, 24);
}

void Switch::RS_OFF(int CS_OFF){
    mySwitch.send(CS_OFF, 24);
}
