#pragma once

#ifdef EVENTLOGGERWRAPPER_EXPORTS
#define EVENTLOGGERWRAPPER_API __declspec(dllexport)
#else
#define EVENTLOGGERWRAPPER_API __declspec(dllimport)
#endif

#include <string>
#include <functional>

namespace EventLoggerWrapper {

    enum Wrapper_SeverityLevel {
        Verbose,
        Info,
        Warning,
        Error,
        Critical
    };

    struct Wrapper_TcMessage {
        unsigned int EventId;
        long long FileTimeRaised;
        Wrapper_SeverityLevel Severity;
        std::string SourceName;
        std::string Message;
    };

    struct Wrapper_TcAlarm {
        unsigned int EventId;
        long long FileTimeRaised;
        std::string SourceName;
        std::string Message;
    };

    extern "C"
    {
        EVENTLOGGERWRAPPER_API void __stdcall init(const char* AmsNetId);
        EVENTLOGGERWRAPPER_API void __stdcall deinit();
        EVENTLOGGERWRAPPER_API void __stdcall registerMessageSent(std::function<void(Wrapper_TcMessage*)> fMessage);
        EVENTLOGGERWRAPPER_API void __stdcall registerAlarmRaised(std::function<void(Wrapper_TcAlarm*)> fAlarmRaised);
        EVENTLOGGERWRAPPER_API void __stdcall registerAlarmCleared(std::function<void(Wrapper_TcAlarm*, bool)> fAlarmCleared);
        EVENTLOGGERWRAPPER_API void __stdcall registerAlarmConfirmed(std::function<void(Wrapper_TcAlarm*, bool)> fAlarmConfirmed);
    }
}