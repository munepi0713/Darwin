#ifndef GARNET__STATEMENT_RESOLVER__H__
#define GARNET__STATEMENT_RESOLVER__H__

#include <map>
#include <string>

#include <Garnet/Tree.h>

namespace Garnet {

class StatementResolver;

class StatementDictionary {
public:
    void  addConstant(const std::string& name, int value);
    void  clear();

private:
    std::map<std::string, int> _constants;

    friend class StatementResolver;
};

class StatementResolver {
public:
    static Tree  resolve(const Tree& tree, const StatementDictionary& dictionary);
};

}// end of namespace Garnet


#endif//GARNET__STATEMENT_RESOLVER__H__
