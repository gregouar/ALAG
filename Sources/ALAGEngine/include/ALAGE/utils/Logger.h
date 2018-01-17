#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include "ALAGE/utils/Singleton.h"

class Logger : Singleton<Logger>
{
    public:
        friend class Singleton<Logger>;

        static void Write(const std::string&);
        static void Error(const std::string&);
        static void FatalError(const std::string&);

        void ActivateConsole();
        void DesactivateConsole();

    protected:
        Logger();
        virtual ~Logger();

    private:
        std::ofstream *m_fileStream;
        bool m_consoleActive;

        static const char *DEFAULT_LOG_PATH;
};

#endif // LOGGER_H
