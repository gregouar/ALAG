#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <map>
#include "ALAGE/utils/singleton.h"

namespace alag{

typedef std::map<std::string, std::string> ConfigSection;

class Config : public Singleton<Config>
{

    public:
        friend class Singleton<Config>;

        bool Load(const std::string&);

        static bool                 GetBool(const std::string& s, const std::string& n, const std::string& d=0);
        static int                  GetInt(const std::string& s, const std::string& n, const std::string& d=0);
        static float                GetFloat(const std::string& s, const std::string& n, const std::string& d=0);
        static const std::string&   GetString(const std::string& s, const std::string& n, const std::string& d=0);

       /* bool        GetBool(const std::string& s, const std::string& n, const std::string& d=0);
        int         GetInt(const std::string& s, const std::string& n, const std::string& d=0);
        float       GetFloat(const std::string& s, const std::string& n, const std::string& d=0);
        const std::string& GetString(const std::string& s, const std::string& n, const std::string& d=0);*/

    protected:
        Config();
        virtual ~Config();

    private:
        std::map<std::string, ConfigSection> m_sections;
};

}

#endif // CONFIG_H
