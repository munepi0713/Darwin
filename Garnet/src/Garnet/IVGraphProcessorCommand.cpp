#include <Garnet/IVGraphProcessorCommand.h>

namespace Garnet {

IVGraphProcessorCommand::IVGraphProcessorCommand() {
    groupMethod    = IVGraphProcessor::GROUP_DEFAULT;
    groupSize      = 5000;
    topoSortMethod = IVGraphProcessor::TOPOSORT_DEFAULT;

    _compiler = PicturePerfect::Compiler::createInstance();
    _engine = PicturePerfect::PicturePerfectEngine::createInstance();
}
IVGraphProcessorCommand::~IVGraphProcessorCommand() {

}

void IVGraphProcessorCommand::execute() {
    // 0. Copy trees.
    IVTreeList tempTrees = trees;

    // 1.
    tempTrees = IVGraphProcessor::resolveAlias(tempTrees, aliases, info);

    // 2.
    tempTrees = IVGraphProcessor::resolveStatement(tempTrees, constants, info);

    // 3.
    ivg = IVGraphProcessor::convertIVTreesToIVGraph(tempTrees, groupMethod, info);

    // 4.
    pg = IVGraphProcessor::convertIVGraphToPPEGraph(ivg, info);

    // 5.
    pg = IVGraphProcessor::removeIntrons(pg, info);

    // 6.
    script = IVGraphProcessor::generateScript(pg, topoSortMethod, info);

    // 7.
    executable = _compiler->compile(std::stringstream(script), info);

    // 8.
    _engine->execute(executable, source, result, info);
}

}// end of namespace Garnet
