
#ifndef IDBG
#define IDBG

#include <IOString.hpp>
#include <TokenString.hpp>
#include <ConnectionID.hpp>
#include <iIOChannelDevice.hpp>


class iDebugEvent {

    public:
    virtual ~iDebugEvent() {}

};

class CmdLineEvent : public iDebugEvent {

    public:
    enum Type {CmdInProgress, CmdInError, CmdComplete, NullEvent};

    CmdLineEvent()=default;
    CmdLineEvent(imx::IOString cl, Type type, unsigned current_loop_count) {
        CommandLine=cl;
        CurrentLoopCount=current_loop_count;
        Event=type;
    }

    bool operator == (CmdLineEvent event) {

        if ((event.Event==Event) && (event.CommandLine.compare(CommandLine)==0)) return true;
        return(false);
    }


    ~CmdLineEvent() { }

    Type Event {NullEvent};

    imx::IOString CommandLine;
    unsigned CurrentLoopCount {0};
};

class TestExecutionEvent : public iDebugEvent {
    public:
    enum Type { UutTestBegin, UutTestEnd, UutTestGroupBegin, UutTestGroupEnd, NullEvent};

    TestExecutionEvent()=default;
    TestExecutionEvent(TestExecutionEvent::Type type,imx::IOString test, imx::IOString descr, unsigned test_id, unsigned loop_count, unsigned slot) {
        TestName=test;
        TestID=test_id;
        CurrentLoopCount=loop_count;
        Description=descr;
        Event=type;
        SlotID=slot;
    }

    bool operator == (TestExecutionEvent event) {

        if ( (event.Event==Event) && (event.TestID==TestID) && (event.SlotID==SlotID) && (event.TestName.compare(TestName)==0)) return true;
        return(false);
    }

    ~TestExecutionEvent() { }

    unsigned TestID {0};
    unsigned SlotID {0};
    imx::IOString TestName;
    imx::IOString Description;
    unsigned CurrentLoopCount;

    TestExecutionEvent::Type Event {TestExecutionEvent::Type::NullEvent};
};

class DataCollectionEvent : public iDebugEvent {
    public:
    enum Type {DataPass, DataFail, NullEvent};
    DataCollectionEvent()=default;
    DataCollectionEvent(imx::IOString name_space, imx::IOString name, double value, double lo_limit, double hi_limit) {

        NameSpace=name_space;
        Name=name;
        Value=value;
        Lolimit=lo_limit;
        Hilimit=hi_limit;
    }

    bool operator == (DataCollectionEvent event) {

        if ((event.NameSpace.compare(NameSpace)==0) && (event.Name.compare(Name))) return true;
        return(false);
    }

    imx::IOString NameSpace;
    imx::IOString Name;
    double Value {0};
    double Lolimit {0};
    double Hilimit {0};
};

class ExceptionEvent : public iDebugEvent {
    public:
    ExceptionEvent()=default;
    ExceptionEvent(imx::IOString module_name, imx::IOString msg) {
        Message=msg;
        ModuleName=module_name;
    }

    bool operator == (ExceptionEvent event) {

        if ((event.Message.compare(Message)==0) && (event.ModuleName.compare(ModuleName))) return true;
        return(false);
    }

    imx::IOString Message;
    imx::IOString ModuleName;
};

class IOEvent : public iDebugEvent {

    public:
    enum Type {SystemTraceAll, SystemOpenDevice, SystemWriteDevice, SystemReadDevice, SystemCloseDevice, NullEvent};

    IOEvent()=default;
    IOEvent(iIOChannelDevice *device, IOEvent::Type event) {

        Event=event;
        Device=device;
    }

    bool operator == (IOEvent event) {

        if (event.Device==Device) return true;
        return(false);
    }

    IOEvent::Type Event {NullEvent};
    iIOChannelDevice *Device {nullptr};
};

class iDbg {
    public:
    virtual void ProcessEvent(iDebugEvent*) =0;

    virtual void SetBreakPoint (bool set, iDebugEvent *event)=0;
};

class iDebugger {

    public:

    void ProcessEvent(iDebugEvent *event) {

        if (TheDebugger && event) {

            TheDebugger->ProcessEvent(event);
        }
    }

    void SetBreakPoint (bool set, iDebugEvent *event) {

        if (TheDebugger && event) TheDebugger->SetBreakPoint(set,event);
    }

    void Configure(iDbg *dbg) { TheDebugger=dbg; }

    private:
    iDbg *TheDebugger{nullptr};
};


#endif
