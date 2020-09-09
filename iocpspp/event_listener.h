#pragma once

#include "connect_event.h"
#include "disconnect_event.h"
#include "recv_data_event.h"
#include "send_data_event.h"

namespace iocpspp
{
	class event_listener
	{
	protected:
		__forceinline event_listener() {};

	public:
		__forceinline virtual ~event_listener() {};

	public:
		__forceinline virtual void on_connect(connect_event *event) {}
		__forceinline virtual void on_disconnect(disconnect_event *event) {}
		__forceinline virtual void on_data_recv(recv_data_event *event) {}
		__forceinline virtual void on_data_send(send_data_event *event) {}

	public:
		__forceinline static event_listener *get_instance()
		{
			static event_listener instance;
			return &instance;
		}
	};
}
