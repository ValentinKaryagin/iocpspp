#pragma once

#include "socket.h"

namespace iocpspp
{
	constexpr size_t IP_ADDRESS_MAX_LENGTH = 16;

	class socket_factory;
	class event_listener;

	class server
	{
	public:		
		static const short		PORT_DEFAULT;
		static const wchar_t	IP_ADDRESS_DEFAULT[IP_ADDRESS_MAX_LENGTH];

	public:
		struct init_settings
		{
			static const init_settings DEFAULT;

			short	port								= 0;
			wchar_t	ip_address[IP_ADDRESS_MAX_LENGTH]	= L"";

			init_settings(short port, const wchar_t *ip_address);
		};

	private:
		volatile bool is_exit_requested		= false;

	private:
		socket_factory	*socket_factory_ptr	= nullptr;
		event_listener	*event_listener_ptr	= nullptr;
		void			*iocp				= nullptr;
		size_t			threads_count		= 0;
		void			**threads			= nullptr;
		socket			*acceptor			= nullptr;
		socket			listener;

	public:
		server();
		virtual ~server();

	public:
		int init(const init_settings &settings = init_settings::DEFAULT);
		void terminate();

	public:
		__forceinline void request_exit()
		{
			is_exit_requested = true;
		}

	public:
		__forceinline void set_socket_factory(socket_factory *ptr)
		{
			socket_factory_ptr = ptr;
		}

		__forceinline void set_event_listener(event_listener *ptr)
		{
			event_listener_ptr = ptr;
		}

	public:
		static unsigned long __stdcall thread_worker(void *params);
	};
}
