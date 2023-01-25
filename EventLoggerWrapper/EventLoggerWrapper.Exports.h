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

    extern "C"
    {
        //EVENTLOGGERWRAPPER_API void __stdcall init(std::function<void()> messageReceived);
        EVENTLOGGERWRAPPER_API void __stdcall init(const char* AmsNetId);
        EVENTLOGGERWRAPPER_API void __stdcall registerMessageSent(std::function<void(Wrapper_TcMessage*)> fMessage);
    }
}