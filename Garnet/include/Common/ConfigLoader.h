/*********************************************************
 *  Garnet
 *  Author: Munehiro Doi.
 *********************************************************/
#ifndef GARNET__CONFIG_LOADER__H__
#define GARNET__CONFIG_LOADER__H__


#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <boost/format.hpp>

#include "Parse.h"

#include <Common/Text.h>

#include <Common/GetTime.h>

/**
 *
 *
 */
class ConfigLoader
{
public :
	/**
	 *  Exception
	 */
	struct NumArgsException
	{
		static void matchOrThrow(size_t spec, size_t actual, const std::string& msg) {
			if ( spec  != actual ) {
				std::string::size_type pos;
				std::string temp = msg;
				while ( (pos = temp.find("$1")) != std::string::npos ) temp = temp.replace(pos, 2, (boost::format("%u") % spec).str());
				while ( (pos = temp.find("$2")) != std::string::npos ) temp = temp.replace(pos, 2, (boost::format("%u") % actual).str());
				throw temp;
			}
		}
		static void greaterEqOrThrow(size_t spec, size_t actual, const std::string& msg) {
			if ( spec > actual ) {
				std::string::size_type pos;
				std::string temp = msg;
				while ( (pos = temp.find("$1")) != std::string::npos ) temp = temp.replace(pos, 2, (boost::format("%u") % spec).str());
				while ( (pos = temp.find("$2")) != std::string::npos ) temp = temp.replace(pos, 2, (boost::format("%u") % actual).str());
				throw temp;
			}
		}
	};

	//----------------------------------------------
	//  Value loader
	//----------------------------------------------
	class Value
	{
	public :
		virtual void load(const strings& tokens) = 0;
	};
	
	class Boolean : public Value
	{
        typedef bool T;
		bool  _value;
	public :
		Boolean() : _value(0) {}
		Boolean(T value) : _value(value) {}
		Boolean(const Boolean& x) : _value(x._value) {}
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
#if 1
			load(tokens[1]);
#else
			if ( tokens[1].compare("True") == 0 || tokens[1].compare("true") == 0 || tokens[1].compare("TRUE") == 0 || tokens[1].compare("1") == 0 ) {
				_value = true;
			}
			else {
				_value = false;
			}
#endif
		}
		virtual void load(const std::string& token)
		{
			if ( token.compare("True") == 0 || token.compare("true") == 0 || token.compare("TRUE") == 0 || token.compare("1") == 0 ) {
				_value = true;
			}
			else {
				_value = false;
			}
		}
        operator T() const
		{
			return _value;
		}
        const T& operator = (const T& value)
        {
            return (_value = value);
        }
        friend std::ostream& operator << (std::ostream& os, const Boolean& b)
        {
            os << (b._value ? "True" : "False");
            return os;
        }
	};
	
	class Integer : public Value
	{
        typedef int T;
		int  _value;
	public :
		Integer() : _value(0) {}
		Integer(T value) : _value(value) {}
		Integer(const Integer& x) : _value(x._value) {}
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
#if 1
			load(tokens[1]);
#else
			_value = strtol(tokens[1].c_str(), 0, 0);
#endif
		}
		virtual void load(const std::string& token)
		{
			_value = strtol(token.c_str(), 0, 0);
		}
		operator int() const
		{
			return _value;
		}
        const int& operator = (const int& value)
        {
            return (_value = value);
        }
	};
		
	class Long : public Value
	{
        typedef int64_t T;
		T  _value;
	public :
		Long() : _value(0) {}
		Long(T value) : _value(value) {}
		Long(const Long& x) : _value(x._value) {}
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
#if 1
			load(tokens[1]);
#else
			_value = strtol(tokens[1].c_str(), 0, 0);
#endif
		}
		virtual void load(const std::string& token)
		{
			_value = strtol(token.c_str(), 0, 0);
		}
		operator T() const
		{
			return _value;
		}
        const T& operator = (const T& value)
        {
            return (_value = value);
        }
	};
		
	class Double : public Value
	{
        typedef double T;
		T  _value;
	public :
		Double() : _value(0) {}
		Double(T value) : _value(value) {}
		Double(const Double& x) : _value(x._value) {}
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
#if 1
			load(tokens[1]);
#else
			_value = strtod(tokens[1].c_str(), 0);
#endif
		}
		virtual void load(const std::string& token)
		{
			_value = strtod(token.c_str(), 0);
		}
		operator T() const
		{
			return _value;
		}
        const T& operator = (const T& value)
        {
            return (_value = value);
        }
	};

	class String : public Value, public std::string
	{
        typedef std::string T;
	public :
		explicit String() : std::string() {}
		String(const String& x) : std::string(x) {}
		String(const T& value) : std::string(value) {}
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
#if 1
			load(tokens[1]);
#else
            this->assign(tokens[1]);
#endif
		}
		virtual void load(const std::string& token) {
            this->assign(token);
		}
        const T& operator = (const T& value)
        {
            return std::string::operator=(value);
        }
	};
	
	/**
	 *  Multiple strings defined in a single line.
	 */
	class StringTuple : public Value, public std::vector<std::string>
	{
	public :
		virtual void load(const strings& tokens)
		{
			NumArgsException::greaterEqOrThrow(2, tokens.size(), tokens[0] + " command must have equals to or more than $1 argument, but $2 args specified.");
			auto it = tokens.begin();
			it++;
			clear();
            insert(this->end(), it, tokens.end());
		}
	};
	
	/**
	 *  Multiple strings defined in multiple lines.
	 */
	class StringList : public Value, public std::vector<std::string>
	{
	public :
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
			push_back(tokens[1]);
		}
	};
	
	class StringSet : public Value, public std::set<std::string>
	{
	public :
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(2, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
			insert(tokens[1]);
		}
	};
	
	class StringMap : public Value, public std::map<std::string, std::string>
	{
	public :
		virtual void load(const strings& tokens)
		{
			NumArgsException::matchOrThrow(3, tokens.size(), tokens[0] + " command must have $1 argument, but $2 args specified.");
			insert(std::make_pair(tokens[1], tokens[2]));
		}
	};
	
	/**
	 *  Multiple values defined in a single line.
	 */
	template <typename T>
	class Array : public Value, public std::vector<T>
	{
	public :
		virtual void load(const strings& tokens)
		{
			NumArgsException::greaterEqOrThrow(2, tokens.size(), tokens[0] + " command must have equals to or more than $1 argument, but $2 args specified.");
			clear();
			for_each(std::next(tokens.begin()), tokens.end(), [&] (const std::string& token) {
				T value;
				value.load(token);
				this->push_back(value);
			});
		}
	};

	//----------------------------------------------
	//  Value loader
	//----------------------------------------------
    ConfigLoader()
    {
        _ignoreUnknownCommand = false;
    }

    virtual ~ConfigLoader()
    {
        clear();
    }

    void  add(const std::string& name, Value& value)
    {
        _values[name] = &value;
    }
    
    void  clear()
    {
    	_values.clear();
    }

    void  ignoreUnknownCommand(bool value) {
        _ignoreUnknownCommand = value;
    }
    
    /**
     *  Load values from a stream.
     *  @param[in] ins
     *  @param[in] data : 
     */
    bool  load(std::istream& ins, void* data) 
    {
        while (!ins.eof()) {
        	// Read a line from the stream.
            std::string line;
            getline(ins, line);
            if (ins.fail()) break;
            
            // Skip if it is an empty line.
			if (line[0] == '#') continue;
			if (line[0] == '\n') continue;
			if (line[0] == '\r') continue;

			// Remove comment.
            std::string::size_type pos = line.find_first_of('#');
            if (pos != std::string::npos) line.erase(pos);// = line.substr(0, pos);

			// Skip if it is a comment line.
            if (line.empty()) continue;

            //std::cerr << line << std::endl;

            // Parse the line to tokens.
            strings tokens = parse(line);
            //fprintf(stderr, "[%s] %u args.\n", tokens[0].c_str(), tokens.size());
            
            // Include an another file.
			if (tokens[0].compare("Include") == 0) {
				std::ifstream fin(tokens[1].c_str());
				if (fin.fail()) {
                    printError(std::string("Invalid Include File : ") + tokens[1]);
					return false;
				}
				load(fin, data);
				fin.close();
			}
			// Load a value specified with the first token.
            else if (_values.find(tokens[0]) != _values.end()) {
            	std::string name = tokens[0];
                _values[name]->load(tokens);
            }
            else if (!_ignoreUnknownCommand) {
                throw (std::string("Invalid Name : ") + tokens[0]);
            }
        }
		return true;
    }

	/**
	 *
	 */
	static void readSections(std::istream& ins, std::map<std::string, std::stringstream>& sections)
	{
		std::string secname = "";
		while ( !ins.eof() && ins.good() ) {
			std::string line;
			std::getline(ins, line);
			line = Text::removeAfter(Text::trimLeft(line), "#");
			if ( line.empty() ) continue;
			
			strings tokens = parse(line);
			if ( Text::equalsWithoutCase(tokens[0], "Begin") ) {
				if ( tokens.size() > 1 ) {
					secname = tokens[1];
				}
			}
			else if ( Text::equalsWithoutCase(tokens[0], "End") ) {
				secname = "";
			}
			else {
				sections[secname] << line << '\n';
			}
		}
	}
private :
    typedef std::map<std::string, Value*>  ValueList;
    ValueList  _values;
    bool  _ignoreUnknownCommand;

    static void  printError(const std::string& msg)
    {
        std::cerr << msg << std::endl;
    }
};

#endif//GARNET__CONFIG_LOADER__H__
