#ifndef GARNET__TREE_KERNEL_COLLINS__H__
#define GARNET__TREE_KERNEL_COLLINS__H__

#include <vector>
#include <queue>
#include <boost/graph/adjacency_list.hpp>

namespace Garnet {

/**
 *  Tree Kernel by Collins[1, 2].
 *
 *  This function returns larger value if tree a and b are similar.
 *  Absolute value depends on the size of trees. Tree is larger, the value will be larger.
 *
 *  [1] Collins, M. and Duffy, N.: Convolution Kernels for Natural Language, 
 *      in Advances in Neural Information Processing Systems 14, Cambridge,
 *      MA (2002), MIT Press.
 *  [2] Kashima, H., Sakamoto, H. and Koyanagi, T.: Design and Analysis of 
 *      Convolution Kernels for Tree-Structured Data, in Transactions of the
 *      Japanese Society for Artificial Intelligence, 2006
 *
 *  Note: Confirmed by Hand.
 */
template <typename G>
struct CollinsKernel
{
    static const uint32_t ncalc = 0xffffffffu;
    
    typedef typename G::vertex_descriptor V;
    
	std::vector<uint32_t>  _work;  // ncalc:not yet. 0-:score has been calculated.
    uint32_t               _score;
	
	uint32_t  getValue() {
		return _score;
	}
    
    template <typename T>
    uint32_t execute(const T& t1, const T& t2) {
        return execute(t1.getGraph(), t2.getGraph());
    }
	
	#if 1
	uint32_t _N2; // Temporary value. Only valid inside execute().
	const G* _t1;
	const G* _t2;

	// Recursive with early non-match judgement and a little tunings.
	uint32_t execute(const G& t1, const G& t2) {
		// Save variables for frequent use.
		_t1 = &t1;
		_t2 = &t2;
		_N2 = boost::num_vertices(t2);
		
		auto v1range = boost::vertices(t1);
		auto v2range = boost::vertices(t2);
		
		// 1. Initialize
		std::vector<std::pair<V, V> > q;
		_work.resize(boost::num_vertices(t1) * _N2);
		for (auto v1 = v1range.first; v1 != v1range.second; v1++) {
    		for (auto v2 = v2range.first; v2 != v2range.second; v2++) {
				_work[*v1 * _N2 + *v2] = 
					Text::equals(t1[*v1].label, t2[*v2].label) ?  
						((boost::out_degree(*v1, t1) == 0) || (boost::out_degree(*v2, t2) == 0)) ? 1 : ncalc
					: 0;
			}
		}
		
		// 2. Calculation
        _score = 0;
		for (auto v1 = v1range.first; v1 != v1range.second; v1++) {
    		for (auto v2 = v2range.first; v2 != v2range.second; v2++) {
				uint32_t score = _work[*v1 * _N2 + *v2];
                if ( score == ncalc ) {
					_score += evaluate(*v1, *v2);
				}
				else {
					_score += score;
				}
			}
		}
        return _score;
	}
	
	// Calculate (i,j) score and return it.
	uint32_t evaluate(V v1, V v2) {
		uint32_t score = _work[v1 * _N2 + v2];
        
        if ( score == ncalc ) {
            // Label matching : No match cases are judged at initialization.
			score = 1;
		
			// Subtree matching.
			auto e1range = boost::out_edges(v1, *_t1);
			auto e2range = boost::out_edges(v2, *_t2);
			for (auto e1 = e1range.first, e2 = e2range.first; e1 != e1range.second && e2 != e2range.second; e1++, e2++) {
				auto u1 = boost::target(*e1, *_t1);
				auto u2 = boost::target(*e2, *_t2);
				score *= evaluate(u1, u2) + 1;
			}
            
            _work[v1 * _N2 + v2] = score;
    		//std::cerr << boost::format("(%u, %u)=%u") % v1 % v2 % score << std::endl;
        }

        return score;
	}
	#else
	// Trivial recursive version.
	uint32_t execute(const G& t1, const G& t2) {
		// 1. Initialize
		_work.clear();
		for (auto v1range = boost::vertices(t1); v1range.first != v1range.second; v1range.first++) {
    		for (auto v2range = boost::vertices(t2); v2range.first != v2range.second; v2range.first++) {
                _work.push_back(ncalc);
			}
		}
		
		// 2. Calculation
        _score = 0;
		for (auto v1range = boost::vertices(t1); v1range.first != v1range.second; v1range.first++) {
    		for (auto v2range = boost::vertices(t2); v2range.first != v2range.second; v2range.first++) {
                _score += evaluate(t1, t2, *v1range.first, *v2range.first);
			}
		}
        return _score;
	}
	
	// Calculate (i,j) score and return it.
	uint32_t evaluate(const G& t1, const G& t2, V v1, V v2)
	{
		uint32_t score = _work[v1 * boost::num_vertices(t2) + v2];
        
        if ( score == ncalc ) {
            // Label matching
            if ( !Text::equals(t1[v1].label, t2[v2].label) ) {
                score = 0;
            }
            else {
                score = 1;
    		
                // Subtree matching.
                auto e1range = boost::out_edges(v1, t1);
                auto e2range = boost::out_edges(v2, t2);
                for (; e1range.first != e1range.second && e2range.first != e2range.second; e1range.first++, e2range.first++) {
                    auto e1 = *e1range.first;
                    auto e2 = *e2range.first;
                    
                    score *= (evaluate(t1, t2, boost::target(e1, t1), boost::target(e2, t2)) + 1);
                }
            }
            
            _work[v1 * boost::num_vertices(t2) + v2] = score;
    		//std::cerr << boost::format("(%u, %u)=%u") % v1 % v2 % score << std::endl;
        }

        return score;
	}
	#endif
};

}//end of namespace Garnet

#endif//GARNET__TREE_KERNEL_COLLINS__H__
