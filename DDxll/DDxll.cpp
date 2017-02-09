#include "DDxll.hpp"
#include "xllMain.h"

#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"WS2_32")
//#pragma comment(linker, "/SUBSYSTEM:windows ")
int DDxllAutoOpen(void)
{
	return 0;
}

class ConnectZeroMQ {
public:
	boost::shared_ptr<zmq::socket_t> zSocSub;
	boost::shared_ptr<zmq::socket_t> zSocReq;

	ConnectZeroMQ()
		:zSocSub( new zmq::socket_t( *(new zmq::context_t(1)), ZMQ_SUB))
		,zSocReq( new zmq::socket_t( *(new zmq::context_t(1)), ZMQ_REQ))
	{
		//zSocSub->connect("epgm://10.88.88.128;239.192.1.11:5553");
		//zSocReq->connect("tcp://10.88.88.115:5555");
		zSocReq->connect("tcp://10.88.88.115:5551");
		zSocSub->connect("tcp://10.88.88.115:5552");
	}
	int conn()
	{
	}
	std::string req(std::string tpc, std::string msg)
	{
		zmq::message_t q;
		zSocReq->send(zmq::message_t(tpc.c_str(), tpc.length() + 1), ZMQ_SNDMORE);
		zSocReq->send(zmq::message_t(msg.c_str(), msg.length() + 1));
		zSocReq->recv(&q);
		return std::string((char*)q.data(), q.size());
	}
};

//ConnectZeroMQ conn;

DWORD WINAPI DDSret(LPVOID args)
{
	LPXLOPER12* opers = (LPXLOPER12*)args;
	XLOPER12 xlResult;

	ConnectZeroMQ conn;

	//// Simulate waiting for a long external operation.
	//Sleep(1000);
	//if (opers[0]->xltype & xltypeNum)
	//	opers[0]->val.num *= 2;

	//int retval = Excel12(xlAsyncReturn, &xlResult, 2, opers[1], opers[0]);

	std::string s = conn.req("test","");
	int retval = Excel12(xlAsyncReturn, &xlResult, 2, opers[1], TempCStr12( 
		std::wstring(s.begin(), s.end()).c_str() ));
	// Free the passed pointer array
	// (Excel itself calls xlAutoFree12 to free the XLOPERs, since they have xlbitDLLFree).  
	delete opers;

	ExitThread(0);
	return 0;
}

class ConnCache {
public:
	boost::shared_ptr<zmq::socket_t> zSocSub;
	boost::shared_ptr<zmq::socket_t> zSocReq;

	ConnCache()
		:zSocSub(new zmq::socket_t(*(new zmq::context_t(1)), ZMQ_XSUB))
		, zSocReq(new zmq::socket_t(*(new zmq::context_t(1)), ZMQ_REQ))
	{
		zSocSub->connect("epgm://10.88.88.128;239.192.1.11:5552");
		zSocReq->connect("tcp://10.88.88.115:5551");
	}
	int conn()
	{
	}
	std::string req(std::string s)
	{
		zmq::message_t q;
		zSocReq->send(zmq::message_t(s.c_str(), s.length() + 1));
		zSocReq->recv(&q);
		return std::string( (char*)q.data(), q.size());
	}
}; DWORD WINAPI DDDret(LPVOID args)

{
	LPXLOPER12* opers = (LPXLOPER12*)args;
	XLOPER12 xlResult;

	ConnectZeroMQ conn;

	// Simulate waiting for a long external operation.
	Sleep(1000);
	if (opers[0]->xltype & xltypeNum)
		opers[0]->val.num *= 2;

	int retval = Excel12(xlAsyncReturn, &xlResult, 2, opers[1], opers[0]);

	// Free the passed pointer array
	// (Excel itself calls xlAutoFree12 to free the XLOPERs, since they have xlbitDLLFree).  
	delete opers;

	ExitThread(0);
	return 0;
}


//void AsyncStubFailHelper(LPXLOPER12 asyncHandle)
//{
//	//an error handler with no heap/thread operations.  
//	//used to send back #VALUE! when an async stub fails
//	XLOPER12 operResult;
//
//	XLOPER12 operErr;
//	operErr.xltype = xltypeErr;
//	operErr.val.err = xlerrValue;
//
//	Excel12(xlAsyncReturn, &operResult, 2, asyncHandle, &operErr);
//}

/* !send DD
 
*/
void WINAPI DDS(LPXLOPER12 oper, LPXLOPER12 asyncHandle)
{
	// point to the arguments from a pointer array that will be freed by XllEchoSetReturn
	LPXLOPER12* argsArray = new LPXLOPER12[2];
	if (argsArray == NULL)
	{
		AsyncStubFailHelper(asyncHandle);
		return;
	}

	argsArray[0] = TempOper12(oper);
	if (argsArray[0] == NULL)
	{
		delete argsArray;
		AsyncStubFailHelper(asyncHandle);
	}

	argsArray[1] = TempOper12(asyncHandle);
	if (argsArray[1] == NULL)
	{
		xlAutoFree12(argsArray[0]);
		delete argsArray;
		AsyncStubFailHelper(asyncHandle);
	}

	// Simulate an external async operation - start a thread and return.
	if (CreateThread(NULL, 0, DDSret, argsArray, 0, NULL) == NULL)
	{
		xlAutoFree12(argsArray[1]);
		xlAutoFree12(argsArray[0]);
		delete argsArray;
		AsyncStubFailHelper(asyncHandle);
	}
}

