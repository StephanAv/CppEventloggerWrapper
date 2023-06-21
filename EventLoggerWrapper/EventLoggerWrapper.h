#pragma once
#include <vcclr.h>
#include <functional>
#include "EventLoggerWrapper.Exports.h"

using namespace System;
using namespace TcEventLoggerAdsProxyLib;

namespace EventLoggerWrapper {

	// Managed Type
	ref class LoggerEventHandler
	{
	public:
		LoggerEventHandler(
			std::function<void(Wrapper_TcMessage*)>* messageSend,
			std::function<void(Wrapper_TcAlarm*)>* alarmRaised,
			std::function<void(Wrapper_TcAlarm*, bool)>* alarmCleared,
			std::function<void(Wrapper_TcAlarm*, bool)>* alarmConfirmed
			)
			: m_messageSend(messageSend)
			, m_alarmRaised(alarmRaised)
			, m_alarmCleared(alarmCleared)
			, m_alarmConfirmed(alarmConfirmed)
			{	};

		void MessageSent(TcMessage^ message);
		void AlarmRaised(TcAlarm^ alarm);
		void AlarmCleared(TcAlarm^ alarm, bool bRemove);
		void AlarmConfirmed(TcAlarm ^alarm, bool bRemove);
		
	private:
		std::function<void(Wrapper_TcMessage*)> *m_messageSend;
		std::function<void(Wrapper_TcAlarm*)>* m_alarmRaised;
		std::function<void(Wrapper_TcAlarm*, bool)>* m_alarmCleared;
		std::function<void(Wrapper_TcAlarm*, bool)>* m_alarmConfirmed;
	};


	// Unmanaged Type
	class EventLoggerProxy
	{
	public:
		static EventLoggerProxy& getInstance()
		{
			static EventLoggerProxy instance;
			return instance;
		}

		EventLoggerProxy(EventLoggerProxy const&) = delete;
		void operator=(EventLoggerProxy const&) = delete;
		
		void init(String^ amsNetId);
		void deinit();
		void messageSent(Wrapper_TcMessage *message);
		void alarmRaised(Wrapper_TcAlarm* alarm);
		void alarmCleared(Wrapper_TcAlarm* alarm, bool bRemove);
		void alarmConfirmed(Wrapper_TcAlarm* alarm, bool bRemove);

		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_instance;
		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_user;
		std::function<void(Wrapper_TcAlarm*)> m_fPtr_alarmRaised_instance;
		std::function<void(Wrapper_TcAlarm*)> m_fPtr_alarmRaised_user;
		std::function<void(Wrapper_TcAlarm*, bool)> m_fPtr_alarmCleared_instance;
		std::function<void(Wrapper_TcAlarm*, bool)> m_fPtr_alarmCleared_user;
		std::function<void(Wrapper_TcAlarm*, bool)> m_fPtr_alarmConfirmed_instance;
		std::function<void(Wrapper_TcAlarm*, bool)> m_fPtr_alarmConfirmed_user;
	private:
		EventLoggerProxy() { /* Console::WriteLine("EventLoggerProxy() Constructor Called!"); */ };
		~EventLoggerProxy() { /* Console::WriteLine("~EventLoggerProxy() Destructor Called!"); */ };
		
		gcroot<LoggerEventHandler^> m_handler;
		gcroot<TcEventLogger^> m_logger;
	};
}
