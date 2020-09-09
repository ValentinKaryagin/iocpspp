#pragma once

namespace iocpspp
{
	class server;

	class server_event
	{
	public:
		friend class server;

	private:
		server *server_ptr = nullptr;

	public:
		__forceinline server_event() {};
		__forceinline virtual ~server_event() {};

	private:
		__forceinline void set_server(server *ptr)
		{
			server_ptr = ptr;
		}

	public:
		__forceinline server *get_server()
		{
			return server_ptr;
		}
	};
}
