
// Avoid annoying VC's warning.
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <fstream>
#include <iostream>

#include <boost/format.hpp>

#include <Common/Text.h>
#include <Common/GetTime.h>

#include <PicturePerfectEngine/Compiler.h>
#include <PicturePerfectEngine/Engine.h>

namespace {

struct Configuration
{
	bool isOclEnabled;
	cv::Size size;

	Configuration() {
		isOclEnabled = false;
		size = cv::Size(256, 256);
	}
};


void loadConfiguration(Configuration& conf, std::istream& ins)
{
	while ( !ins.eof() && ins.good() ) {
		std::string line;
		std::getline(ins, line);
		line = Text::removeAfter(Text::trimLeft(line), "#");
		if ( line.empty() ) continue;
			
		strings tokens = parse(line);
		if ( Text::equalsWithoutCase(tokens[0], "Size") ) {
			if ( tokens.size() < 3 ) {
				PICTUREPERFECT_THROW(NumberOfArgsException, "Size command requires 3 arguments");
			}
			conf.size = cv::Size(strtoul(tokens[1].c_str(), 0, 0), strtoul(tokens[2].c_str(), 0, 0));
		}
		else if ( Text::equalsWithoutCase(tokens[0], "OpenCL") ) {
#if defined(GARNET_ENABLE_OPENCL__)
			if ( Text::equalsWithoutCase(tokens[1], "True") ) {
				conf.isOclEnabled = true;
			}
			else {
				conf.isOclEnabled = false;
			}
#else
			conf.isOclEnabled = false;
#endif
		}
	}
}

}

int main(int argc, const char* argv[])
{
	std::string filename;

    // Analyze Arguments
    int mode = 'i';
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
            mode = 'i';
        }
    }

    try {
		// 1. Load script
		// 1.1 Open input script.
		std::ifstream fin;
		std::istream* ins;
		if ( filename.empty() ) {
#if 0
			ins = &std::cin;
#else
			std::cerr << "Needs filename of a script." << std::endl;
			return 1;
#endif
		}
		else {
			fin.open(filename.c_str());
			if ( fin.fail() ) {
				std::cerr << "Failed to open " << filename << "." << std::endl;
				return 1;
			}
			ins = &fin;
		}
		// 1.2 Seperate the script into control, archive, and execution parts.
		std::stringstream cins, ains, eins;
		std::ostream* ous = &cins;
		while ( !ins->eof() && ins->good() ) {
			std::string line;
			std::getline(*ins, line);
			line = Text::removeAfter(Text::trimLeft(line), "#");
			if ( line.empty() ) continue;
			
			strings tokens = parse(line);
			if ( Text::equalsWithoutCase(tokens[0], "Begin") ) {
				if ( tokens.size() > 1 && Text::equalsWithoutCase(tokens[1], "Archive") ) {
					ous = &ains;
				}
				else if ( tokens.size() > 1 && Text::equalsWithoutCase(tokens[1], "Execution") ) {
					ous = &eins;
				}
			}
			else if ( Text::equalsWithoutCase(tokens[0], "End") && tokens.size() >= 2 ) {
				ous = &cins;
			}
			else {
				(*ous) << line << '\n';
			}
		}
		ains.flush();
		std::string ainsSave = ains.str();

		// 1.2 Save the directory path where the script is.
		std::wstring basedir = Text::toWideChar(filename);
		std::wstring::size_type pos = basedir.find_last_of(L"\\/");
		if ( pos != std::wstring::npos ) {
			basedir = basedir.substr(0, pos);
			PicturePerfect::PicturePerfectEngine::setBaseDirectory(basedir);
		}

		// 1.3 Load configuration
		Configuration conf;
		loadConfiguration(conf, cins);

		// 2. Create PPE instance.
		PicturePerfect::PicturePerfectEnginePtr ppe;
		ppe = PicturePerfect::PicturePerfectEngine::createInstance(
			conf.isOclEnabled ? "ocl" : "noocl");
#if 0
		ppe->setImageSize(conf.size);
#endif

		// 4. Rehearse PPE execution units.
		if ( conf.isOclEnabled ) {
			ppe->rehearse(conf.size);
		}

		// 5. Create and set up archive contents.
		PicturePerfect::ArchivePtr source;
		PicturePerfect::ArchivePtr result;
		source = PicturePerfect::Archive::createInstance(conf.size);
		result = PicturePerfect::Archive::createInstance(conf.size);
		ains.str(ainsSave);
		ains.clear();
		source->load(ains);

		// 6. Compile
		PicturePerfect::ExecutablePtr executable;
		PicturePerfect::CompilerPtr compiler;
		compiler = PicturePerfect::Compiler::createInstance();
		executable = compiler->compile(eins);

		// 7. Execute.
		InfoContainer info;
		ppe->execute(executable, source, result, info);

		// 8. Save archive contents.
		ains.str(ainsSave);
		ains.clear();
		result->save(ains);

		// 9. Show result.
		double t;
		info.getInformation(PicturePerfect::INFO_COMPILATION_TIME, &t);
		std::cout << boost::format("Compile: %.6fs\n") % t;
		info.getInformation(PicturePerfect::INFO_EXECUTION_TIME, &t);
		std::cout << boost::format("Execute: %.6fs\n") % t;
    }
    catch ( const char* msg ) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
    catch ( std::string msg ) {
        std::cerr << "[ERROR] " << msg << std::endl;
    }
    catch ( PicturePerfect::Exception e ) {
    	std::cerr << "[ERROR] " << e.what() << std::endl;
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

