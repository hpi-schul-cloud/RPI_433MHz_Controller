#include "switch.h"
#include "../rc-switch/RCSwitch.h"

RCSwitch mySwitch;

int cs_on;
int cs_off;

Switch::Switch(int _cs_on, int _cs_off){
    cs_on = _cs_on;
    cs_off = _cs_off;
    
    int PIN = 0; 
    wiringPiSetup();

    mySwitch = RCSwitch();
    mySwitch.enableTransmit(PIN);
}

void Switch::on(){
    mySwitch.send(cs_on, 24);
}

void Switch::off(){
    mySwitch.send(cs_off, 24);
}
