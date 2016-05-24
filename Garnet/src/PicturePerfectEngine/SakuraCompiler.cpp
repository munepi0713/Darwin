	
// Avoid annoying VC's warning.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <map>

#include <boost/format.hpp>

#include <opencv2/ocl/ocl.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>
#include <Common/Parse.h>

#include <PicturePerfectEngine/Exception.h>
#include <PicturePerfectEngine/Storage.h>
#include <PicturePerfectEngine/SakuraCompiler.h>
#include <PicturePerfectEngine/SakuraExecutionUnits.h>

//---------------------------------------------------------
//  Sakura::Compiler
//---------------------------------------------------------

Sakura::Compiler::Compiler(bool enableOpenCL)
{
}

Sakura::Compiler::~Compiler()
{
}

PicturePerfect::ExecutablePtr Sakura::Compiler::compile(std::istream& ins, InfoContainer& info)
{
	StopWatch watch;
	PicturePerfect::ExecutablePtr executable = PicturePerfect::Executable::createInstance();
	std::map<uint32_t, uint16_t> shortCutMap; // <address, index>
	uint32_t  numOps = 0;
	std::stringstream script;
	
	PicturePerfect::ProgramWriter writer = executable->getWriter();
	uint16_t opcode = 0xffffu;
	while ( !ins.eof() ) {
		std::string line;
		std::getline(ins, line);
		line = Text::removeAfter(Text::trimLeft(line), "#");
		if ( line.empty() ) continue;

		script << line << "\n";
		
		strings tokens = parse(line);

		std::string inst;

		// 1. Convert arguments to address numbers.
		std::vector<uint32_t> addresses;
		for (size_t i = 1; i < tokens.size(); i++) {
			addresses.push_back(PicturePerfect::Storage::decodeAddress(tokens[i]));
		}

		// 2. Convert instruction to opcode.
		inst = tokens[0];
		std::transform(inst.begin(), inst.end(), inst.begin(), ::tolower);
		auto it = _opcodeMap.find(inst);
		if ( it != _opcodeMap.end() ) {
			// Mnemonic found.
			opcode = it->second.opcode;
		}
		else {
			// Try to find as an alias.
			size_t i = 0;
			if ( _voidAliases.find(inst) == _voidAliases.end() ) {
				inst = std::string(PicturePerfect::Storage::toDataTypeName(addresses[i])) + "_" + inst;
				i++;
			}
			while ( i < addresses.size() ) {
				inst = inst + "_" + PicturePerfect::Storage::toDataTypeName(getStorageDataType(addresses[i]));
				i++;
			}
			it = _opcodeMap.find(inst);
			if ( it != _opcodeMap.end() ) {
				opcode = it->second.opcode;
			}
		}
		if ( opcode == 0xffffu ) {
			PICTUREPERFECT_THROW(InvalidInstructionException, (boost::format("operator: %s. (resolved as %s)") % tokens[0] % inst).str());
		}

		// 3. Convert addresses into shortcut indices.
		for (size_t i = 0; i < addresses.size(); i++) {
			auto it = shortCutMap.find(addresses[i]);
			if ( it == shortCutMap.end() ) {
				shortCutMap.insert(std::make_pair(addresses[i], static_cast<uint16_t>(shortCutMap.size())));
			}
		}

		// 4. Write opcode and operands to program.
		numOps++;
		*writer++ = static_cast<uint16_t>(opcode);
		for (size_t i = 0; i < addresses.size(); i++) {
			*writer++ = shortCutMap[addresses[i]];
		}
		if ( opcode == 0 ) {
			// "End" detected.
			break;
		}
	}
	if ( opcode != 0 ) {
		throw "Script does not end with End instruction.";
	}

	// 5. Save short cut map.
	PicturePerfect::Executable::ShortCutTable& table = executable->getShortCutTable();
	table.resize(shortCutMap.size());
	for (auto it = shortCutMap.begin(); it != shortCutMap.end(); it++) {
		table[it->second] = it->first;
	}

	// 6. Save information.
	watch.stop();
	executable->setInformation(numOps, static_cast<uint32_t>(shortCutMap.size()));
	executable->setSciprt(script.str());
	info.setInformation(PicturePerfect::INFO_COMPILATION_TIME, watch.get());

	// 7. Return program.
	return executable;
}
