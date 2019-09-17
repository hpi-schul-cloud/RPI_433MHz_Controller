#ifndef API_LISTENER_H
#define API_LISTENER_H

class API_Listener{
	public:
	API_Listener(const char*, bool, const char*);
	int getFirealarm(bool);
	
	private:
	bool apiRequest(const char*, bool);
	int init();
};

#endif
