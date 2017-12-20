/*!
* \brief       ��������.
* \author      ���@�ڰ��Ƽ�����ʵ����.
* \date        -
*
* \usage
*
*
*/

#ifndef _TCP_SERVER_H_  
#define _TCP_SERVER_H_  

#include "network/TcpSocket.h"

namespace network {

class TcpServerConnSpi {
public:
	virtual void OnAccept(TcpSocket* tcp_sock) = 0;
	virtual void OnDiscon(TcpSocket *tcp_sock) = 0;
};

class NETWORK_API TcpServer
{
public:
	TcpServer *CreateTcpServer(NETWORK_LIB_TYPE type, 
		int port, SocketReaderSpi* spi, TcpServerConnSpi* conn_spi);

	virtual TcpServer::~TcpServer();
	
	virtual bool StartUp(std::string& err) = 0;

protected:		
	TcpServer(SocketReaderSpi* read_spi, TcpServerConnSpi* conn_spi = NULL);
	SocketReaderSpi* read_spi_;
	TcpServerConnSpi* conn_spi_;
};

}

#endif