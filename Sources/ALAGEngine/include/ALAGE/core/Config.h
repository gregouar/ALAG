#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <map>

namespace alag{

typedef std::map<std::string, std::string> ConfigSection;

class Config
{
    public:
        Config();
        virtual ~Config();

        bool Load(const std::string&);

        bool        GetBool(const std::string& s, const std::string& n, const std::string& d=0);
        int         GetInt(const std::string& s, const std::string& n, const std::string& d=0);
        float       GetFloat(const std::string& s, const std::string& n, const std::string& d=0);
        const std::string& GetString(const std::string& s, const std::string& n, const std::string& d=0);

    protected:

    private:
        std::map<std::string, ConfigSection> m_sections;
};

}

#endif // CONFIG_H
