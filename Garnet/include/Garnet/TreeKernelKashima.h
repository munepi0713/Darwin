#ifndef GARNET__TREE_KERNEL_KASHIMA__H__
#define GARNET__TREE_KERNEL_KASHIMA__H__

#include <vector>
#include <boost/graph/adjacency_list.hpp>

namespace Garnet {

/**
 *  Tree Kernel by Kashima[1, 2].
 *
 *  [1] Kashima, H., Sakamoto, H. and Koyanagi, T.: Design and Analysis of 
 *      Convolution Kernels for Tree-Structured Data, in Transactions of the
 *      Japanese Society for Artificial Intelligence, 2006
 */

/**
 *  Tree Kernel by Kashima06.
 */
template <typename G>
struct KashimaKernel
{

	static const uint32_t ncalc = 0xffffffffu;

    typedef typename G::vertex_descriptor V;
    
	std::vector<uint32_t>  _Kr;      // ncalc:not yet. 0-:score has been calculated.
	std::vector<uint32_t>  _Kr_bar;  // ncalc:not yet. 0-:score has been calculated.
	std::vector<uint32_t>  _Kd;      // ncalc:not yet. 0-:score has been calculated.
    uint32_t               _score;
	
	uint32_t  getValue() {
		return _score;
	}
    
    template <typename T>
    uint32_t execute(const T& t1, const T& t2) {
        return execute(t1.getGraph(), t2.getGraph());
    }
	
	uint32_t execute(const G& t1, const G& t2) {
		// 1. Initialize
        _Kr.assign(boost::num_vertices(t1) * boost::num_vertices(t2), ncalc);
        _Kr_bar.assign(boost::num_vertices(t1) * boost::num_vertices(t2), ncalc);
        _Kd.assign(boost::num_vertices(t1) * boost::num_vertices(t2), ncalc);
		
		// 2. Calculation
        _score = 0;
		for (auto v1range = boost::vertices(t1); v1range.first != v1range.second; v1range.first++) {
    		for (auto v2range = boost::vertices(t2); v2range.first != v2range.second; v2range.first++) {
                auto score = Kr(t1, t2, *v1range.first, *v2range.first);
                _score += score;
			}
		}
                
        return _score;
	}
    
    // Calculate Kr
    uint32_t Kr(const G& t1, const G& t2, V v1, V v2) {
		uint32_t score = _Kr[v1 * boost::num_vertices(t2) + v2];
        
        if ( score == ncalc ) {
            //std::cerr << __LINE__ << std::endl;
            
            // Label matching
            score = Text::equals(t1[v1].label, t2[v2].label) ? 1 : 0;
            
            if ( score > 0 && boost::out_degree(v1, t1) > 0 && boost::out_degree(v2, t2) > 0 ) {
                score = Kr_bar(t1, t2, v1, v2, boost::out_degree(v1, t1), boost::out_degree(v2, t2));
            }
            
            _Kr[v1 * boost::num_vertices(t2) + v2] = score;
    		//std::cerr << boost::format("(%u, %u)=%u") % v1 % v2 % score << std::endl;
        }

        return score;
    }
    
    // Calculate Kr_bar
    uint32_t Kr_bar(const G& t1, const G& t2, V v1, V v2, int i, int j) {
        uint32_t score = _Kr_bar[v1 * boost::num_vertices(t2) + v2];
        if ( score == ncalc ) {
            //std::cerr << __LINE__ << std::endl;
            score = 0;
            
            if ( i > 0 && j > 0 ) {
                auto ch1 = boost::out_edges(v1, t1);
                auto ch2 = boost::out_edges(v2, t2);
                std::advance(ch1.first, i - 1);
                std::advance(ch2.first, j - 1);
                
                uint32_t score;
                score = Kr_bar(t1, t2, v1, v2, i - 1, j)
                      + Kr_bar(t1, t2, v1, v2, i, j - 1)
                      - Kr_bar(t1, t2, v1, v2, i - 1, j - 1)
                      + Kr_bar(t1, t2, v1, v2, i - 1, j - 1)
                      * Kd(t1, t2,
                           boost::target(*ch1.first, t1),
                           boost::target(*ch2.first, t2));
            }
            _Kr_bar[v1 * boost::num_vertices(t2) + v2] = score;
        }
        return score;
    }
    
    // Kd
    uint32_t Kd(const G& t1, const G& t2, V v1, V v2) {
        uint32_t score = _Kd[v1 * boost::num_vertices(t2) + v2];
        if ( score == ncalc ) {
            //std::cerr << __LINE__ << std::endl;
            uint32_t score = 0;
            for (auto i = 1u; i <= boost::out_degree(v1, t1); i++) {
                auto ch1 = boost::out_edges(v1, t1);
                std::advance(ch1.first, i - 1);
                score += Kd(t1, t2,
                    boost::target(*ch1.first, t1),
                    v2);
            }
            for (auto j = 1u; j <= boost::out_degree(v2, t2); j++) {
                auto ch2 = boost::out_edges(v2, t2);
                std::advance(ch2.first, j - 1);
                score += Kd(t1, t2,
                    v1,
                    boost::target(*ch2.first, t2));
            }
            for (auto i = 1u; i <= boost::out_degree(v1, t1); i++) {
                for (auto j = 1u; j <= boost::out_degree(v2, t2); j++) {
                    auto ch1 = boost::out_edges(v1, t1);
                    auto ch2 = boost::out_edges(v2, t2);
                    std::advance(ch1.first, i - 1);
                    std::advance(ch2.first, j - 1);
                    score -= Kd(t1, t2,
                        boost::target(*ch1.first, t1),
                        boost::target(*ch2.first, t2));
                }
            }
            score += Kr(t1, t2, v1, v2);
            _Kd[v1 * boost::num_vertices(t2) + v2] = score;
        }
        return score;
    }
};

}//end of namespace Garnet

#endif//GARNET__TREE_KERNEL_KASHIMA__H__
