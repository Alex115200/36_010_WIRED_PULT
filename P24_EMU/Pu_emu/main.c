#define  DEFINE_MAIN_GLOBAL

//#include "win32.h"
#include "OS.h"
#include "windows.h"


#include "globalIncludes.h"


//void __stdcall InitDllIFace(void);

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
  return 1;
}
*/

void OS_init(void);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

//    OS_init();

  return 1;
}
