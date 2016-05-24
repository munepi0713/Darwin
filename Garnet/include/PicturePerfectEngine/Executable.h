#ifndef PICTURE_PERFECT_ENGINE__EXECUTABLE__H__
#define PICTURE_PERFECT_ENGINE__EXECUTABLE__H__

#include <memory>

#include <Common/Statistics.h>
#include <Common/InfoContainer.h>

#include <PicturePerfectEngine/Program.h>

namespace PicturePerfect
{

const enum {
	INFO_IMPLEMENTATION_NAME, // std::string
	INFO_COMPILATION_TIME,    // double, [sec]
	INFO_EXECUTION_TIME,      // double, [sec]
	INFO_NUMBER_OF_OPERATORS, // uint32_t. # of operators.
	INFO_NUMBER_OF_DATA,      // uint32_t. # of data in all storages.
};

class Executable;
typedef std::shared_ptr<Executable> ExecutablePtr;

class Executable
{
public :
	typedef std::vector<uint32_t> ShortCutTable;

	ProgramIterator getEntryPoint() const {
		return _program.begin();
	}
	ProgramIterator begin() const {
		return _program.begin();
	}
	ProgramIterator end() const {
		return _program.end();
	}
	bool isEnd(const ProgramIterator& it) const {
		return it == _program.end();
	}
	ShortCutTable& getShortCutTable() {
		return _table;
	}
	const ShortCutTable& getShortCutTable() const {
		return _table;
	}
	const std::vector<uint32_t>& getOccurenceList() const {
		return _occurence;
	}
	ProgramWriter getWriter() {
		return std::back_inserter<Program>(_program);
	}
	uint32_t getNumberOfOperators() const {
		uint32_t value;
		_info.getInformation(INFO_NUMBER_OF_OPERATORS, &value);
		return value;
	}
	uint32_t getNumberOfData() const {
		uint32_t value;
		_info.getInformation(INFO_NUMBER_OF_DATA, &value);
		return value;
	}
	const std::string& getScript() const {
		return _script;
	}
	void setInformation(uint32_t numOps, uint32_t numData) {
		_info.setInformation(INFO_NUMBER_OF_OPERATORS, numOps);
		_info.setInformation(INFO_NUMBER_OF_DATA, numData);
	}
	void setSciprt(const std::string& script) {
		_script = script;
	}

	static ExecutablePtr createInstance() {
		return ExecutablePtr(new Executable());
	}
private :
	InfoContainer          _info;
	Program                _program;
	ShortCutTable          _table;
	std::vector<uint32_t>  _occurence; // Op occurence.
	std::string            _script;

	Executable() {}
};

}//end of namespace PicturePerfect

#endif//PICTURE_PERFECT_ENGINE__EXECUTABLE__H__
