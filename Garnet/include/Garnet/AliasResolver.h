#ifndef GARNET__ALIAS_RESOLVER__H__
#define GARNET__ALIAS_RESOLVER__H__

#include <map>
#include <string>

#include <Garnet/Tree.h>

namespace Garnet {

class AliasResolver;

class AliasDictionary {
public:
    void  addAlias(const std::string& alias, const Tree& entity);
    void  clear();

private:
    std::map<std::string, Garnet::Tree> _aliases;

    friend class AliasResolver;
};

class AliasResolver {
public :
    static Tree  resolve(const Tree& tree, const AliasDictionary& dictionary);
};

}// end of namespace Garnet


#endif//GARNET__ALIAS_RESOLVER__H__
