#ifndef PICTUREPERFECT__VECTOR__H__
#define PICTUREPERFECT__VECTOR__H__

#include <vector>

#include <Common/Statistics.h>

namespace PicturePerfect
{

template <typename T>
class Vector
{
public :
    typedef std::vector<T> Container;
    typedef typename Container::value_type       value_type;
    typedef typename Container::reference        reference;
    typedef typename Container::const_reference  const_reference;
    typedef typename Container::iterator         iterator;
    typedef typename Container::const_iterator   const_iterator;
    typedef typename Container::size_type        size_type;

    Vector() : _dirty(true) {}
    Vector(const Vector& a) : _dirty(true), _data(a._data.begin(), a._data.end()) {}

    iterator        begin() { _dirty = true; return _data.begin(); }
    const_iterator  begin() const { return _data.begin(); }
    iterator        end() { _dirty = true; return _data.end(); }
    const_iterator  end() const { return _data.end(); }
    const_iterator  cbegin() const { return _data.begin(); }
    const_iterator  cend() const { return _data.end(); }
    reference       operator[](size_type index) { _dirty = true; return _data[index]; }
    const_reference operator[](size_type index) const { return _data[index]; }

    size_type       size() const { return _data.size(); }
    void            resize(size_type n, value_type val = value_type()) { _dirty = true; _data.resize(n, val); }
    bool            empty() const { return _data.empty(); }

    void            push_back(const value_type& val) { _dirty = true; _data.push_back(val); }
    void            clear() { _dirty = true; _data.clear(); }

	Vector&         operator=(const Vector& x) { _dirty = true; _data = x._data; return *this; }

	value_type  average() const { refresh(); return _statistics.getAverage(); }
    value_type  stdev() const { refresh(); return _statistics.getStdev(); }
    value_type  max() const { refresh(); return _statistics.getMax(); }
    value_type  min() const { refresh(); return _statistics.getMin(); }
    value_type  sum() const { refresh(); return _statistics.getTotal(); }
    void        refresh() const {
        if ( _dirty ) {
            _statistics = Statistics(); // reset.
			_statistics.put(_data.begin(), _data.end());
            _dirty = false;
        }
    }

private :
    Container           _data;
    mutable bool        _dirty;
    mutable Statistics  _statistics;
};

}//end of namespace PicturePerfect

#endif//PICTUREPERFECT__VECTOR__H__
