#ifndef NOTIFICATIONSENDER_H
#define NOTIFICATIONSENDER_H

#include "ALAGE/Types.h"

namespace alag
{

class NotificationListener;

class NotificationSender
{
    public:
        NotificationSender();
        virtual ~NotificationSender();

        void AskForAllNotifications(NotificationListener *);
        void AskForNotification(NotificationListener *, NotificationType);

        void RemoveFromNotificationList(NotificationListener *, NotificationType);
        void RemoveFromAllNotificationList(NotificationListener *);

    protected:
        void SendNotification(NotificationType);

    private:
        std::list<NotificationListener*> m_listenerToNotifyEverything;
        std::map<NotificationType, std::list<NotificationListener*> > m_listenerToNotify;
};

}

#endif // NOTIFICATIONSENDER_H
