#include <iostream>
#include <conio.h>
#include <functional>

#include "EventLoggerWrapper.Exports.h"

using namespace std::placeholders;
using namespace EventLoggerWrapper;


void eventMessage(Wrapper_TcMessage *message)
{
	std::cout << message->Message << std::endl;
}

void eventAlarmRaised(Wrapper_TcAlarm* alarm)
{
	std::cout << alarm->Message << std::endl;
}

void eventAlarmCleared(Wrapper_TcAlarm* alarm, bool bRemove)
{
	std::cout << (bRemove ? "Alarm Cleared and was Confirmed: " : "Alarm Cleared: ") << alarm->Message << std::endl;
}

void eventAlarmConfirmed(Wrapper_TcAlarm* alarm, bool bRemove)
{
	std::cout << (bRemove ? "Alarm Confirmed and was Cleared: " : "Alarm Confirmed: ") << alarm->Message << std::endl;
}

int main()
{
	// Info: the following files must be copied to the binaries folder
	// %USERPROFILE%\.nuget\packages\beckhoff.twincat.tceventloggeradsproxy.net\2.6.3\lib\net6.0\Beckhoff.TwinCAT.TcEventLoggerAdsProxy.Native.Interop.dll
	// %USERPROFILE%\.nuget\packages\beckhoff.twincat.tceventloggeradsproxy.net\2.6.3\runtimes\win-x64\native\TcEventLoggerAdsProxy.dll

	//EventLoggerWrapper::init("5.73.96.116.1.1");
	EventLoggerWrapper::init("5.80.201.232.1.1");
	EventLoggerWrapper::registerMessageSent(std::bind(eventMessage, _1));
	EventLoggerWrapper::registerAlarmRaised(std::bind(eventAlarmRaised, _1));
	EventLoggerWrapper::registerAlarmCleared(std::bind(eventAlarmCleared, _1, _2));
	EventLoggerWrapper::registerAlarmConfirmed(std::bind(eventAlarmConfirmed, _1, _2));

	do {
		std::cout << "Press ENTER to exit" << std::endl;;
	} while (_getch() != '\r');

	EventLoggerWrapper::deinit();
}