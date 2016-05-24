#ifndef PICTURE_PERFECT_ENGINE__EXCEPTION__H__
#define PICTURE_PERFECT_ENGINE__EXCEPTION__H__

#include <string>
#include <exception>

#include <boost/format.hpp>

namespace PicturePerfect
{

enum ExceptionCode
{
	CODE_SUCCESS = 0,
	CODE_NUMBER_OF_ARGS,
	CODE_INVALID_OPERATION,
	CODE_INVALID_STORAGE_ACCESS,
	CODE_INVALID_NUMBER_FORMAT,
	CODE_ADDRESS_OUT_OF_RANGE,
	CODE_FILE_OPEN_FAILURE,
	CODE_INVALID_INSTRUCTION,
	CODE_NOT_IMPLEMENTED,
	CODE_INVALID_STORAGE_OPERATION,
	CODE_MAX
};

struct Exception : public std::exception
{
private :
	std::string  msg;

public :
	virtual const char* what() const override {
		return msg.c_str();
	}

	Exception(const std::string& msg_)
		: std::exception() {
			msg = msg_;
		}
	Exception(const std::string& msg_, unsigned int code_, const char* file_, unsigned int line_)
		: std::exception() {
			msg = (boost::format("[%s:%u] %s (Code:%u)") % file_ % line_ % msg_ % code_).str();
		}
};

#define DECLARE_EXCEPTION(name, code) \
	struct name : public Exception \
	{ \
		name(const std::string& msg_, const char* file_, unsigned int line_) \
		: Exception(#name " " + msg_, (code), file_, line_) {}; \
	}

DECLARE_EXCEPTION(NumberOfArgsException, CODE_NUMBER_OF_ARGS);
DECLARE_EXCEPTION(InvalidOperatorException, CODE_INVALID_OPERATION);
DECLARE_EXCEPTION(InvalidStorageAccessException, CODE_INVALID_STORAGE_ACCESS);
DECLARE_EXCEPTION(InvalidNumberFormatException, CODE_INVALID_NUMBER_FORMAT);
DECLARE_EXCEPTION(AddressOutOfRangeException, CODE_ADDRESS_OUT_OF_RANGE);
DECLARE_EXCEPTION(FileOpenFailureException, CODE_FILE_OPEN_FAILURE);
DECLARE_EXCEPTION(InvalidInstructionException, CODE_INVALID_INSTRUCTION);
DECLARE_EXCEPTION(NotImplementedException, CODE_NOT_IMPLEMENTED);
DECLARE_EXCEPTION(InvalidStorageOperationException, CODE_INVALID_STORAGE_OPERATION);

#undef DECLARE_EXCEPTION

#define PICTUREPERFECT_THROW(type, msg) throw ::PicturePerfect:: type ((msg), __FUNCTION__, __LINE__)

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__EXCEPTION__H__
