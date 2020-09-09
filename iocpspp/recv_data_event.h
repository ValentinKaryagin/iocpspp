#pragma once

#include "socket_event.h"

namespace iocpspp
{
	class recv_data_event
		: public socket_event
	{
	public:
		__forceinline recv_data_event() {};
		__forceinline virtual ~recv_data_event() {};
	};
}
