#include "ALAGE/core/NotificationSender.h"
#include "ALAGE/Core/NotificationListener.h"

namespace alag
{


NotificationSender::NotificationSender()
{
    //ctor
}

NotificationSender::~NotificationSender()
{
    std::list<NotificationListener*>::iterator listenerIt;
    for(listenerIt = m_listenerToNotifyEverything.begin() ;
        listenerIt != m_listenerToNotifyEverything.end() ; ++listenerIt)
        {
            (*listenerIt)->Notify(this, NotificationSenderDestroyed);
            (*listenerIt)->NotifySenderDestruction(this);
        }

    std::map<NotificationType, std::list<NotificationListener*> >::iterator typeIt;
    for(typeIt = m_listenerToNotify.begin() ; typeIt != m_listenerToNotify.end() ; ++typeIt)
        for(listenerIt = typeIt->second.begin() ; listenerIt != typeIt->second.end() ; ++listenerIt)
        {
            (*listenerIt)->Notify(this, NotificationSenderDestroyed);
            (*listenerIt)->NotifySenderDestruction(this);
        }

}



void NotificationSender::AskForAllNotifications(NotificationListener *listener)
{
    std::list<NotificationListener*>::iterator listenerIt;
    listenerIt = std::find(m_listenerToNotifyEverything.begin(),
                            m_listenerToNotifyEverything.end(), listener);

    if(listenerIt == m_listenerToNotifyEverything.end())
    {
        m_listenerToNotifyEverything.push_back(listener);
        listener->AddSender(this);
    }
}

void NotificationSender::AskForNotification(NotificationListener *listener, NotificationType type)
{
    std::list<NotificationListener*>::iterator listenerIt;
    listenerIt = std::find(m_listenerToNotify[type].begin(),
                            m_listenerToNotify[type].end(), listener);

    if( listenerIt == m_listenerToNotify[type].end())
    {
        m_listenerToNotify[type].push_back(listener);
        listener->AddSender(this);
    }
}

void NotificationSender::RemoveFromNotificationList(NotificationListener *listener, NotificationType type)
{
    m_listenerToNotify[type].remove(listener);
}

void NotificationSender::RemoveFromAllNotificationList(NotificationListener *listener)
{
    m_listenerToNotifyEverything.remove(listener);

    std::map<NotificationType, std::list<NotificationListener*> >::iterator typeIt;
    for(typeIt = m_listenerToNotify.begin() ; typeIt != m_listenerToNotify.end() ; ++typeIt)
        RemoveFromNotificationList(listener, typeIt->first);
}


void NotificationSender::SendNotification(NotificationType type)
{
    std::list<NotificationListener*>::iterator listenerIt;
    for(listenerIt = m_listenerToNotify[type].begin() ;
        listenerIt != m_listenerToNotify[type].end() ; ++listenerIt)
        (*listenerIt)->Notify(this, type);

    for(listenerIt = m_listenerToNotifyEverything.begin() ;
        listenerIt != m_listenerToNotifyEverything.end() ; ++listenerIt)
        (*listenerIt)->Notify(this, type);
}

}
