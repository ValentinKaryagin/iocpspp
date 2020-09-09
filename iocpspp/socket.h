#pragma once

namespace iocpspp
{
#if _WIN64
	constexpr auto WIN32_WSABUF_SIZE			= 16;
	constexpr auto WIN32_CRITICAL_SECTION_SIZE	= 40;
	constexpr auto WIN32_OVERLAPPED_SIZE		= 32;
#else
	constexpr auto WIN32_WSABUF_SIZE			= 8;
	constexpr auto WIN32_CRITICAL_SECTION_SIZE	= 24;
	constexpr auto WIN32_OVERLAPPED_SIZE		= 20;
#endif

	constexpr auto RECV_BUFFER_SIZE_DEFAULT		= 8196;

#ifdef _WIN64
	typedef unsigned long long	WIN32_SOCKET;
#else
	typedef int WIN32_SOCKET;
#endif

	typedef unsigned char WIN32_WSABUF[WIN32_WSABUF_SIZE];
	typedef unsigned char WIN32_CRITICAL_SECTION[WIN32_CRITICAL_SECTION_SIZE];
	typedef unsigned char WIN32_OVERLAPPED[WIN32_OVERLAPPED_SIZE];

	class socket
	{
	public:
		friend class server;

	public:
		static const WIN32_SOCKET INVALID;

	private:
		size_t					recv_buffer_size		= 0;
		size_t					send_buffer_size		= 0;
		unsigned char			*recv_buffer			= nullptr;
		unsigned char			*send_buffer			= nullptr;
		WIN32_WSABUF			win32_recv_buffer		{};
		WIN32_WSABUF			win32_send_buffer		{};
		WIN32_SOCKET			win32_socket			= socket::INVALID;
		WIN32_CRITICAL_SECTION	win32_critical_section	{};		
		WIN32_OVERLAPPED		recv_overlapped			{};
		WIN32_OVERLAPPED		send_overlapped			{};
		unsigned long			received				= 0;

	public:
		socket();
		virtual ~socket();

	private:
		int init(size_t recv_buffer_size = RECV_BUFFER_SIZE_DEFAULT);
		int init(WIN32_SOCKET win32_socket, size_t recv_buffer_size = RECV_BUFFER_SIZE_DEFAULT);
		void terminate();

	public:
		int bind(unsigned short port, const wchar_t *ip_address);
		int listen();
		int accept(socket *acceptor);

	private:
		int recv_pending();

	public:
		__forceinline WIN32_SOCKET get_win32_socket() const
		{
			return win32_socket;
		}

		__forceinline bool is_valid() const
		{
			return win32_socket != INVALID;
		}

	public:
		void lock();
		bool try_lock();
		void unlock();
	};
}
