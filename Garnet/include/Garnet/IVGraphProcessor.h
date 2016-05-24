#ifndef GARNET__IV_GRAPH_PROCESSOR__H__
#define GARNET__IV_GRAPH_PROCESSOR__H__

#include <vector>

#include <Common/InfoContainer.h>

#include <PicturePerfectEngine/Compiler.h>
#include <PicturePerfectEngine/Engine.h>

#include <Garnet/AliasResolver.h>
#include <Garnet/IVGraph.h>
#include <Garnet/IVPairArchive.h>
#include <Garnet/PPEGraph.h>
#include <Garnet/PPEGraphProcessor.h>
#include <Garnet/StatementResolver.h>
#include <Garnet/Tree.h>

namespace Garnet {

class IVGraphInfo
{
public:
	uint32_t             group;
	std::string          igraph;
	std::string          vgraph;
	std::string          agraph;
};

class IVGraphProcessor
{
public:
	static const uint32_t GROUP_METHOD = 0;           // GroupMethod.
	static const uint32_t GROUP_SIZE = 1;             // The number of vertices in uint32_t. 0: no grouping. MAX_UINT: single group.
	static const uint32_t TOPOSORT_METHOD = 2;        // TopoSortMethod.

    static const int INFO_COMPILE_TIME = 1;
    static const int INFO_EXECUTE_TIME = 1;

	enum GroupMethod {
		GROUP_AS_FOREST,
		GROUP_AS_DAG,
		GROUP_DEFAULT = GROUP_AS_DAG,
	};

	enum TopoSortMethod {
		TOPOSORT_BGL,
		TOPOSORT_SARKAR,
		TOPOSORT_DEFAULT = TOPOSORT_SARKAR,
	};

	IVGraphProcessor();
	virtual ~IVGraphProcessor();

    static IVTreeList                    resolveAlias(const IVTreeList& trees, const AliasDictionary& dictionary, InfoContainer& info = InfoContainer());
    static IVTreeList                    resolveStatement(const IVTreeList& trees, const StatementDictionary& dictionary, InfoContainer& info = InfoContainer());
    static IVGraph                       convertIVTreesToIVGraph(const IVTreeList& trees, GroupMethod method, InfoContainer& info = InfoContainer());
    static PPEGraph                      convertIVGraphToPPEGraph(const IVGraph& ivg, InfoContainer& info = InfoContainer());
    static PPEGraph                      removeIntrons(const PPEGraph& pg, InfoContainer& info = InfoContainer());
    static std::string                   generateScript(const PPEGraph& pg, TopoSortMethod method, InfoContainer& info = InfoContainer());
    static PicturePerfect::ExecutablePtr compile(PicturePerfect::CompilerPtr compiler, std::istream& ins, InfoContainer& info = InfoContainer());
    static void                          execute(PicturePerfect::PicturePerfectEnginePtr ppe, const PicturePerfect::ExecutablePtr& executable, const PicturePerfect::ArchivePtr& source, PicturePerfect::ArchivePtr result, InfoContainer& info = InfoContainer());
    static std::vector<IVTreeList>       makeIVTreeGroups(const IVTreeList& trees, int numVerticesLimit, InfoContainer& info = InfoContainer());
};

}// end of namespace Garnet


#endif//GARNET__IV_GRAPH_PROCESSOR__H__

