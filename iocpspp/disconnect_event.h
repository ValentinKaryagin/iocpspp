#pragma once

#include "socket_event.h"

namespace iocpspp
{
	class disconnect_event
		: public socket_event
	{
	public:
		__forceinline disconnect_event() {};
		__forceinline virtual ~disconnect_event() {};
	};
}
