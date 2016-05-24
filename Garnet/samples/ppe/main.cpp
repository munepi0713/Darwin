
// Avoid annoying VC's warning.
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <fstream>
#include <iostream>

#include <GetTime.h>
#include <PicturePerfectEngine.h>
#include <Exception.h>

int main(int argc, const char* argv[])
{
	std::string filename;

    //
    int mode = 0;
    for (int i = 1; i < argc; i++) {
        if ( argv[i][0] == '/' || argv[i][0] == '-' ) {
            switch ( argv[i][1] ) {
            case 'i' :
                mode = argv[i][1];
                break;
            }
        }
        else {
            switch ( mode ) {
            case 'i' :
				filename = argv[i];
                break;
			}
            mode = 0;
        }
    }

    std::ifstream fin;
    std::istream* ins;
	if ( filename.empty() ) {
        ins = &std::cin;
    }
    else {
        fin.open(filename.c_str());
        ins = &fin;
	}

	//
    try {
        PicturePerfectEngine* ppe = PicturePerfectEngine::createInstance();
	    ppe->execute(*ins);
	    ppe->destroy();
    }
    catch ( const char* msg ) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
    catch ( std::string msg ) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
    catch ( Exception e ) {
    	std::cerr << "[ERROR] " << e.msg << " (Code: " << e.code << ")." << std::endl;
    }

	//
	return 0;
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;
}


BOOL WINAPI DllEntryPoint( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	(void)hinstDLL;
	(void)fdwReason;
	(void)lpvReserved;

	return TRUE;
}

#endif

