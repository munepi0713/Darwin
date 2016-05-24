	
// Avoid annoying VC's warning.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <map>

#include <boost/format.hpp>

#include <opencv2/ocl/ocl.hpp>

#include <Common/GetTime.h>
#include <Common/Text.h>
#include <PicturePerfectEngine/Exception.h>
#include <PicturePerfectEngine/SakuraExecutionUnits.h>
#include <PicturePerfectEngine/SakuraEngine.h>

//---------------------------------------------------------
//  Sakura::Engine
//---------------------------------------------------------

Sakura::Engine::Engine(bool enableOpenCL)
{
	// Initialize execution units.
	Sakura::initialize(enableOpenCL);
	_statistics.resize(_unitMap.size());
}

Sakura::Engine::~Engine()
{
}

void Sakura::Engine::execute(const PicturePerfect::ExecutablePtr executable, std::map<uint32_t, PicturePerfect::StoragePtr>& storages, InfoContainer& info)
{
	using namespace PicturePerfect;

	StopWatch watch;
	StoragePtr storage;
	RegisterMap registerMap;

	// 0. Prepare working registers.
	if (!_registers || _registers->getImageSize() != storages.begin()->second->getImageSize()) {
		_registers = Archive::createInstance(storages.begin()->second->getImageSize());
	}

#if 0
	StorageMapper::Blocks blocks;
	blocks.push_back(StorageMapper::Block(ADDR_V1D, _registers, ADDR_V1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_I1D, _registers, ADDR_I1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_I3D, _registers, ADDR_I3D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_I1B, _registers, ADDR_I1B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_I3B, _registers, ADDR_I3B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_S,   _registers, ADDR_S, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_A1D, _registers, ADDR_A1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_L,   _registers, ADDR_L, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SV1D, userStorage, ADDR_SV1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SI1D, userStorage, ADDR_SI1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SI3D, userStorage, ADDR_SI3D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SI1B, userStorage, ADDR_SI1B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SI3B, userStorage, ADDR_SI3B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SS,   userStorage, ADDR_SS, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SA1D, userStorage, ADDR_SA1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_SL,   userStorage, ADDR_SL, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RV1D, userStorage, ADDR_RV1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RI1D, userStorage, ADDR_RI1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RI3D, userStorage, ADDR_RI3D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RI1B, userStorage, ADDR_RI1B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RI3B, userStorage, ADDR_RI3B, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RS,   userStorage, ADDR_RS, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RA1D, userStorage, ADDR_RA1D, ADDR_BLOCK_SIZE));
	blocks.push_back(StorageMapper::Block(ADDR_RL,   userStorage, ADDR_RL, ADDR_BLOCK_SIZE));
	storage = StorageMapper::createInstance(blocks);
#endif

	// 1. Map actual addresses to short cut map.
	const Executable::ShortCutTable& table = executable->getShortCutTable();
	registerMap.assign(table.size(), 0);
	for (size_t i = 0; i < table.size(); i++) {
		switch (getStorageUsageType(table[i])) {
		case USAGE_REG :
			registerMap[i] = _registers->getSlot(table[i]);
			break;
		default :
			registerMap[i] = storages[getStorageUsageMemoryType(table[i])]->getSlot(table[i]);
		}
	}

	// 2. Execute program.
	uint16_t opcode;
	ProgramIterator it;
	try {
		it = executable->getEntryPoint();
		while ( !executable->isEnd(it) ) {
			opcode = *it++;
			if ( opcode == 0 ) break; // "End"

			StopWatch watch;
			it = _unitMap[opcode](it, registerMap);
			_statistics[opcode].put(watch.lap());
#if 0
			// Write register values to files.
			ArchivePtr archive = Archive::createInstance(_register);
			for (size_t i = 0; i < table.size(); i++) {
				uint32_t address = table[i];
				try {
				if ( address != 0 ) {
					switch ( getStorageClass(address) ) {
					case ADDR_I1B :
						archive->saveImage(address, (boost::format("0x%x.png") % address).str());
						break;
					case ADDR_SI3B :
						source->saveImage(address, (boost::format("0x%x.png") % address).str());
						break;
					case ADDR_RI1B :
						result->saveImage(address, (boost::format("0x%x.png") % address).str());
						break;
					case ADDR_RV1D :
						std::cout << boost::format("\n[0x%08x] %f") % address % result->getValue(address);
						break;
					}
				}
				}
				catch( Exception e ) {
				}
			}
#endif
#if 0
			// Register scan.
			for (size_t i = 0; i < table.size(); i++) {
				uint32_t address = table[i];
				if ( address != 0 ) {
					switch( getStorageUsage(address) ) {
					case USAGE_REG :
						_register->validate(address);
						break;
					case USAGE_SRC :
						source->validate(address);
						break;
					case USAGE_RES :
						result->validate(address);
						break;
					case USAGE_CAS :
						cache->validate(address);
						break;
					}
				}
			}
#endif
		}
	}
	catch ( Exception& e ) {
		throw Exception((boost::format("%s (opcode=%u, counter=%u)") % e.what() % opcode % (it - executable->getEntryPoint())).str());
	}

	// 3. Save information.
	watch.stop();
	info.setInformation(INFO_EXECUTION_TIME, watch.get());
	info.setInformation(PicturePerfect::INFO_NUMBER_OF_OPERATORS, (uint32_t)_unitMap.size());
}

uint16_t Sakura::Engine::getOpCode(const std::string& opname) const
{
	std::map<std::string, OpInfo>::const_iterator it = _opcodeMap.find(Text::toLower(opname));
	if ( it != _opcodeMap.end() ) {
		return it->second.opcode;
	}
	else {
		return 0xffffu;
	}
}

uint16_t Sakura::Engine::getMaxOpCode() const
{
	return (uint16_t)(_opcodeMap.size() - 1);
}

const std::string& Sakura::Engine::getOpName(uint16_t opcode) const
{
	std::map<uint16_t, std::string>::const_iterator it = _opcodeMapInv.find(opcode);
	if ( it != _opcodeMapInv.end() ) {
		return it->second;
	}
	else {
		throw (boost::format("[%s] Unknown operator specified. (opcode=%u)") % __FUNCTION__ % opcode).str();
	}
}

uint16_t Sakura::Engine::getNumOperands(uint16_t opcode) const
{
	std::map<uint16_t, std::string>::const_iterator it = _opcodeMapInv.find(opcode);
	if ( it != _opcodeMapInv.end() ) {
		return _opcodeMap.at(it->second).numOperands;
	}
	else {
		throw (boost::format("[%s] Unknown operator specified. (opcode=%u)") % __FUNCTION__ % opcode).str();
	}
}

