#include "rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "modules/switch.h"
#include <string>

using namespace std;

//global
char input;

bool DEBUG;

int CS_ON;  //code to turn rc-switch on
int CS_OFF; //code to turn rc-switch off
string API_URL = "";

constexpr unsigned int str2int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

string readConfigLine(string str){
    unsigned first = str.find('"');
    return str.substr(first+1, str.length()-1);
}

bool checkConfig(){
    if(CS_ON == 0 || CS_OFF == 0 || API_URL == ""){
        cout << "[\033[1;31mERR\033[0m] config file is missing data or wrongly configured!\n";
        return false;
    } 
    cout << "[\033[1;32mOK\033[0m] config file loaded\n";
    return true;
}

void loadConfig(){
    ifstream cfile("config/Config.cfg");
    if(cfile.good()){
        for(string line; getline(cfile, line); ){
            if(line.size() != 0){ //skip empty line
                if(line.at(0) != '#'){ //skip comments in config
                    char cstr[line.size()+1];
                    strcpy(cstr, line.c_str());
                    switch(str2int(strtok(cstr, ":")))
                    {
                        case str2int("CS_ON"):
                            CS_ON = stoi(readConfigLine(line));
                            break;
                        case str2int("CS_OFF"):
                            CS_OFF = stoi(readConfigLine(line));
                            break;
                        case str2int("API_URL"):
                            API_URL = readConfigLine(line);
                            break;
                        case str2int("DEBUG"):
                            DEBUG = (readConfigLine(line).compare("true"));
                            break;
                    }
                }
            }
        }
        checkConfig();
    }else{
        cout << "[\033[1;31mERR\033[0m] config file not found!\n";
    }
}

int main(int argc, char *argv[]) {
    loadConfig();
    
    
    if(DEBUG)
        cout << CS_ON << ", " << CS_OFF << ", " << API_URL << '\n';
        
    Switch s;
    s.RS_ON(CS_ON);
}
