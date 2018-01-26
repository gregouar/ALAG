#include "ALAGE/Core/NotificationListener.h"
#include "ALAGE/core/NotificationSender.h"


namespace alag
{

NotificationListener::NotificationListener()
{
    //ctor
}

NotificationListener::~NotificationListener()
{
    std::list<NotificationSender*>::iterator senderIt;
    for(senderIt = m_senders.begin() ; senderIt != m_senders.end() ; ++senderIt)
        (*senderIt)->RemoveFromAllNotificationList(this);
}

void NotificationListener::AddSender(NotificationSender* sender)
{
    std::list<NotificationSender*>::iterator senderIt;
    senderIt = std::find(m_senders.begin(), m_senders.end(), sender);
    if(senderIt == m_senders.end())
        m_senders.push_back(sender);
}

void NotificationListener::NotifySenderDestruction(NotificationSender* sender)
{
    m_senders.remove(sender);
}

void NotificationListener::StopListeningTo(NotificationSender* sender)
{
    if(sender != nullptr)
    {
        /*std::list<NotificationSender*>::iterator senderIt;
        senderIt = std::find(m_senders.begin(), m_senders.end(), sender);
        if(senderIt != m_senders.end())
            m_senders.erase(senderIt);*/

        m_senders.remove(sender);
        sender->RemoveFromAllNotificationList(this);
    }
}

}
