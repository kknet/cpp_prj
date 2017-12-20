/*!
* \brief       ����һ����������.
* \author      ���@�ڰ��Ƽ�����ʵ����.
* \date        -
*
* \usage
* 
*
*/

#ifndef _TCP_SOCKET_H_  
#define _TCP_SOCKET_H_  

#include <string>
#include "common/Global.h"

namespace network {

// �����ĸ�����з�װ.
enum NETWORK_LIB_TYPE {
	LIB_EVENT,
	BOOST_ASIO,
};

class TcpSocket;

// �յ�����.
class SocketReaderSpi {
public:
	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len) = 0;
};
// �������ӶϿ�.
class SocketDissConnSpi {
public:
	virtual void OnDisconnect(TcpSocket *tcp_sock) = 0;
};

class NETWORK_API TcpSocket
{
public:

	virtual ~TcpSocket();
	
	// ������.
	virtual bool Send(char* buf, int len, std::string &err) = 0;


protected:
	TcpSocket(SocketReaderSpi* reader_spi, SocketDissConnSpi* disconn_spi);

	SocketReaderSpi *read_spi_;
	SocketDissConnSpi *disconn_spi_;

};



}

#endif