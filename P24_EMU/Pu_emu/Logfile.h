int OpenLog();
int CloseLog();
int LogEvent(int Level, const char * Event);
int LogNum1(int Level, const char * Event, int n1);
int LogPacket(int Level, const char * Event, const void * buf, int len, int needlen);
