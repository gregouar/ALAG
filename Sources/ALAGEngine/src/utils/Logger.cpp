#include "ALAGE/utils/Logger.h"

const char *Logger::DEFAULT_LOG_PATH = "log.txt";

Logger::Logger()
{
    m_consoleActive = true;
    m_enableWarnings = true;
    m_fileStream = new std::ofstream();
    m_fileStream->open(DEFAULT_LOG_PATH, std::ios::out | std::ios::trunc );
}

Logger::~Logger()
{
    m_fileStream->close();
    delete m_fileStream;
}

void Logger::Write(const std::ostringstream& s)
{
    Logger::Write(s.str());
}

void Logger::Warning(const std::ostringstream& s)
{
    Logger::Warning(s.str());
}

void Logger::Error(const std::ostringstream& s)
{
    Logger::Error(s.str());
}

void Logger::FatalError(const std::ostringstream& s)
{
    Logger::FatalError(s.str());
}

void Logger::Write(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cout<<s<<std::endl;

    *Logger::Instance()->m_fileStream<<s<<std::endl;
}

void Logger::Warning(const std::string& s)
{
    if(Logger::Instance()->m_enableWarnings)
    {
        if(Logger::Instance()->m_consoleActive)
            std::cout<<"Warning: "<<s<<std::endl;

        *Logger::Instance()->m_fileStream<<"Warning: "<<s<<std::endl;
    }
}

void Logger::Error(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cerr<<"ERROR: "<<s<<std::endl;

   *Logger::Instance()->m_fileStream<<"ERROR: "<<s<<std::endl;
}

void Logger::FatalError(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cerr<<"FATAL ERROR: "<<s<<", stopping application"<<std::endl;

   *Logger::Instance()->m_fileStream<<"FATAL ERROR: "<<s<<", stopping application"<<std::endl;
}

void Logger::ActivateConsole()
{
    Logger::Instance()->m_consoleActive = true;
}

void Logger::DesactivateConsole()
{
    Logger::Instance()->m_consoleActive = false;
}

