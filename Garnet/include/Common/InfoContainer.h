#ifndef INFO_CONTAINER__H__
#define INFO_CONTAINER__H__

// STL headers
#include <memory>   // shared_ptr
#include <map>      // map

class InfoContainer
{
    struct Item {
    	template <typename T> const T&  at() const { return *static_cast<const T*>(data()); }
    	template <typename T> T&  at() { return *static_cast<T*>(data()); }
    	virtual void* data() = 0;
    	virtual const void* data() const = 0;
    };
    template <typename T>
    struct ItemT : public Item {
    	virtual void* data() { return &_data; }
    	virtual const void* data() const { return &_data; }
    private :
    	T _data;
    };
public :
    typedef std::map<uint32_t, std::shared_ptr<Item> >  ItemList;
    
	// Information
	template <typename T>
	bool getInformation(uint32_t type, T* value) const {
        ItemList::const_iterator it = _info.find(type);
        if ( it != _info.end() ) {
            *value = it->second->at<T>();
            return true;
        }
        else {
            return false;
        }
	}
	template <typename T>
	T getInformation(uint32_t type) const {
		ItemList::const_iterator it = _info.find(type);
		if (it != _info.end()) {
			return it->second->at<T>();
		}
		else {
			throw std::exception("Unknown parameter name is specified to InfoContainer.");
		}
	}
	template <typename T>
	void  setInformation(uint32_t type, const T& value) {
		if ( _info.find(type) == _info.end() ) {
			_info[type] = std::shared_ptr<Item>(new ItemT<T>());
		}
		_info[type]->at<T>() = value;
	}
private :
	ItemList _info;
};

#endif//INFO_CONTAINER__H__
