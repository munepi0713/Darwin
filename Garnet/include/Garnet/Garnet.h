#ifndef GARNET__H__
#define GARNET__H__

#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include <boost/any.hpp>
//#include <boost/filesystem.hpp>

#include <Common/ConfigLoader.h>

#include <PicturePerfectEngine/Archive.h>
#include <PicturePerfectEngine/Engine.h>

#include <Garnet/Configuration.h>
#include <Garnet/Generation.h>
#include <Garnet/Individual.h>
#include <Garnet/IVGraph.h>
#include <Garnet/PPEGraph.h>
#include <Garnet/ThreadPool.h>
#include <Garnet/GPEngine.h>

namespace Garnet
{

const enum {
	INFO_NUMBER_OF_IV_OPERATORS, // uint32_t # of IV operators executed.
	INFO_NUMBER_OF_IV_DATA,      // uint32_t # of IV data allocated.
	INFO_NUMBER_OF_IV_OPERATORS_OF_EVALUATOR, // uint32_t # of IV operators in the evaluator.
	INFO_NUMBER_OF_IV_DATA_OF_EVALUATOR,      // uint32_t # of IV data in the evaluator.
	INFO_NUMBER_OF_IV_OPERATORS_OF_CONDITIONER, // uint32_t # of IV operators in the conditioner.
	INFO_NUMBER_OF_IV_DATA_OF_CONDITIONER,      // uint32_t # of IV data in the conditioner.
};


#if defined(GARNET_ENABLE_OPENCL__)
const bool isOpenCLEnabled = true;
#else
const bool isOpenCLEnabled = false;
#endif

class AnyStore : public std::map<std::string, boost::any>
{
public :
	template <typename T>
	void write(const std::string& module, const std::string& part, const std::string& item, const T& value) {
		write((boost::format("/%s/%s/%s") % module % part % item).str(), value);
	}
	template <typename T>
	void write(const std::string& key, const T& value) {
		(*this)[key] = value;
		std::cerr << key << std::endl;
	}
	template <typename T>
	const T& read(const std::string& key) const {
		return boost::any_cast<const T&>(this->at(key));
	}
};

typedef AnyStore ConfigStore;
typedef AnyStore TraceStore;


/**
 *  Generate Executable from IV trees.
 */
void evaluateFitnessVector(const std::vector<Tree>& trees, std::vector<std::vector<double> >& fitnessVectors, const Garnet::Configuration& conf, Garnet::GPEnginePtr engine = Garnet::GPEnginePtr());

/**
 *
 */
struct ExecTimeEntry
{
	std::string opName;
	uint32_t    unitTime; 
};

/**
 *
 */
void initializeExecTimeTable(const PicturePerfect::PicturePerfectEnginePtr engine, const ExecTimeEntry* first, const ExecTimeEntry* last);

/**
 *
 */
uint32_t getOpExecTime(uint16_t opCode);

#if 0
/**
 *
 */
template <typename It>
std::string resolveAliases(const std::string& src, It first, It last)
{
    std::string ret = src;

	// Repeat until all alises have been resolved.
    for (;;) {
        int replaceCounter = 0;

		for (auto it = first; it != last; it++) {

            std::string::size_type pos = 0;
            while ( (pos = ret.find(it->first, pos)) != std::string::npos ) {
                ret.replace(pos, it->first.size(), it->second);
                replaceCounter++;
            }
		}

		if ( replaceCounter == 0 ) break;
    }

    return ret;
}
#endif

namespace Garnetimpl {
	static bool replace(Garnet::Tree& g, const std::string& label, const Garnet::Tree& newTree) {
		Garnet::Tree gSub;
		bool result = g.replace(label, newTree, gSub);
		if (result) {
			auto subtrees = gSub.getSubTrees(1, 2);
			if (subtrees.size() > 0) while (replace(g, "_1", subtrees[0].first));
			if (subtrees.size() > 1) while (replace(g, "_2", subtrees[1].first));
			if (subtrees.size() > 2) while (replace(g, "_3", subtrees[2].first));
			if (subtrees.size() > 3) while (replace(g, "_4", subtrees[3].first));
			return true;
		}
		else {
			return false;
		}
	};
}

/**
*
*/
template <typename It>
Garnet::Tree& resolveAliases(Garnet::Tree& tree, It first, It last)
{
	// Repeat until all alises have been resolved.
	for (;;) {
		int replaceCounter = 0;

		for (auto it = first; it != last; it++) {
			Garnet::Tree oldSubTree;
			while (Garnetimpl::replace(tree, it->first, it->second)) {
				replaceCounter++;
			}
		}

		if (replaceCounter == 0) break;
	}

	return tree;
}

namespace Trace
{
}

namespace Settings
{
	void load(const Configuration& conf);
	bool isCompressionEnabled();
}//end of namespace Global

}//end of namespace Garnet

#define GTRACE(fmt)         std::cerr << (::boost::format("[%s:%u] " fmt) % __FUNCTION__ % __LINE__) << std::endl
#define GTRACE_F(fmt, expr) std::cerr << (::boost::format("[%s:%u] " fmt) % __FUNCTION__ % __LINE__ % expr) << std::endl

#define GARNET_THROW(type, msg) throw ::Garnet:: type ((msg), __FUNCTION__, __LINE__)


#endif//GARNET__H__

