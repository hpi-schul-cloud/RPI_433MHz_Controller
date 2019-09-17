#include "rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <time.h>
#include <unistd.h>

#include "loguru/loguru.cpp"
#include "modules/switch.h"
#include <string>

using namespace std;

bool DEBUG = true;

int wait = 30;
time_t nextAuth;
bool deleted = true;

//Config
int config_csOn, config_csOff;
string config_apiUrl = "";
string config_apiUserpw = "";
int config_apiUpdateNoAlarm, config_apiUpdateAlarm, config_apiAuth;

constexpr unsigned int str2int(const char* str, int h = 0){
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

string readConfigLine(string str){
    unsigned first = str.find('"');
    return str.substr(first+1, str.length()-1);
}

bool checkConfig(){
    if(config_csOn == 0 || config_csOff == 0 || config_apiUserpw == "" || config_apiUrl == "" ||
        config_apiUpdateAlarm == 0 || config_apiUpdateNoAlarm == 0 || config_apiAuth == 0){
        LOG_F(ERROR,"Config file is missing data or wrongly configured!");
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
                            config_csOn = stoi(readConfigLine(line));
                            break;
                        case str2int("CS_OFF"):
                            config_csOff = stoi(readConfigLine(line));
                            break;
                        case str2int("API_URL"):
                            config_apiUrl = readConfigLine(line);
                            break;
                        case str2int("API_USERPW"):
                            config_apiUserpw = readConfigLine(line);
                            break;
                        case str2int("API_UPDATE_noAlarm"):
                            config_apiUpdateNoAlarm = stoi(readConfigLine(line));
                            break;
                        case str2int("API_UPDATE_Alarm"):
                            config_apiUpdateAlarm = stoi(readConfigLine(line));
                            break;
                        case str2int("API_AUTH"):
                            config_apiAuth = stoi(readConfigLine(line));
                            break;
                        case str2int("DEBUG"):
                            DEBUG = (readConfigLine(line) == "true");
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
    system("exec rm -r log/*"); //clear log
    
    time_t t = time(0);
    tm* now = localtime(&t);
    
    nextAuth = t + config_apiAuth;
        
    //start logging
    loguru::init(argc, argv);
    string file = "log/" + to_string(now->tm_mday) + to_string(now->tm_mon + 1) + to_string(now->tm_year + 1900) + ".log";
    loguru::add_file(file.c_str(), loguru::Append, loguru::Verbosity_MAX);
    
    //load config
    loadConfig();
        
    //init switch and api
    Switch rcs = Switch(config_csOn,config_csOff);
    API_Listener api = API_Listener(config_apiUrl.c_str(), DEBUG, config_apiUserpw.c_str());
    config_apiUserpw = "";
        
    if(DEBUG){
        cout << config_csOn << ", " << config_csOff << ", " << config_apiUrl << ", " << config_apiUpdateAlarm << ", " << config_apiUpdateNoAlarm <<'\n';
    }
    
    //endless loop
    while(true){
        t = time(0);
        tm* now = localtime(&t);
        
        int s;
        if(nextAuth <= t){
            s = api.getFirealarm(true);
            nextAuth = t + config_apiAuth;
        }else{
            s = api.getFirealarm(false);
        }
        
        switch(s){
            case 1:
                LOG_F(WARNING, "New firealarm");
                rcs.on(); 
                wait = config_apiUpdateAlarm;
                break;
            case 0:
                LOG_F(INFO, "No firealarm");
                rcs.off();
                wait = config_apiUpdateNoAlarm;
                break;
            case -1:
                LOG_F(ERROR, "API Request failed. Please check api url and make sure internet connection is available.");
                wait = 10;
                break;
            default:
                break;
        }

        //clear log very day at 10
        if(!deleted && now->tm_hour == 12){
            system("exec rm -r log/*");
            string file = "log/" + to_string(now->tm_mday) + to_string(now->tm_mon + 1) + to_string(now->tm_year + 1900) + ".log";
            loguru::add_file(file.c_str(), loguru::Append, loguru::Verbosity_MAX);
            LOG_F(WARNING, "Old logs have been removed");
            deleted = true;
        }
        if(now ->tm_hour > 12 && deleted){
            deleted = false;
        }
        
        sleep(wait);
    }
}
