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
		LoggerEventHandler(std::function<void(Wrapper_TcMessage *message)>* messageSend)
			: m_messageSend(messageSend) {	};

		void MessageSent(TcMessage ^message);
		//void AlarmRaised(TcAlarm evtObj) {};
		//void AlarmCleared(TcAlarm evtObj, bool bRemove) {};
		//void AlarmConfirmed(TcAlarm evtObj, bool bRemove) {};
	private:
		std::function<void(Wrapper_TcMessage *message)> *m_messageSend;
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

		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_instance;
		std::function<void(Wrapper_TcMessage*)> m_fPtr_messageSend_user;
	private:
		EventLoggerProxy() { /* Console::WriteLine("EventLoggerProxy() Constructor Called!"); */ };
		~EventLoggerProxy() { /* Console::WriteLine("~EventLoggerProxy() Destructor Called!"); */ };
		
		gcroot<LoggerEventHandler^> m_handler;
		gcroot<TcEventLogger^> m_logger;
	};
}
