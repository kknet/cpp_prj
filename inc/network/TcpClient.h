/*!
* \brief       ����ͻ���.
* \author      ���@�ڰ��Ƽ�����ʵ����.
* \date        -
*
* \usage
*
*
*/

#ifndef _TCP_CLIENT_H_  
#define _TCP_CLIENT_H_  

#include "network/TcpSocket.h"

namespace network {


class ReConnSpi
{
public:
	//�����һ�����ӡ� �����һ������ʧ�ܣ�������Ͽ�ʱ������successΪfalse
	virtual void SockConn(bool success) = 0;
	//����Ͽ�.
	virtual void SockDisconn() = 0;
	//��������.
	virtual void SockReConn() = 0;
};


class NETWORK_API TcpClient
{
public:
	TcpClient *CreateTcpClient(NETWORK_LIB_TYPE type,
		const char *ip, const char *port,
		SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);

	
	virtual ~TcpClient(void);

	// ����.
	virtual bool StartUp(std::string &err) = 0;
	// ֹͣ.
	virtual void TearDown() = 0;
	// ������.
	virtual bool Send(char* buf, int len, std::string &err) = 0;


protected:
	TcpClient(SocketReaderSpi* read_spi = NULL, ReConnSpi* re_conn_spi = NULL);

	SocketReaderSpi* read_spi_;
	ReConnSpi* re_conn_spi_;

};

}

#endif