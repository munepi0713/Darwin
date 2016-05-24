#ifndef GARNET__IV_GRAPH_PROCESSOR_COMMAND__H__
#define GARNET__IV_GRAPH_PROCESSOR_COMMAND__H__

#include <PicturePerfectEngine/Engine.h>

#include <Garnet/IVGraphProcessor.h>

namespace Garnet {

class IVGraphProcessorCommand {
public :
    using ArchivePtr = PicturePerfect::ArchivePtr;
    using CompilerPtr = PicturePerfect::CompilerPtr;
    using ExecutablePtr = PicturePerfect::ExecutablePtr;
    using PicturePerfectEnginePtr = PicturePerfect::PicturePerfectEnginePtr;

    IVTreeList          trees;
    AliasDictionary     aliases;
    ConstantDictionary  constants;
    ArchivePtr          source;
    ArchivePtr          result;

    IVGraphProcessor::GroupMethod      groupMethod;
    int                                groupSize;
    IVGraphProcessor::TopoSortMethod   topoSortMethod;

    IVGraph             ivg;
    PPEGraph            pg;
    std::string         script;
    ExecutablePtr       executable;
    InfoContainer       info;

    IVGraphProcessorCommand();
    virtual ~IVGraphProcessorCommand();

    virtual void execute();

private :
    CompilerPtr              _compiler;
    PicturePerfectEnginePtr  _engine;
};

}//end of namespace Garnet

#endif//GARNET__IV_GRAPH_PROCESSOR_COMMAND__H__
