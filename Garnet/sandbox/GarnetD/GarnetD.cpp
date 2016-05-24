/**
 *  Garnet
 */

#include <GarnetD.h>

void  printUsage();

/**
 *
 */
int main(int argc, const char* argv[])
{
	//
    GarnetDConf conf;
    
    //
    int mode = 0;
    for (int i = 1; i < argc; i++) {
        if ( argv[i][0] == '/' || argv[i][0] == '-' ) {
            switch ( argv[i][1] ) {
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
            case 'a' :
            case 'd' :
            case 'g' :
            case 't' :
			case 'D' :
			case 'E' :
			case 'G' :
			case 'H' :
			case 'W' :
                mode = argv[i][1];
                break;
            case 'c' :
                conf.outputCSV = true;
                break;
            case 'f' :
                conf.outputImage = true;
                break;
            case 'h' :
                printUsage();
                return 0;
			case 'v' :
                conf.verbose = true;
                break;
			case 'x' :
                conf.outputHTML = true;
                break;
            }
        }
        else {
            switch ( mode ) {
            case '0' :
            case '1' :
            case '2' :
            case '3' :
            case '4' :
            case '5' :
            case '6' :
            case '7' :
            case '8' :
            case '9' :
                conf.srcs[mode - '0'] = argv[i];
                break;
            case 'a' :
                conf.aliasDics.push_back(argv[i]);
                break;
            case 'd' :
                conf.workingDir = argv[i];
                break;
            case 'g' :
                conf.initialGen = strtol(argv[i], 0, 0);
				break;
            case 't' :
                conf.trees.push_back(argv[i]);
                break;
            case 'D' :
                conf.mode = MODE_GARNET_DAEMON;
                conf.project = argv[i];
                break;
            case 'E' :
                conf.mode = MODE_EVAL_BATCH;
                conf.project = argv[i];
                break;
            case 'G' :
                conf.mode = MODE_GARNET_BATCH;
                conf.project = argv[i];
                break;
            case 'H' :
                conf.height = strtol(argv[i], 0, 0);
                break;
            case 'W' :
                conf.width = strtol(argv[i], 0, 0);
                break;
            }
            mode = 0;
        }
    }

    // Execute.
    int result = 0;
    try {
        switch ( conf.mode ) {
        case MODE_GARNET_DAEMON : 
            result = execute_garnet_daemon(conf);
            break;
        case MODE_GARNET_BATCH : 
            result = execute_garnet_batch(conf);
            break;
        case MODE_EVAL_BATCH : 
            result = execute_eval_batch(conf);
            break;
        case MODE_EVAL_COMMAND : 
            result = execute_eval_command(conf);
            break;
        }
    }
    catch ( Exception e ) {
    	std::cerr << "[ERROR] " << e.msg << " (Code: " << e.code << ")." << std::endl;
        result = 1;
    }

    return result;
}

/**
 *
 */
void  printUsage()
{
	std::cerr << 
			"Garnet\n"
            "(c) 2008-2011 Munehiro Doi\n"
            "All rights reserved.\n"
			"\n"
			"[usage]\n"
            "  Garnet <ProjectName or ProfileFile> [-d WorkingDir] [-g GenerationFile]\n"
			<< std::endl;
}

void  setNumCPUs(int num)
{
#if defined(_WIN32)
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;
    if ( GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask) ) {
        // Count up the number of processes available for our app.
        int n = 0;
        for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
            if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) n++;
        }

        // Limit max CPUs by user configuration.
        DWORD_PTR newAffinityMask = 0;
        if ( num < n ) {
            int m = 0;
            for (int i = 0; i < sizeof(DWORD_PTR) * 4; i++) {
                if ( (systemAffinityMask & ((DWORD_PTR)1 << i)) ) {
                    newAffinityMask |= ((DWORD_PTR)1 << i);
                    m++;
                    if ( m == num ) break;
                }
            }
            newAffinityMask <<= (n - num);
        }
        else {
            newAffinityMask = systemAffinityMask;
        }

        // Set new affinity mask.
        SetProcessAffinityMask(GetCurrentProcess(), newAffinityMask);
    }
#else
#endif
}
