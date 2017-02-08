// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <zmq.hpp>
#include <azmq/socket.hpp>
#include <boost/asio.hpp>
#include <array>
#include <Windows.h>

#include <vector>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include <blpapi_session.h>
#include <blpapi_eventdispatcher.h>

#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_element.h>
#include <blpapi_name.h>
#include <blpapi_request.h>
#include <blpapi_subscriptionlist.h>
#include <blpapi_defs.h>

#define sleep(x) Sleep(x)
