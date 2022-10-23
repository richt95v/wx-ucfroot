
/*************************************************************************************************************************** 
 
    Overview:	This class implements an interface to the UniConsole framework's console interface.

			    Do not modify.
 
    Author:		Richard Todd
    Date:		12.1.2017

*/

#ifndef IUNICONSOLE
#define IUNICONSOLE

#include <IOString.hpp>

using imx::IOString;

namespace iUniConsole {

    class Console {

        public:

        virtual unsigned CreateConsole(IOString name,unsigned tab_width)=0;
        virtual unsigned RegisterConsole(const char *name, unsigned size)=0;

        virtual void SetProgressBarPosition(int position)=0;

        virtual void SetFocus()=0;

        virtual void UserNotify(bool open, bool error=false, IOString title="",IOString msg="")=0;
        virtual void UserSync(bool open, bool error=false, IOString title="",IOString msg="")=0;
        virtual bool UserInput(bool open, IOString prompt, IOString default_value)=0;

        virtual const char* GetTextInput()=0;
        virtual bool GetButtonSelection()=0;

        virtual void SignalWriteText (unsigned console_id, std::string msg, int type) =0;
    };

}

#endif
