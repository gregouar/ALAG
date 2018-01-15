#include "Logger.h"

const char *Logger::DEFAULT_LOG_PATH = "log.txt";

Logger::Logger()
{
    m_consoleActive = true;
    m_fileStream = new std::ofstream();
    m_fileStream->open(DEFAULT_LOG_PATH, std::ios::out | std::ios::trunc );
}

Logger::~Logger()
{
    m_fileStream->close();
    delete m_fileStream;
}

void Logger::Write(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cout<<s<<std::endl;

    *Logger::Instance()->m_fileStream<<s<<std::endl;
}

void Logger::Error(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cout<<"ERROR : "<<s<<std::endl;

   *Logger::Instance()->m_fileStream<<"ERROR : "<<s<<std::endl;
}

void Logger::FatalError(const std::string& s)
{
    if(Logger::Instance()->m_consoleActive)
        std::cout<<"ERROR : "<<s<<std::endl;

   *Logger::Instance()->m_fileStream<<"FATAL ERROR : "<<s<<", stopping application"<<std::endl;
}

void Logger::ActivateConsole()
{
    Logger::Instance()->m_consoleActive = true;
}

void Logger::DesactivateConsole()
{
    Logger::Instance()->m_consoleActive = false;
}

