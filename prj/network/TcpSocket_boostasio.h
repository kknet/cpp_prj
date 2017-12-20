/*!
* \brief       ����һ����������.
* \author      ���@�ڰ��Ƽ�����ʵ����.
* \date        -
*
* \usage
* 
*
*/

#ifndef _TCP_SOCKET_BOOST_ASIO_H_  
#define _TCP_SOCKET_BOOST_ASIO_H_  

#include <iostream>
#include <vector>
#include <deque>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include "common/Mutex.h"
#include "network/TcpSocket.h"
#include "TcpMessage.h"

using namespace boost::asio::ip;
using namespace zhongan;

namespace network {



typedef std::deque<TcpMessage> TcpMessageQueue;


class  TcpSocket_boostasio : public TcpSocket, public boost::enable_shared_from_this<TcpSocket_boostasio>
{
public:
	TcpSocket_boostasio(boost::asio::io_service& io_service, 
		SocketReaderSpi* read_spi, SocketDissConnSpi* dis_conn_spi, bool is_server=false);
	virtual ~TcpSocket_boostasio();
	
	virtual bool Send(char* buf, int len, std::string &err);

	tcp::socket& socket() { return socket_; }
	void start();
	void close();

private:
	void handle_read_header(const boost::system::error_code& error);
	void handle_read_body(const boost::system::error_code& error);
	void handle_write(const boost::system::error_code& error);
	void handle_close();

	boost::asio::io_service &io_service_;
	// The socket used to communicate with the client.
	tcp::socket socket_;

	

	TcpMessage read_message_; // �����������.
	TcpMessageQueue write_message_;
	common::Mutex write_message_mutex_;

	// �Ƿ���˻��ǿͻ���.
	bool is_server_;
	bool server_recv_data_; // ��������EXPIRES_TIME���Ƿ��յ�������,���û�յ�������������.
	// �ͻ��˼�������<NUMBER_�����ͻ���ÿ��EXPIRES��������������������������NUMBER_�ζ�û�յ�����������������.
	int client_ii_a; 

	//����ˣ����EXPIRES_TIME ���ղ������ݣ������ϸ�����.
	//�ͻ��ˣ�ÿ��EXPIRES_TIME�����˷�һ����������������6���ղ������������ص���������������.
	boost::asio::deadline_timer timer_;
	void OnTimer(const boost::system::error_code&);
};

typedef boost::shared_ptr<TcpSocket_boostasio> TcpSocket_boostasioPtr;




}

#endif