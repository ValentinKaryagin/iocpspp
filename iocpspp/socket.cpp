#include "socket.h"

#include <WS2tcpip.h>
#include <MSWSock.h>

#include <new>
#include <cstdio>

namespace iocpspp
{
	const WIN32_SOCKET socket::INVALID = INVALID_SOCKET;

	socket::socket()
	{
		InitializeCriticalSection((CRITICAL_SECTION *)win32_critical_section);
	}

	socket::~socket()
	{
		DeleteCriticalSection((CRITICAL_SECTION *)win32_critical_section);

		terminate();
	}

	int socket::init(size_t recv_buffer_size)
	{
		int error_level = 0;

		this->recv_buffer_size = recv_buffer_size;

		recv_buffer = new (std::nothrow) unsigned char[recv_buffer_size] {};
		if (!recv_buffer)
		{
			++error_level;
		}

		win32_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (win32_socket == INVALID_SOCKET)
		{
			++error_level;
		}

		if (error_level)
		{
			terminate();
		}

		return error_level;
	}

	int socket::init(WIN32_SOCKET win32_socket, size_t recv_buffer_size)
	{
		int error_level = 0;

		this->recv_buffer_size = recv_buffer_size;

		recv_buffer = new (std::nothrow) unsigned char[recv_buffer_size] {};
		if (!recv_buffer)
		{
			++error_level;
		}

		this->win32_socket = win32_socket;
		if (this->win32_socket == INVALID_SOCKET)
		{
			++error_level;
		}

		if (error_level)
		{
			terminate();
		}

		return error_level;
	}

	void socket::terminate()
	{
		WIN32_SOCKET old_win32_socket_value = InterlockedExchange(&win32_socket, INVALID_SOCKET);
		if (old_win32_socket_value != INVALID_SOCKET)
		{
			shutdown(old_win32_socket_value, SD_BOTH);
			closesocket(old_win32_socket_value);

			if (send_buffer)
			{
				delete[] send_buffer;
				send_buffer = nullptr;
			}

			if (recv_buffer)
			{
				delete[] recv_buffer;
				recv_buffer = nullptr;
			}
		}
	}

	int socket::bind(unsigned short port, const wchar_t *ip_address)
	{
		int error_level = 0;

		SOCKADDR_IN addr{};
		addr.sin_family	= AF_INET;
		addr.sin_port	= htons(port);
		if (InetPtonW(AF_INET, ip_address, &addr.sin_addr) != 1)
		{
			++error_level;
		}

		if (!error_level)
		{
			if (::bind(win32_socket, (const SOCKADDR *)&addr, sizeof(SOCKADDR)) != 0)
			{
				++error_level;
			}
		}

		return error_level;
	}

	int socket::listen()
	{
		int error_level = 0;

		if (::listen(win32_socket, SOMAXCONN) != 0)
		{
			++error_level;
		}

		return error_level;
	}

	int socket::accept(socket *acceptor)
	{
		int error_level = 0;

		if (!AcceptEx(
			win32_socket,
			acceptor->win32_socket,
			recv_buffer,
			0,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			&received,
			(OVERLAPPED *)recv_overlapped))
		{
			int res = WSAGetLastError();
			if (res != ERROR_IO_PENDING)
			{
				++error_level;
			}
		}

		return error_level;

		return 0;
	}

	int socket::recv_pending()
	{
		int error_level = 0;

		DWORD flags = 0;
		DWORD received = 0;

		((WSABUF *)win32_recv_buffer)->buf = (CHAR *)recv_buffer;
		((WSABUF *)win32_recv_buffer)->len = (ULONG)recv_buffer_size;

		if (WSARecv(win32_socket, (WSABUF *)win32_recv_buffer, 1, &received, &flags, (OVERLAPPED *)recv_overlapped, NULL) == SOCKET_ERROR)
		{
			int res = WSAGetLastError();
			if (res != ERROR_IO_PENDING && res != WSAECONNRESET)
			{
				++error_level;
			}
		}

		return error_level;
	}

	void socket::lock()
	{
		EnterCriticalSection((CRITICAL_SECTION *)win32_critical_section);
	}

	bool socket::try_lock()
	{
		return (bool)TryEnterCriticalSection((CRITICAL_SECTION *)win32_critical_section);
	}

	void socket::unlock()
	{
		LeaveCriticalSection((CRITICAL_SECTION *)win32_critical_section);
	}
}
