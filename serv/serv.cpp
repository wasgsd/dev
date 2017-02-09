// serv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


//namespace asio = boost::asio;
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

class Publisher {
public:
	//void *zmqSocket;
	zmq::socket_t* zmqSocket;
	int send(char* ticker, char* field, char *value)
	{
		std::stringstream ss;
		ss << ticker << "|" << field << "=" << value;
		return send(ss.str());
	}
	int send(std::string s)
	{
		return zmqSocket->send(zmq::message_t(s.c_str(), s.length() + 1));
		//void *s_c_str = (void*)s.c_str();
		//size_t len = s.length() + 1;
		//return s_send(zmqSocket, s.c_str());
	}
};

using namespace BloombergLP;
using namespace blpapi;

class SubscriptionCorrelationExample
{
	class GridWindow {
		std::string                  d_name;
		std::vector<std::string>    &d_securities;

	public:
		GridWindow(const char *name, std::vector<std::string> &securities)
			: d_name(name), d_securities(securities)
		{
		}

		void processSecurityUpdate(Message &msg, int row)
		{
			const std::string &topicname = d_securities[row];

			std::cout << d_name << ":" << row << ',' << topicname << std::endl;
		}
	};

	std::string              d_host;
	int                      d_port;
	int                      d_maxEvents;
	int                      d_eventCount;
	std::vector<std::string> d_securities;
	GridWindow               d_gridWindow;

	void printUsage()
	{
		std::cout << "Usage:" << std::endl
			<< "    Retrieve realtime data " << std::endl
			<< "        [-ip        <ipAddress  = localhost>" << std::endl
			<< "        [-p         <tcpPort    = 8194>" << std::endl
			<< "        [-me        <maxEvents  = MAX_INT>" << std::endl;
	}

	bool parseCommandLine(int argc, char **argv)
	{
		for (int i = 1; i < argc; ++i) {
			if (!std::strcmp(argv[i], "-ip") && i + 1 < argc)
				d_host = argv[++i];
			else if (!std::strcmp(argv[i], "-p") && i + 1 < argc)
				d_port = std::atoi(argv[++i]);
			else if (!std::strcmp(argv[i], "-me") && i + 1 < argc)
				d_maxEvents = std::atoi(argv[++i]);
			else {
				printUsage();
				return false;
			}
		}
		return true;
	}

public:

	Publisher pub;
	SubscriptionCorrelationExample()
		: d_gridWindow("SecurityInfo", d_securities)
	{
		d_securities.push_back("USDJPY Curncy");
		//d_securities.push_back("IBM US Equity");
		//d_securities.push_back("VOD LN Equity");
	}

	void run(int argc, char **argv)
	{
		d_host = "localhost";
		d_port = 8194;
		d_maxEvents = INT_MAX;
		d_eventCount = 0;

		if (!parseCommandLine(argc, argv)) return;

		SessionOptions sessionOptions;
		sessionOptions.setServerHost(d_host.c_str());
		sessionOptions.setServerPort(d_port);

		std::cout << "Connecting to " << d_host << ":"
			<< d_port << std::endl;
		Session session(sessionOptions);
		if (!session.start()) {
			std::cerr << "Failed to start session." << std::endl;
			return;
		}
		if (!session.openService("//blp/mktdata")) {
			std::cerr << "Failed to open //blp/mktdata" << std::endl;
			return;
		}

		SubscriptionList subscriptions;
		for (size_t i = 0; i < d_securities.size(); ++i) {
			subscriptions.add(d_securities[i].c_str(),
				"LAST_PRICE",
				"",
				CorrelationId(i));
		}
		session.subscribe(subscriptions);

		while (true) {
			Event event = session.nextEvent();
			if (event.eventType() == Event::SUBSCRIPTION_DATA) {
				MessageIterator msgIter(event);
				while (msgIter.next()) {
					Message msg = msgIter.message();
					pub.send( std::string( msg.getElementAsString("LAST_PRICE") ));
					int row = (int)msg.correlationId().asInteger();
					d_gridWindow.processSecurityUpdate(msg, row);
				}
				if (++d_eventCount >= d_maxEvents) break;
			}
			else {
				std::cerr << "NON DATA event: " << event.eventType() << std::endl;
			}
		}
	}
};

std::ostream& operator<<(std::ostream& out, const BloombergLP::blpapi::Event::EventType value) {
	const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
	switch (value) {
		PROCESS_VAL(BLPAPI_EVENTTYPE_ADMIN);
		PROCESS_VAL(BLPAPI_EVENTTYPE_SESSION_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_SUBSCRIPTION_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_REQUEST_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_RESPONSE);
		PROCESS_VAL(BLPAPI_EVENTTYPE_PARTIAL_RESPONSE);
		PROCESS_VAL(BLPAPI_EVENTTYPE_SUBSCRIPTION_DATA);
		PROCESS_VAL(BLPAPI_EVENTTYPE_SERVICE_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_TIMEOUT);
		PROCESS_VAL(BLPAPI_EVENTTYPE_AUTHORIZATION_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_RESOLUTION_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_TOPIC_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_TOKEN_STATUS);
		PROCESS_VAL(BLPAPI_EVENTTYPE_REQUEST);
	}
#undef PROCESS_VAL

	//PROCESS_VAL(Event::EventType::ADMIN);
	//PROCESS_VAL(Event::EventType::SESSION_STATUS);
	//PROCESS_VAL(Event::EventType::SUBSCRIPTION_STATUS);
	//PROCESS_VAL(Event::EventType::REQUEST_STATUS);
	//PROCESS_VAL(Event::EventType::RESPONSE);
	//PROCESS_VAL(Event::EventType::PARTIAL_RESPONSE);
	//PROCESS_VAL(Event::EventType::SUBSCRIPTION_DATA);
	//PROCESS_VAL(Event::EventType::SERVICE_STATUS);
	//PROCESS_VAL(Event::EventType::TIMEOUT);
	//PROCESS_VAL(Event::EventType::AUTHORIZATION_STATUS);
	//PROCESS_VAL(Event::EventType::RESOLUTION_STATUS);
	//PROCESS_VAL(Event::EventType::TOPIC_STATUS);
	//PROCESS_VAL(Event::EventType::TOKEN_STATUS);
	//PROCESS_VAL(Event::EventType::REQUEST);
	//PROCESS_VAL(Event::EventType::UNKNOWN);
	return out << s;
}
int main(int argc, char **argv)
{
#if 1
	zmq::context_t ctx(1);
	zmq::socket_t zSoc(ctx, ZMQ_PUB);
	zSoc.bind("epgm://10.88.88.128;239.192.1.11:5553");
#else
	zmq::message_t msg(100);
	memset(msg.data(), 0, 100);
	zSoc.send(msg);

	void *context = zmq_ctx_new();
	zmq_msg_t msg;
	char buf[256];
	void *zSoc = zmq_socket(context, ZMQ_PUB);
	//zmq_connect(requester, "pgm://10.88.88.115;239.192.1.1:5555");
	//zmq_recv(requester, buf, 256, 0);

	int res = zmq_bind(zSoc, "epgm://10.88.88.128;239.192.1.11:5553");
	assert(res==0);
	Publisher p;
	p.zmqSocket = zSoc;
	p.send(std::string("test"));

	while (1) {
		int res = zmq_send(zSoc, "Hello", 5, 0);
		if (res <0 )
			std::cout << zmq_strerror(zmq_errno()) << std::endl;
		sleep(100);
	}
	zmq_close(zSoc);
	zmq_ctx_destroy(context);
#endif
	
	std::cout << "SubscriptionCorrelationExample" << std::endl;
	SubscriptionCorrelationExample example;
	example.pub.zmqSocket =  &zSoc;
	try {
		example.run(argc, argv);
	}
	catch (Exception &e) {
		std::cerr << "Library Exception!!! " << e.description() << std::endl;
	}
	// wait for enter key to exit application
	std::cout << "Press ENTER to quit" << std::endl;
	char dummy[2];
	std::cin.getline(dummy, 2);
	return 0;
}
