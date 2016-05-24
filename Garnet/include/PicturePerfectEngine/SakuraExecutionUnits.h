#ifndef PICTURE_PERFECT_OPEN__SAKURA_EXECUTION_UNITS__H__
#define PICTURE_PERFECT_OPEN__SAKURA_EXECUTION_UNITS__H__

#include <stdint.h>

#include <PicturePerfectEngine/Engine.h>

namespace Sakura
{

typedef std::vector<PicturePerfect::DataSlot*> RegisterMap;

struct ExecutionUnit
{
	virtual const char* getName() const = 0;
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs) = 0;
};

struct InstructionIssue
{
	InstructionIssue();
	InstructionIssue(Sakura::ExecutionUnit& unit, uint32_t index)
		: _unit(&unit), _index(index) {}
	InstructionIssue(const InstructionIssue& rhs)
		: _unit(rhs._unit), _index(rhs._index) {}
	PicturePerfect::ProgramIterator operator()(PicturePerfect::ProgramIterator program, Sakura::RegisterMap& regs) {
		return (*_unit)(_index, program, regs);
	}
private:
	Sakura::ExecutionUnit* _unit;
	uint32_t               _index;
};

struct OpInfo
{
	uint16_t  opcode;
	uint16_t  numOperands;

	OpInfo() {}
	OpInfo(uint16_t a, uint16_t b) : opcode(a), numOperands(b) {}
	OpInfo(const OpInfo& a) : opcode(a.opcode), numOperands(a.numOperands) {}
};

struct NI : public ExecutionUnit
{
	virtual const char* getName() const { return "NI"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t, PicturePerfect::ProgramIterator program, RegisterMap&) {
		PICTUREPERFECT_THROW(NotImplementedException, "NI is not implemented.");
	}
};

struct MU : public ExecutionUnit
{
	virtual const char* getName() const { return "MU"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs);
};

struct VF : public ExecutionUnit
{
	virtual const char* getName() const { return "VF"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs);
};

struct IF : public ExecutionUnit
{
	virtual const char* getName() const { return "IF"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs);
};

struct AF : public ExecutionUnit
{
	virtual const char* getName() const { return "AF"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs);
};

struct IFg : public ExecutionUnit
{
	virtual const char* getName() const { return "IFg"; }
	virtual PicturePerfect::ProgramIterator operator()(uint32_t index, PicturePerfect::ProgramIterator program, RegisterMap& regs);
};

extern std::vector<InstructionIssue> _unitMap;
extern std::map<std::string, OpInfo> _opcodeMap;
extern std::map<uint16_t, std::string> _opcodeMapInv;
extern std::set<std::string> _voidAliases; // Aliases with no return value.

void  initialize(bool enableOpenCL);

}//end of namespace Sakura

#endif//PICTURE_PERFECT_OPEN__SAKURA_EXECUTION_UNITS__H__
