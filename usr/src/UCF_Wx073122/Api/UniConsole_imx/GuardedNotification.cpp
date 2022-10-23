
#include <GuardedNotification.hpp>
#include <string>

using namespace iUniConsole;

GuardedNotification::GuardedNotification(Console *ptr, std::string title, std::string msg) {

    if (shell=ptr)  shell->UserNotify(true,false,title.c_str(), msg.c_str());
 }

GuardedNotification::~GuardedNotification() {

    if (shell)  shell->UserNotify(false,false,"","");
}

