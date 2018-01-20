#include "ALAGE/core/Config.h"

#include <iostream>
#include <fstream>
#include "ALAGE/utils/Logger.h"
#include "ALAGE/utils/Parser.h"

namespace alag{

Config::Config()
{
    //ctor
}

Config::~Config()
{
    //dtor
}


bool Config::Load(const std::string& filePath)
{
    std::ifstream file;
    file.open(filePath.c_str(),std::ifstream::in);

    if(!file.is_open())
    {
        Logger::Error("Can't open config file: "+filePath);
        return (false);
    }

    file.close();
    return (true);
}


bool Config::GetBool(const std::string& s, const std::string& n, const std::string& d)
{
    return Parser::ParseBool(Config::GetString(s,n,d));
}

int Config::GetInt(const std::string& section, const std::string& name, const std::string& dflt)
{
    const std::string &data = Config::GetString(section,name,dflt);
    if(Parser::IsInt(data))
        return Parser::ParseInt(data);
    else
        return Parser::ParseInt(dflt);
}

float Config::GetFloat(const std::string& s, const std::string& n, const std::string& d)
{
    return Parser::ParseFloat(Config::GetString(s,n,d));
}

const std::string &Config::GetString(const std::string& s, const std::string& n, const std::string& d)
{
     std::map<std::string, ConfigSection>::iterator  section_it;
        section_it = Config::Instance()->m_sections.find(s);

    if(section_it != Config::Instance()->m_sections.end())
    {
        ConfigSection::iterator option_it = section_it->second.find(n);

        if(option_it != section_it->second.end())
            return option_it->second;
    }

    return d;
}



}
