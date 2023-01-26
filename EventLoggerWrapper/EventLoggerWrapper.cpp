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

extern "C" EVENTLOGGERWRAPPER_API void __stdcall EventLoggerWrapper::registerAlarmCleared(std::function<void(Wrapper_TcAlarm*, bool)> fAlarmCleared)
{
    EventLoggerProxy& logger = EventLoggerProxy::getInstance();
    logger.m_fPtr_alarmCleared_user = fAlarmCleared;
}

extern "C" EVENTLOGGERWRAPPER_API void __stdcall EventLoggerWrapper::registerAlarmConfirmed(std::function<void(Wrapper_TcAlarm*, bool)> fAlarmConfirmed)
{
    EventLoggerProxy& logger = EventLoggerProxy::getInstance();
    logger.m_fPtr_alarmConfirmed_user = fAlarmConfirmed;
}

void EventLoggerWrapper::EventLoggerProxy::init(String^ amsNetId)
{
    m_logger = gcnew TcEventLogger();

    m_fPtr_messageSend_instance = std::bind(&EventLoggerProxy::messageSent, this, _1);
    m_fPtr_alarmRaised_instance = std::bind(&EventLoggerProxy::alarmRaised, this, _1);
    m_fPtr_alarmCleared_instance = std::bind(&EventLoggerProxy::alarmCleared, this, _1, _2);
    m_fPtr_alarmConfirmed_instance = std::bind(&EventLoggerProxy::alarmConfirmed, this, _1, _2);

    m_handler = gcnew LoggerEventHandler(
                                            &m_fPtr_messageSend_instance,
                                            &m_fPtr_alarmRaised_instance,
                                            &m_fPtr_alarmCleared_instance,
                                            &m_fPtr_alarmConfirmed_instance
                                        );

    m_logger->MessageSent += gcnew _ITcEventLoggerEvents_MessageSentEventHandler(m_handler, &LoggerEventHandler::MessageSent);
    m_logger->AlarmRaised += gcnew _ITcEventLoggerEvents_AlarmRaisedEventHandler(m_handler, &LoggerEventHandler::AlarmRaised);
    m_logger->AlarmCleared += gcnew _ITcEventLoggerEvents_AlarmClearedEventHandler(m_handler, &LoggerEventHandler::AlarmCleared);
    m_logger->AlarmConfirmed += gcnew _ITcEventLoggerEvents_AlarmConfirmedEventHandler(m_handler, &LoggerEventHandler::AlarmConfirmed);
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

void EventLoggerWrapper::EventLoggerProxy::alarmCleared(Wrapper_TcAlarm* alarm, bool bRemove)
{
    if (m_fPtr_alarmCleared_user) {
        m_fPtr_alarmCleared_user(alarm, bRemove);
    }
}

void EventLoggerWrapper::EventLoggerProxy::alarmConfirmed(Wrapper_TcAlarm* alarm, bool bRemove)
{
    if (m_fPtr_alarmConfirmed_user) {
        m_fPtr_alarmConfirmed_user(alarm, bRemove);
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

void LoggerEventHandler::AlarmCleared(TcAlarm^ alarm, bool bRemove)
{
    Wrapper_TcAlarm _alarm = {
    alarm->EventId,
    alarm->FileTimeRaised,
    managedStrToNative(alarm->SourceName),
    managedStrToNative(alarm->GetText(CultureInfo::CurrentCulture->LCID))
    };

    (*m_alarmCleared)(&_alarm, bRemove);
}

void LoggerEventHandler::AlarmConfirmed(TcAlarm^ alarm, bool bRemove)
{
    Wrapper_TcAlarm _alarm = {
    alarm->EventId,
    alarm->FileTimeRaised,
    managedStrToNative(alarm->SourceName),
    managedStrToNative(alarm->GetText(CultureInfo::CurrentCulture->LCID))
    };

    (*m_alarmConfirmed)(&_alarm, bRemove);
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
