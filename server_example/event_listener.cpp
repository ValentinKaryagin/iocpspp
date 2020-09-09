#include "event_listener.h"

#include <Windows.h>

#include <cstdio>

static volatile unsigned long connections_count = 0;
static volatile unsigned long connections_total = 0;

event_listener::event_listener() {}

event_listener::~event_listener() {}

void event_listener::on_connect(iocpspp::connect_event *event)
{
	//printf(__FUNCTION__ "\n");

	//wchar_t console_title[MAX_PATH]{};
	//wsprintfW(console_title, L"connections: %lu total: %lu", InterlockedIncrement(&connections_count), InterlockedIncrement(&connections_total));
	//SetConsoleTitleW(console_title);
}

void event_listener::on_disconnect(iocpspp::disconnect_event *event)
{
	//printf(__FUNCTION__ "\n");

	//wchar_t console_title[MAX_PATH]{};
	//wsprintfW(console_title, L"connections: %lu total: %lu", InterlockedDecrement(&connections_count), connections_total);
	//SetConsoleTitleW(console_title);
}

void event_listener::on_data_recv(iocpspp::recv_data_event *event)
{
	//printf(__FUNCTION__ "\n");
}

void event_listener::on_data_send(iocpspp::send_data_event *event)
{
	//printf(__FUNCTION__ "\n");
}
