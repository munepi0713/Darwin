#include <iostream>
#include <stdint.h>
#include <vector>
//#include <numeric>
#include <iterator>

#include <boost/format.hpp>

#include <Common/Text.h>
#include <Common/GetTime.h>
#include <Garnet/Tree.h>

const uint32_t ncalc = 0xffffffffu;

#if 1
#include <Garnet/TreeKernelCollins.h>
#include <Garnet/TreeKernelKashima.h>
#else
/**
 *  Tree Kernel by Collins 02.
 */
template <typename G>
struct CollinsKernel
{
    
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
                auto score = evaluate(t1, t2, *v1range.first, *v2range.first);
                _score += score;
                //std::cerr << boost::format("%2u ") % score;
			}
            //std::cerr << std::endl;
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
};

/**
 *  Tree Kernel by Kashima06.
 */
template <typename G>
struct KashimaKernel
{
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
		for (auto v1range = boost::vertices(t1); v1range.first != v1range.second; v1range.first++) {
    		for (auto v2range = boost::vertices(t2); v2range.first != v2range.second; v2range.first++) {
                auto score = Kr(t1, t2, *v1range.first, *v2range.first);
                std::cerr << boost::format("%2u ") % score;
			}
            std::cerr << std::endl;
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
#endif

using namespace Garnet;

int main(int argc, const char* argv[])
{
    Tree  t[] = {
        Tree("(A)"),
        Tree("(A(B))"),
        Tree("(A(B)(A))"),
    #if 1
        Tree("(A(B)(C(D)))"),
        Tree("(A(B(E))(C))"),
        Tree("(A(C)(B(D(E))))"),
        Tree("(A(B)(C(G(D))))"),
        Tree("(A(B)(C(G(D))))"),
        Tree("(A(B)(C(A(B)(C))))"),
        Tree("(B(A(B(A(B(A(B(C))))))))"),
        Tree("(B(A(B(A(B(A(B(C)(A(B)(C(G(D)))))))(A(B)(C(G(D))))))))"),
        //Tree("(B(J(L(P(B(E(B(C)(F(B)(C(G(K)))))))(A(I)(C(G(D))))))))"),
    #endif
    };
    
    //uint32_t score;
	const uint32_t N = sizeof(t) / sizeof(t[0]);
    CollinsKernel<Tree::GraphType> ck;
    KashimaKernel<Tree::GraphType> kk;
	std::vector<uint32_t> ckValue(N * N, 0);
	std::vector<uint32_t> kkValue(N * N, 0);
	StopWatch watch;
	watch.reset();
	
	for (auto i = 0u; i < 100000; i++) {
		for (auto i = 0u; i < N; i++) {
			for (auto j = i; j < N; j++) {
				ckValue[N * i + j] = ck.execute(t[i], t[j]);
			}
		}
	}
	std::cout << boost::format("\nCollins %.6fms/call") % watch.lap();
	watch.reset();
	for (auto i = 0u; i < 1000; i++) {
		for (auto i = 0u; i < N; i++) {
			for (auto j = i; j < N; j++) {
				kkValue[N * i + j] = kk.execute(t[i], t[j]);
			}
		}
	}
	std::cout << boost::format("\nKashima %.6fms/call") % watch.lap();

	std::cout << "\n     ";
	for (auto j = 0u; j < N; j++) std::cout << boost::format("[%5u]  ") % j;
	for (auto i = 0u; i < N; i++) {
		std::cout << boost::format("\n[%2u] ") % i;
		for (auto j = 0u; j < N; j++) {
			std::cout << boost::format("%3u/%3u  ") % ckValue[N * i + j] % kkValue[N * i + j];
		}
	}
	std::cout << std::endl;

	return 0;
}
