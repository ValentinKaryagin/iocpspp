#pragma once

#include "server_event.h"

namespace iocpspp
{
	class socket;

	class socket_event
		: public server_event
	{
	public:
		friend class server;

	private:
		socket *socket_ptr = nullptr;

	public:
		__forceinline socket_event() {};
		__forceinline virtual ~socket_event() {};

	private:
		__forceinline void set_socket(socket *ptr)
		{
			socket_ptr = ptr;
		}

	public:
		__forceinline socket *get_socket()
		{
			return socket_ptr;
		}
	};
}
