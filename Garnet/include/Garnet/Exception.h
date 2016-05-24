#ifndef GARNET__EXCEPTION__H__
#define GARNET__EXCEPTION__H__

#include <exception>

#include <boost/exception.hpp>

class GarnetException : virtual std::exception, virtual boost::exception {};
class GarnetException

#endif//GARNET__EXCEPTION__H__
