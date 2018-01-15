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

bool  Parser::ParseBool(const std::string& data)
{
    bool value = false;
    std::istringstream ss(data);
    ss>>value;
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
