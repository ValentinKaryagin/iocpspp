#include "..\iocpspp\iocpspp.h"

class event_listener
	: public iocpspp::event_listener
{
protected:
	event_listener();

public:
	virtual ~event_listener();

public:
	void on_connect(iocpspp::connect_event *event) override;
	void on_disconnect(iocpspp::disconnect_event *event) override;
	void on_data_recv(iocpspp::recv_data_event *event) override;
	void on_data_send(iocpspp::send_data_event *event) override;

public:
	__forceinline static event_listener *get_instance()
	{
		static event_listener instance;
		return &instance;
	}
};
