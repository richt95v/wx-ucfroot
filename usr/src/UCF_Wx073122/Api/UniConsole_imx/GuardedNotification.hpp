
#ifndef GUARDEDNOTIFICATION
#define GUARDEDNOTIFICATION

#include <iUniConsole.hpp>

namespace iUniConsole {

    class GuardedNotification {

        public:
        GuardedNotification(Console *ptr, std::string title, std::string msg);
        ~GuardedNotification();

        private:
        Console *shell {nullptr};
    };
}

#endif
