#include "server.h"

#include <WS2tcpip.h>
#include <strsafe.h>

#include <new>

#include "socket_factory.h"
#include "event_listener.h"

namespace iocpspp
{
	static volatile				size_t												wsa_counter = 0;

	const short					server::PORT_DEFAULT								= 7777;
	const wchar_t				server::IP_ADDRESS_DEFAULT[IP_ADDRESS_MAX_LENGTH]	= L"0.0.0.0";

	const server::init_settings	server::init_settings::DEFAULT						= server::init_settings(PORT_DEFAULT, IP_ADDRESS_DEFAULT);

	server::init_settings::init_settings(short port, const wchar_t *ip_address)
		: port(port)
	{
		StringCchCopyW(this->ip_address, IP_ADDRESS_MAX_LENGTH, ip_address);
	}

	server::server()
		: socket_factory_ptr(socket_factory::get_instance())
	{}

	server::~server()
	{
		terminate();
	}

	int server::init(const init_settings &settings)
	{
		int error_level = 0;

		is_exit_requested = false;

		if (InterlockedIncrement(&wsa_counter) == 1)
		{
			WSADATA wsa_data{};
			if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
			{
				++error_level;
			}
		}

		if (!error_level)
		{			
			iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
			if (!iocp)
			{
				++error_level;
			}
		}		

		SYSTEM_INFO sys_info{};
		GetSystemInfo(&sys_info);

		threads_count = (size_t)sys_info.dwNumberOfProcessors;

		threads = new (std::nothrow) void *[threads_count] {};
		if (!threads)
		{
			++error_level;
		}

		if (!error_level)
		{
			for (size_t i = 0; i < threads_count; ++i)
			{
				threads[i] = CreateThread(NULL, 0, &server::thread_worker, this, 0, NULL);
				if (threads[i] == INVALID_HANDLE_VALUE)
				{
					++error_level;
					break;
				}
			}
		}

		if (!error_level)
		{
			acceptor = socket_factory_ptr->create();
			if (!acceptor)
			{
				++error_level;
			}
		}

		if (!error_level)
		{
			if (acceptor->init() != 0)
			{
				++error_level;
			}
		}

		if (!error_level)
		{
			if (listener.init() != 0)
			{
				++error_level;
			}
		}

		if (!error_level)
		{
			void *new_iocp = CreateIoCompletionPort((HANDLE)listener.win32_socket, iocp, (ULONG_PTR)&listener, 0);
			if (!new_iocp)
			{
				++error_level;
			}
			else
			{
				iocp = new_iocp;
			}
		}
		
		if (!error_level)
		{
			if (listener.bind(settings.port, settings.ip_address) != 0)
			{
				++error_level;
			}
		}

		if (!error_level)
		{
			if (listener.listen() != 0)
			{
				++error_level;
			}
		}	

		if (!error_level)
		{
			if (listener.accept(acceptor) != 0)
			{
				++error_level;
			}
		}

		if (error_level)
		{
			terminate();
		}

		return error_level;
	}

	void server::terminate()
	{
		is_exit_requested = true;

		if (listener.is_valid())
		{
			listener.terminate();
		}

		if (acceptor)
		{
			socket_factory_ptr->destroy(acceptor);
		}

		if (threads)
		{
			for (size_t i = 0; i < threads_count; ++i)
			{
				CloseHandle(threads[i]);
				threads[i] = INVALID_HANDLE_VALUE;
			}

			delete[] threads;
			threads = nullptr;

			threads_count = 0;
		}

		if (iocp)
		{
			CloseHandle(iocp);
			iocp = nullptr;
		}

		if (!InterlockedDecrement(&wsa_counter))
		{
			WSACleanup();
		}
	}

	unsigned long __stdcall server::thread_worker(void *params)
	{
		server		*server_ptr			= (server *)params;

		DWORD		bytes_transferred	= 0;
		ULONG_PTR	completion_key		= 0;
		OVERLAPPED	*overlapped			= nullptr;
		BOOL		status				= FALSE;

		socket		*socket_ptr			= nullptr;

		int			error_level			= 0;

		while (!server_ptr->is_exit_requested)
		{
			status = GetQueuedCompletionStatus(server_ptr->iocp, &bytes_transferred, &completion_key, &overlapped, 1000);
			if (!status)
			{
				socket_ptr = (socket *)completion_key;
				if (socket_ptr)
				{					
					if (overlapped)
					{
						if (server_ptr->event_listener_ptr)
						{
							disconnect_event event;
							event.set_server(server_ptr);
							event.set_socket(socket_ptr);
							server_ptr->event_listener_ptr->on_disconnect(&event);
						}

						server_ptr->socket_factory_ptr->destroy(socket_ptr);
					}
				}
			}
			else
			{
				socket_ptr = (socket *)completion_key;
				if (socket_ptr)
				{
					if (socket_ptr == &server_ptr->listener)
					{
						socket *connected = server_ptr->acceptor;
						server_ptr->acceptor = nullptr;

						if (!error_level)
						{
							server_ptr->acceptor = server_ptr->socket_factory_ptr->create();
							if (!server_ptr->acceptor)
							{
								++error_level;
								break;
							}
						}

						if (!error_level)
						{
							if (server_ptr->acceptor->init() != 0)
							{
								++error_level;
								break;
							}
						}

						if (!error_level)
						{
							if (server_ptr->listener.accept(server_ptr->acceptor) != 0)
							{
								++error_level;
								break;
							}
						}

						if (server_ptr->event_listener_ptr)
						{
							connect_event event;
							event.set_server(server_ptr);
							event.set_socket(socket_ptr);
							server_ptr->event_listener_ptr->on_connect(&event);
						}

						void *new_iocp = CreateIoCompletionPort((HANDLE)connected->win32_socket, server_ptr->iocp, (ULONG_PTR)connected, 0);
						if (!new_iocp)
						{
							++error_level;
							break;
						}
						else
						{
							server_ptr->iocp = new_iocp;
						}

						if (!error_level)
						{
							if (connected->recv_pending() != 0)
							{
								++error_level;
								break;
							}
						}
					}
					else
					{
						if (overlapped == (OVERLAPPED *)socket_ptr->recv_overlapped)
						{
							if (bytes_transferred)
							{
								if (server_ptr->event_listener_ptr)
								{
									recv_data_event event;
									event.set_server(server_ptr);
									event.set_socket(socket_ptr);
									server_ptr->event_listener_ptr->on_data_recv(&event);
								}

								socket_ptr->recv_pending();
							}
							else
							{
								if (server_ptr->event_listener_ptr)
								{
									disconnect_event event;
									event.set_server(server_ptr);
									event.set_socket(socket_ptr);
									server_ptr->event_listener_ptr->on_disconnect(&event);
								}

								delete socket_ptr;
							}
						}
						else if (overlapped == (OVERLAPPED *)socket_ptr->send_overlapped)
						{
							if (bytes_transferred)
							{
								if (server_ptr->event_listener_ptr)
								{
									send_data_event event;
									event.set_server(server_ptr);
									event.set_socket(socket_ptr);
									server_ptr->event_listener_ptr->on_data_send(&event);
								}
							}
							else
							{
								if (server_ptr->event_listener_ptr)
								{
									disconnect_event event;
									event.set_server(server_ptr);
									event.set_socket(socket_ptr);
									server_ptr->event_listener_ptr->on_disconnect(&event);
								}

								delete socket_ptr;
							}
						}
					}
				}
			}

			bytes_transferred	= 0;
			completion_key		= 0;
			overlapped			= nullptr;
			status				= FALSE;

			socket_ptr			= nullptr;
		}

		return error_level;
	}
}
