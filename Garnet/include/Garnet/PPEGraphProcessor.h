#ifndef GARNET__PPE_GRAPH_PROCESSOR__H__
#define GARNET__PPE_GRAPH_PROCESSOR__H__

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <iostream>
#include <sstream>

#include <Common/InfoContainer.h>

#include <Garnet/PPEGraph.h>

#include <PicturePerfectEngine/Executable.h>
#include <PicturePerfectEngine/Archive.h>
#include <PicturePerfectEngine/Engine.h>

namespace Garnet {

#if 0
class PPEGraphProcessor : public InfoContainer
{
public:

	PPEGraphProcessor(const std::string& type = "");
	virtual ~PPEGraphProcessor();

	/**
     *  Execute all executables for each archives.
	 *  (Thus, total execution will be the number of executables * the number of archives)
	 */

	const std::string&  generateScript(PPEGraph& pg);
	const PicturePerfect::ExecutablePtr&  compile(std::istream& script) {
		return (_executable = _engine->compile(script));
	}

	const PicturePerfect::ExecutablePtr&  compile(PPEGraph& pg) {
		return compile(std::stringstream(generateScript(pg)));
	}
	void  execute(PicturePerfect::ExecutablePtr executable, PicturePerfect::ArchivePtr archive) {
		_engine->execute(_executable, archive);
	}
	void  execute(PPEGraph& pg, PicturePerfect::ArchivePtr archive) {
		execute(compile(pg), archive);
	}

private :
	std::string                             _script;
	PicturePerfect::ExecutablePtr           _executable;
	PicturePerfect::PicturePerfectEnginePtr _engine;
};
#endif

}// end of namespace Garnet


#endif//GARNET__PPE_GRAPH_PROCESSOR__H__

