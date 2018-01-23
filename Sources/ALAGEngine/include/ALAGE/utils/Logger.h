#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <sstream>
#include <SFML/System.hpp>
#include "ALAGE/utils/Singleton.h"

class Logger : public Singleton<Logger>
{
    public:
        friend class Singleton<Logger>;

        static void Write(const std::ostringstream&);
        static void Warning(const std::ostringstream&);
        static void Error(const std::ostringstream&);
        static void FatalError(const std::ostringstream&);

        static void Write(const std::string&);
        static void Warning(const std::string&);
        static void Error(const std::string&);
        static void FatalError(const std::string&);

        void ActivateConsole();
        void DesactivateConsole();

    protected:
        Logger();
        virtual ~Logger();

        sf::Mutex m_loggerMutex;

    private:
        std::ofstream *m_fileStream;
        bool m_consoleActive;
        bool m_enableWarnings;

        static const char *DEFAULT_LOG_PATH;
};

#endif // LOGGER_H
