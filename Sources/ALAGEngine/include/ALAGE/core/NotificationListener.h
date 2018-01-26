#ifndef NOTIFICATIONLISTENER_H
#define NOTIFICATIONLISTENER_H

#include "ALAGE/Types.h"

namespace alag
{

class NotificationSender;

class NotificationListener
{
    friend class NotificationSender;

    public:
        NotificationListener();
        virtual ~NotificationListener();

        void StopListeningTo(NotificationSender*);

    protected:
        virtual void Notify(NotificationSender*, NotificationType) = 0;

    private:
        void AddSender(NotificationSender*);
        void NotifySenderDestruction(NotificationSender*);

        std::list<NotificationSender*> m_senders;
};

}

#endif // NOTIFICATIONLISTENER_H
