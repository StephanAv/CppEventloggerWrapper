#include "pch.h"
#include "EventLoggerWrapper.h"
#include "EventLoggerWrapper.Exports.h"

using namespace std::placeholders;
using namespace System;
using namespace System::Globalization;
using namespace EventLoggerWrapper;

std::string managedStrToNative(System::String^ sysstr)
{
    using System::IntPtr;
    using System::Runtime::InteropServices::Marshal;

    IntPtr ip = Marshal::StringToHGlobalAnsi(sysstr);
    std::string outString = static_cast<const char*>(ip.ToPointer());
    Marshal::FreeHGlobal(ip);
    return outString;
}

#pragma warning( push )
#pragma warning( disable : 4715)
Wrapper_SeverityLevel managedSeverityToNative(SeverityLevelEnum lvl)
{
    switch (lvl) {
    case SeverityLevelEnum::Verbose:
        return Wrapper_SeverityLevel::Verbose;
        break;
    case SeverityLevelEnum::Info:
        return Wrapper_SeverityLevel::Info;
        break;
    case SeverityLevelEnum::Warning:
        return Wrapper_SeverityLevel::Warning;
        break;
    case SeverityLevelEnum::Error:
        return Wrapper_SeverityLevel::Error;
        break;
    case SeverityLevelEnum::Critical:
        return Wrapper_SeverityLevel::Critical;
        break;
    }
}
#pragma warning( pop )

extern "C" EVENTLOGGERWRAPPER_API void __stdcall EventLoggerWrapper::init(const char* AmsNetId)
{
    EventLoggerProxy& logger = EventLoggerProxy::getInstance();
    String^ managedString = gcnew String(AmsNetId);
    logger.init(managedString);
}

extern "C" EVENTLOGGERWRAPPER_API void __stdcall EventLoggerWrapper::registerMessageSent(std::function<void(Wrapper_TcMessage*)> fMessage)
{
    EventLoggerProxy& logger = EventLoggerProxy::getInstance();
    logger.m_fPtr_messageSend_user = fMessage;
}

extern "C" EVENTLOGGERWRAPPER_API void __stdcall EventLoggerWrapper::registerAlarmRaised(std::function<void(Wrapper_TcAlarm*)> fAlarmRaised)
{
    EventLoggerProxy& logger = EventLoggerProxy::getInstance();
    logger.m_fPtr_alarmRaised_user = fAlarmRaised;
}



void EventLoggerWrapper::EventLoggerProxy::init(String^ amsNetId)
{
    m_logger = gcnew TcEventLogger();

    m_fPtr_messageSend_instance = std::bind(&EventLoggerProxy::messageSent, this, _1);
    m_fPtr_alarmRaised_instance = std::bind(&EventLoggerProxy::alarmRaised, this, _1);

    // Register Wrapper Callbacks
    m_handler = gcnew LoggerEventHandler(
                                            &m_fPtr_messageSend_instance,
                                            &m_fPtr_alarmRaised_instance
                                        );
    m_logger->MessageSent += gcnew _ITcEventLoggerEvents_MessageSentEventHandler(m_handler, &LoggerEventHandler::MessageSent);
    m_logger->AlarmRaised += gcnew _ITcEventLoggerEvents_AlarmRaisedEventHandler(m_handler, &LoggerEventHandler::AlarmRaised);

    m_logger->Connect(amsNetId);
}

void EventLoggerWrapper::EventLoggerProxy::messageSent(Wrapper_TcMessage *message)
{
    if (m_fPtr_messageSend_user) {
        m_fPtr_messageSend_user(message);
    }
}

void EventLoggerWrapper::EventLoggerProxy::alarmRaised(Wrapper_TcAlarm* alarm)
{
    if (m_fPtr_alarmRaised_user) {
        m_fPtr_alarmRaised_user(alarm);
    }
}

void LoggerEventHandler::AlarmRaised(TcAlarm^ alarm)
{
    Wrapper_TcAlarm _alarm = {
        alarm->EventId,
        alarm->FileTimeRaised,
        managedStrToNative(alarm->SourceName),
        managedStrToNative(alarm->GetText(CultureInfo::CurrentCulture->LCID))
    };

    (*m_alarmRaised)(&_alarm);
}


void LoggerEventHandler::MessageSent(TcMessage ^message)
{
    Wrapper_TcMessage msg = {
        message->EventId,
        message->FileTimeRaised,
        managedSeverityToNative(message->SeverityLevel),
        managedStrToNative(message->SourceName),
        managedStrToNative(message->GetText(CultureInfo::CurrentCulture->LCID))
    };

    (*m_messageSend)(&msg);
}
