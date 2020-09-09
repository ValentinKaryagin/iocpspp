#pragma once

#include "socket_event.h"

namespace iocpspp
{
	class socket;

	class connect_event
		: public socket_event
	{
	public:
		__forceinline connect_event() {};
		__forceinline virtual ~connect_event() {};
	};
}
