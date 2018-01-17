#include "Parser.h"

#include <sstream>

Parser::Parser()
{
    //ctor
}

Parser::~Parser()
{
    //dtor
}

bool Parser::IsBool(const std::string &data)
{
    bool value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool Parser::IsInt(const std::string &data)
{
    int value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool Parser::IsFloat(const std::string &data)
{
    float value = false;
    std::istringstream ss(data);
    ss>>value;
    return ss.eof();
}

bool  Parser::ParseBool(const std::string& data)
{
    bool value = false;
    std::istringstream ss(data);
    ss>>value;
    if(!ss.eof())
    {
        if(data == "true")
            return (true);
        else
            return (false);
    }
    return value;
}

int   Parser::ParseInt(const std::string& data)
{
    int value = 0;
    std::istringstream ss(data);
    ss>>value;
    return value;
}

float Parser::ParseFloat(const std::string& data)
{
    float value = 0;
    std::istringstream ss(data);
    ss>>value;
    return value;
}
