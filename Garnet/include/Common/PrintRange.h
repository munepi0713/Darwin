#ifndef COMMON__PRINT_RANGE__H__
#define COMMON__PRINT_RANGE__H__

#include <iostream>
#include <functional>

#include <boost/format.hpp>

#include <Common/IntTypes.h>

template <typename C>
struct default_putter
{
	void operator()(std::ostream& os, const C& c) const { os << c; }
};
template <typename Iterator, typename Putter = default_putter<typename Iterator::value_type> >
class print_range_proxy
{
public :
	print_range_proxy(Iterator first, Iterator last, const std::string& delim, Putter put = default_putter<typename Iterator::value_type>())
		: _f(first), _l(last), _delim(delim), _put(put) {}
	void  put(std::ostream& os) const {
		bool isFirst = true;
		for (Iterator it = _f; it != _l; it++) {
			if ( !isFirst ) {
				os << _delim;
			}
			_put(os, *it);
			isFirst = false;
		}
	}
private :
	Iterator    _f;
	Iterator    _l;
	std::string _delim;
	Putter      _put;
};

template <typename Iterator>
print_range_proxy<Iterator>  print_range(Iterator first, Iterator last)
{
	return print_range_proxy<Iterator>(first, last, "");
}
template <typename Iterator>
print_range_proxy<Iterator>  print_range(Iterator first, Iterator last, const std::string& delim)
{
	return print_range_proxy<Iterator>(first, last, delim);
}
template <typename Iterator, typename Putter>
print_range_proxy<Iterator, Putter>  print_range(Iterator first, Iterator last, const std::string& delim, Putter putter)
{
	return print_range_proxy<Iterator, Putter>(first, last, delim, putter);
}

template <typename Iterator, typename Putter>
std::ostream&  operator << (std::ostream& os, const print_range_proxy<Iterator, Putter>& p)
{
	p.put(os);
	return os;
}

struct double_hex_putter
{
	void operator()(std::ostream& os, double c) const {
        union {
            double   d;
            uint64_t u;
        } t;
		t.d = c;
		os << boost::format("0x%016x") % t.u;
	}
};

struct double_digit_putter
{
	double_digit_putter(unsigned int digit, unsigned int sub) {
		_format = (boost::format("%%%u.%uf") % digit % sub).str();
	}
	void operator()(std::ostream& os, double c) const {
		os << boost::format(_format) % c;
	}
private :
	std::string _format;
};

template <typename S, typename T>
struct mem_fun_putter
{
	mem_fun_putter(std::mem_fun_ref_t<S, T> func) : _func(func) {}
	void operator()(std::ostream& os, const T& c) const {
		os << _func(c);
	}
private :
	std::mem_fun_ref_t<S, T> _func;
};

template <typename S, typename T>
mem_fun_putter<S, T> make_mem_fun_putter(std::mem_fun_ref_t<S, T> func)
{
	return mem_fun_putter<S, T>(func);
}

template <typename S, typename T>
struct const_mem_fun_putter
{
	const_mem_fun_putter(std::const_mem_fun_ref_t<S, T> func) : _func(func) {}
	void operator()(std::ostream& os, const T& c) const {
		os << _func(c);
	}
private :
	std::const_mem_fun_ref_t<S, T> _func;
};

template <typename S, typename T>
const_mem_fun_putter<S, T> make_mem_fun_putter(std::const_mem_fun_ref_t<S, T> func)
{
	return const_mem_fun_putter<S, T>(func);
}


#endif//COMMON__PRINT_RANGE__H__
