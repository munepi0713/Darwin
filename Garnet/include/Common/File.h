#ifndef COMMON__FILE__H__
#define COMMON__FILE__H__

#include <iostream>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

#include <Common/Text.h>

namespace Common {
	namespace File {
		template <typename Reader>
		boost::filesystem::path  readFromFile(const boost::filesystem::path& path, Reader reader)
		{
			namespace fs = boost::filesystem;

			fs::path ret;

			// Attempt 1: Reading from compressed file.
			if (Text::endsWith(path.filename().string(), ".gz")) {
				ret = readFromFileCompressed(path, reader);
			}

			// Attempt 2: Reading from uncompressed file.
			if (ret.empty()) {
				ret = readFromFileUncompressed(path, reader);
			}

			// Attempt 3: Reading from compressed file again.
			if (ret.empty()) {
				ret = readFromFileCompressed(path, reader);
			}

			// Return the actual path if any attempt has been successful. Otherwise, it should be empty.
			return ret;
		}
		template <typename Reader>
		boost::filesystem::path  readFromFileCompressed(boost::filesystem::path file, Reader reader)
		{
			namespace fs = boost::filesystem;
			namespace io = boost::iostreams;

			bool isValid = false;
			if (fs::exists(file) && !fs::is_directory(file)) {
				isValid = true;
			}
			else {
				file = fs::path(file.string() + ".gz");
				if (fs::exists(file) && !fs::is_directory(file)) {
					isValid = true;
				}
			}

			if (isValid) {
				fs::ifstream fin(file, std::ios::in | std::ios::binary);
				if (fin) {
					io::filtering_istream ins;
					ins.push(io::gzip_decompressor());
					ins.push(fin);
					reader(ins);
					return file;
				}
			}

			// Failed. Return empty path.
			return boost::filesystem::path();
		}
		template <typename Reader>
		boost::filesystem::path  readFromFileUncompressed(boost::filesystem::path file, Reader reader)
		{
			namespace fs = boost::filesystem;
			if (fs::exists(file)) {
				fs::ifstream ins(file);
				if (ins) {
					reader(ins);
					return file;
				}
			}

			// Failed. Return empty path.
			return boost::filesystem::path();
		}

		template <typename Writer>
		boost::filesystem::path  writeToFile(const boost::filesystem::path& path, Writer writer, bool isCompressed)
		{
			if (boost::algorithm::ends_with(path.string(), ".gz")) {
				return writeToFileCompressed(path, writer);
			}
			else if (isCompressed) {
				return writeToFileCompressed(path.string() + ".gz", writer);
			}
			else {
				return writeToFileUncompressed(path, writer);
			}
		}
		template <typename Writer>
		boost::filesystem::path  writeToFileUncompressed(const boost::filesystem::path& path, Writer writer)
		{
			boost::filesystem::path dstfile(path);
			boost::filesystem::path tmpfile = dstfile.parent_path() / boost::filesystem::unique_path();

			// 1. Save contents to a temporary file.
			boost::filesystem::ofstream outs(tmpfile);
			writer(outs);
			outs.close();

			// 2. Rename the temporary file to the original name.
			boost::filesystem::rename(tmpfile, dstfile);

			// Return a path object of the dstfile.
			return dstfile;
		}
		template <typename Writer>
		boost::filesystem::path  writeToFileCompressed(const boost::filesystem::path& path, Writer writer)
		{
			boost::system::error_code ec;
			boost::filesystem::path dstfile(path);
			boost::filesystem::path tmpfile = dstfile.parent_path() / boost::filesystem::unique_path();

			// 1. Save contents to a temporary file.
			boost::filesystem::ofstream fout(tmpfile, std::ios::out | std::ios::binary);
			boost::iostreams::gzip_params params(
				boost::iostreams::gzip::best_compression,
				boost::iostreams::gzip::deflated,
				boost::iostreams::gzip::default_window_bits,
				9,
				boost::iostreams::gzip::default_strategy);
			boost::iostreams::filtering_ostream outs;
			outs.push(boost::iostreams::gzip_compressor(params));
			outs.push(fout);
			writer(outs);
			boost::iostreams::close(outs);
			fout.close();

			// 2. Rename the temporary file to the original name.
			boost::filesystem::rename(tmpfile, dstfile, ec);
			if (ec) {
				std::cerr << ec.message() << std::endl;
				throw ec.message();
			}


			// Return a path object of the dstfile.
			return dstfile;
		}

		/**
		*  Returns a path to a data file of the specified generation.
		*
		*  @param[in] projdir     The path to the project directory.
		*  @param[in] generation  The generation. Finds the last generation if -1 is passed.
		*  @return                The path to the generation file. If the specified path does not exist,
		*                         an empty path will be returned.
		*/
		boost::filesystem::path  findGenerationFile(const boost::filesystem::path& projdir, int generation = -1, int maxGeneration = 999999);
	}//end of namespace File
}//end of namespace Common

#endif//COMMON__FILE__H__
