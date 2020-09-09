#pragma once

#include "socket_event.h"

namespace iocpspp
{
	class send_data_event
		: public socket_event
	{
	public:
		__forceinline send_data_event() {};
		__forceinline virtual ~send_data_event() {};
	};
}
