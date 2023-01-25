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


int main()
{
	// Info: the following files must be copied to the binaries folder
	// %USERPROFILE%\.nuget\packages\beckhoff.twincat.tceventloggeradsproxy.net\2.6.3\lib\net6.0\Beckhoff.TwinCAT.TcEventLoggerAdsProxy.Native.Interop.dll
	// %USERPROFILE%\.nuget\packages\beckhoff.twincat.tceventloggeradsproxy.net\2.6.3\runtimes\win-x64\native\TcEventLoggerAdsProxy.dll

	EventLoggerWrapper::init("5.80.201.232.1.1");
	EventLoggerWrapper::registerMessageSent(std::bind(&eventMessage, _1));

	do {
		std::cout << "Press ENTER to exit" << std::endl;;
	} while (_getch() != '\r');
}