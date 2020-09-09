#pragma once

#include "socket.h"

#include <new>

namespace iocpspp
{
	class socket_factory
	{
	protected:
		__forceinline socket_factory() {};

	public:
		__forceinline virtual ~socket_factory() {};

	public:
		__forceinline virtual socket *create()
		{
			return new (std::nothrow) socket();
		}

		__forceinline virtual void destroy(socket *ptr)
		{
			delete ptr;
		}

	public:
		__forceinline static socket_factory *get_instance()
		{
			static socket_factory instance;
			return &instance;
		}
	};
}
