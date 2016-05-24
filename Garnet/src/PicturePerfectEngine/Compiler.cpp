
#include <vector>

#include <Common/Text.h>

#include <PicturePerfectEngine/Compiler.h>

#include <PicturePerfectEngine/SakuraCompiler.h>

PicturePerfect::Compiler::~Compiler()
{
}

PicturePerfect::CompilerPtr  PicturePerfect::Compiler::createInstance(const std::string& type)
{
	const std::vector<std::string> table({
		"Sakura",
	});
	switch (Text::getIndex(table, type)) {
	case 0 :
	default :
		return CompilerPtr(new Sakura::Compiler());
	}
}
