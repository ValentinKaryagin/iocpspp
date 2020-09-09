#include "..\iocpspp\iocpspp.h"

#include "event_listener.h"

#include <thread>

int main()
{
	int error_level = 0;

	iocpspp::server server;
	server.set_event_listener(event_listener::get_instance());

	error_level = server.init();
	if (!error_level)
	{
		for (;;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}

	return error_level;
}
