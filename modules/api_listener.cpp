#include "api_listener.h"
#include <string>
#include <stdio.h>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include <iostream>

using namespace std;

CURL *curl;
CURLcode res;

Json::Reader reader;
Json::Value obj;

string response;
const char* urlToFilter;
const char* userpw;
string searchUrl;

bool debug = false;

size_t write_to_string(void *ptr, size_t size, size_t count, void *stream) {
  ((string*)stream)->append((char*)ptr, 0, size*count);
  return size*count;
}

API_Listener::API_Listener(const char* URL, bool _debug, const char* _userpw){
	urlToFilter = URL;
	userpw = _userpw;
	debug = _debug;
	init();
}

int API_Listener::getFirealarm(bool auth){
	if(apiRequest(searchUrl.c_str(),auth)){
		reader.parse(response, obj);
		if(obj["total"].asInt() >= 1)
			return 1;
		return 0;
	}
	return -1;
}

int API_Listener::init(){
	if(apiRequest(urlToFilter, true)){
		reader.parse(response, obj);
		searchUrl = obj["searchUrl"].asString();
		return getFirealarm(false);
	}else{
		return -1;
	}
}

bool API_Listener::apiRequest(const char* url, bool auth){
	bool success = true;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	response = "";
	if(curl){
		if(auth){
			curl_easy_setopt(curl, CURLOPT_USERPWD, userpw);
			if(debug)
				cout << "with userpw\n";
		}
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_string);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		
		struct curl_slist *headers = NULL;
		
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		
		while((res = curl_easy_perform(curl)) != CURLE_OK){
			switch(res){
				case CURLE_COULDNT_CONNECT:
				case CURLE_COULDNT_RESOLVE_HOST:
				case CURLE_COULDNT_RESOLVE_PROXY:
					success = false;
					break;
				default:
					success = false;
					break;
			}
		}	
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
	
	if(debug)
		cout << "-------------------------------------- \n" << response << "\n--------------------------------------- \n";
	
	return success;
}
