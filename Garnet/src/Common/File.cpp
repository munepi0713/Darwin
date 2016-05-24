
#include <regex>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include <Common/File.h>
#include <Common/Text.h>

boost::filesystem::path  Common::File::findGenerationFile(const boost::filesystem::path& projdir, int generation, int maxGeneration)
{
	namespace fs = boost::filesystem;

	// Check the project directory is valid.
	if (!fs::exists(projdir) || !fs::is_directory(projdir)) {
		// Error. Project dir is not valid.
		return fs::path();
	}

	// Determine the generation directory type.
	//  Style (1): proj/generations/000000.txt
	//  Style (2): proj/000000/generation.000000.txt  (Obsolete)
	fs::path gendir = projdir / "generations";
	boost::wformat format;
	if (fs::exists(gendir) && fs::is_directory(gendir)) {
		// Style (1).
		format = boost::wformat(L"generations%2$s%1$06d.txt");
	}
	else {
		// Style (2).
		format = boost::wformat(L"%1$06d%2$sgeneration.%1$06d.txt");
	}

	// Determine the generation.
	auto getPath = [&projdir, &format](int generation) -> fs::path {
		fs::path genfile = projdir / str(format % generation % fs::path::preferred_separator);
		if (fs::exists(genfile) && fs::is_regular_file(genfile)) return genfile;
		
		fs::path genfilez = projdir / (str(format % generation % fs::path::preferred_separator) + L".gz");
		if (fs::exists(genfilez) && fs::is_regular_file(genfilez)) return genfilez;

		return fs::path();
	};

	fs::path genfile;
	if (generation == -1) {
		// Find the last generation.
		for (int i = 0; i < maxGeneration; i++) {
			fs::path temp = getPath(i);
			if (temp.empty()) break;
			genfile = temp;
		}
	}
	else {
		genfile = getPath(generation);
	}

	// Finished.
	return genfile;
}
