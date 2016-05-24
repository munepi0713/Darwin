#include <iostream>
#include <stdint.h>
#include <vector>

struct V
{
	uint32_t  label;
	uint32_t  _epos; // Edge index in the EList of first child of this v.
	V(uint32_t l, uint32_t e) : label(l), _epos(e) {}
	V(const V& v) : label(v.label), _epos(v._epos) {};
};

struct E
{
	uint32_t  parent;
	uint32_t  child;
	E(uint32_t p, uint32_t c) : parent(p), child(c) {}
	E(const E& e) : parent(e.parent), child(e.child) {};
};

typedef std::vector<V>  VList;
typedef std::vector<E>	EList;

struct Tree
{
	VList  v;
	EList  e;
};

//    A
//    |
//  +-+-+
//  |   |
//  B   C
//      |
//      D
Tree  makeTree1()
{
	Tree  t;
	t.v.push_back(V('A', 0)); // 0
	t.v.push_back(V('B', 3)); // 1
	t.v.push_back(V('C', 2)); // 2
	t.v.push_back(V('D', 3)); // 3
	t.e.push_back(E(0, 1));
	t.e.push_back(E(0, 2));
	t.e.push_back(E(2, 3));
	return t;
}
//    A
//    |
//  +-+-+
//  |   |
//  B   C
//  |
//  E
Tree  makeTree2()
{
	Tree  t;
	t.v.push_back(V('A', 0)); // 0
	t.v.push_back(V('B', 2)); // 1
	t.v.push_back(V('C', 3)); // 2
	t.v.push_back(V('E', 3)); // 3
	t.e.push_back(E(0, 1));
	t.e.push_back(E(0, 2));
	t.e.push_back(E(1, 3));
	return t;
}
//    A
//    |
//  +-+-+
//  |   |
//  C   B
//      |
//      D
//      |
//      E
Tree  makeTree3()
{
	Tree  t;
	t.v.push_back(V('A', 0)); // 0
	t.v.push_back(V('C', 4)); // 1
	t.v.push_back(V('B', 2)); // 2
	t.v.push_back(V('D', 4)); // 3
	t.v.push_back(V('E', 4)); // 4
	t.e.push_back(E(0, 1));
	t.e.push_back(E(0, 2));
	t.e.push_back(E(2, 3));
	t.e.push_back(E(3, 4));
	return t;
}

//    A
//    |
//  +-+-+
//  |   |
//  B   C
//      |
//      G
//      |
//      D
Tree  makeTree4()
{
	Tree  t;
	t.v.push_back(V('A', 0)); // 0
	t.v.push_back(V('B', 4)); // 1
	t.v.push_back(V('C', 2)); // 2
	t.v.push_back(V('G', 3)); // 3
	t.v.push_back(V('D', 4)); // 4
	t.e.push_back(E(0, 1));
	t.e.push_back(E(0, 2));
	t.e.push_back(E(2, 3));
	t.e.push_back(E(3, 4));
	return t;
}

struct Work
{
	uint32_t  v1;
	uint32_t  v2;
	bool     done;
	uint32_t  score;
};

struct TreeMatcher
{
	std::vector<Work>  _work;
    uint32_t           _score;
	
	uint32_t  getValue() {
		return _score;
	}
	
	void execute(const Tree& t1, const Tree& t2) {
		_work.clear();
		// 1. Initialize
		for (size_t i = 0; i < t1.v.size(); i++) {
			for (size_t j = 0; j < t2.v.size(); j++) {
				Work w;
				w.v1 = i;
				w.v2 = j;
				w.done = false;
				w.score = 0;
				_work.push_back(w);
			}
		}
		
		// 2. Calculation
        _score = 0;
        for (size_t i = 0; i < _work.size(); i++) {
		    if ( !_work[i].done ) {
                evaluate(t1, t2, _work[i]);
            }
            _score += _work[i].score;
        }
	}
	
	void evaluate(const Tree& t1, const Tree& t2, Work& w)
	{
		const V& v1 = t1.v[w.v1];
		const V& v2 = t2.v[w.v2];
		
		uint32_t score = v1.label == v2.label ? 1 : 0;
		
		// if any of v1 or v2 is a leaf.
		if ( v1._epos != t1.v.size() && v2._epos != t2.v.size() ) {
			// for all children
			std::vector<Work>::iterator it = _work.begin();
			for (size_t i = v1._epos; t1.e[i].parent == w.v1 && i < t1.v.size(); i++) {
				while ( it->v1 != t1.e[i].child ) it++;
				for (size_t j = v2._epos; t2.e[j].parent == w.v2 && j < t2.v.size(); j++) {
					while ( it->v2 != t2.e[j].child ) it++;
					if ( !it->done ) evaluate(t1, t2, *it);
					score *= (it->score + 1);
				}
			}
		}
		w.score = score;
		w.done  = true;
		std::cerr << w.v1 << ", " << w.v2 << " " << score << std::endl;
	}
};

int main(int argc, const char* argv[])
{
	Tree  t1 = makeTree1();
	Tree  t2 = makeTree2();
	Tree  t3 = makeTree3();
	Tree  t4 = makeTree4();

	TreeMatcher  m;
	m.execute(t1, t2);
	std::cout << m.getValue() << std::endl;
	m.execute(t1, t3);
	std::cout << m.getValue() << std::endl;
	m.execute(t1, t4);
	std::cout << m.getValue() << std::endl;
	m.execute(t2, t3);
	std::cout << m.getValue() << std::endl;
	m.execute(t1, t1);
	std::cout << m.getValue() << std::endl;
	m.execute(t3, t3);
	std::cout << m.getValue() << std::endl;

	return 0;
}
