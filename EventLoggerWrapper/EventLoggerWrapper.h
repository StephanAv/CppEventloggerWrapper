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
			std::function<void(Wrapper_TcAlarm*)>* alarmRaised
			)
			: m_messageSend(messageSend)
			, m_alarmRaised(alarmRaised)
			{	};

		void MessageSent(TcMessage ^message);
		void AlarmRaised(TcAlarm ^alarm);
		//void AlarmCleared(TcAlarm evtObj, bool bRemove) {};
		//void AlarmConfirmed(TcAlarm evtObj, bool bRemove) {};
	private:
		std::function<void(Wrapper_TcMessage *message)> *m_messageSend;
		std::function<void(Wrapper_TcAlarm* alarm)>* m_alarmRaised;
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
		void messageSent(Wrapper_TcMessage *message);
		void alarmRaised(Wrapper_TcAlarm* alarm);

		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_instance;
		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_user;
		std::function<void(Wrapper_TcAlarm*)> m_fPtr_alarmRaised_instance;
		std::function<void(Wrapper_TcAlarm*)> m_fPtr_alarmRaised_user;
	private:
		EventLoggerProxy() { /* Console::WriteLine("EventLoggerProxy() Constructor Called!"); */ };
		~EventLoggerProxy() { /* Console::WriteLine("~EventLoggerProxy() Destructor Called!"); */ };
		
		gcroot<LoggerEventHandler^> m_handler;
		gcroot<TcEventLogger^> m_logger;
	};
}
