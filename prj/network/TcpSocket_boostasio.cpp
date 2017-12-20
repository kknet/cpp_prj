
#include <boost/thread.hpp>
#include "common/AppLog.h"
#include "TcpSocket_boostasio.h"

namespace network {

#define EXPIRES 5000 //����.
#define NUMBER_ 6 // 
#define EXPIRES_TIME (is_server_?EXPIRES*NUMBER_ :EXPIRES)

char *CHECK_CODE = "Zhongan"; // У����.
#define SEND_HEART_BEAT Send(CHECK_CODE, 8, err);

TcpSocket_boostasio::TcpSocket_boostasio(boost::asio::io_service& io_service, 
	SocketReaderSpi* read_spi, SocketDissConnSpi* dis_conn_spi, bool is_server)
	: TcpSocket(read_spi, dis_conn_spi)
	, io_service_(io_service)
	, socket_(io_service)
	, is_server_(is_server)
	, timer_(io_service_)
	, client_ii_a(0)
{

}

TcpSocket_boostasio::~TcpSocket_boostasio(){
	socket_.close();
	boost::system::error_code ec;
	//�˺������ûᵼ��������δ���ص�async_wait(handler)��handler������,ͬʱerror_codeΪboost::asio::error::operation_aborted
	//����ֵ�Ǳ�cancel��timer����.
	timer_.cancel(ec);
}

void TcpSocket_boostasio::start() {
	socket_.set_option(boost::asio::socket_base::keep_alive(true));

	boost::asio::async_read(socket_,
		boost::asio::buffer(read_message_.header(), read_message_.head_length()),
		boost::bind(&TcpSocket_boostasio::handle_read_header, this, boost::asio::placeholders::error));

	timer_.expires_from_now(boost::posix_time::milliseconds(EXPIRES_TIME));
	timer_.async_wait(boost::bind(&TcpSocket_boostasio::OnTimer, this, boost::asio::placeholders::error));
}

void TcpSocket_boostasio::OnTimer(const boost::system::error_code& ec){
	if (ec) return;

	if (is_server_) { // ��������EXPIRES_TIME���Ƿ��յ�������,���û�յ�������������
		if (!server_recv_data_) {
			//APP_LOG(LOG_LEVEL_INFO) << "delete this: " << socket_.remote_endpoint().address().to_string() << "\t" << socket_.remote_endpoint().port();
			if (disconn_spi_)disconn_spi_->OnDisconnect(this);
			delete this;
			return;
		}
		server_recv_data_ = false;
	}
	else { // �ͻ������������NUMBER_�ζ�û�յ�����������������
		++client_ii_a;
		//APP_LOG(LOG_LEVEL_INFO) << "send heart beat. client_ii_a==" << client_ii_a;
		if (client_ii_a >= NUMBER_) {
			//APP_LOG(LOG_LEVEL_INFO) << "delete this: " << socket_.remote_endpoint().address().to_string() << "\t" << socket_.remote_endpoint().port();
			if (disconn_spi_)disconn_spi_->OnDisconnect(this);
			delete this;
			return;
		}

		string err;
		SEND_HEART_BEAT
	}

	timer_.expires_from_now(boost::posix_time::milliseconds(EXPIRES_TIME));
	timer_.async_wait(boost::bind(&TcpSocket_boostasio::OnTimer, this, boost::asio::placeholders::error));
}

void TcpSocket_boostasio::close() {
	io_service_.post(boost::bind(&TcpSocket_boostasio::handle_close, shared_from_this()));
}

void TcpSocket_boostasio::handle_close() {
	try{
		socket_.close();
		if (disconn_spi_)disconn_spi_->OnDisconnect(this);
		delete this;
	}
	catch (std::exception& e){

	}
	catch (...){

	}
}

bool TcpSocket_boostasio::Send(char* buf, int len, std::string &err) {
	TcpMessage msg;
	msg.encode_header(buf, len);

	common::Locker locker(&write_message_mutex_);
	bool write_in_progress = !write_message_.empty();
	write_message_.push_back(msg);
	if (!write_in_progress){
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_message_.front().body(), write_message_.front().body_length()),
			boost::bind(&TcpSocket_boostasio::handle_write, this, boost::asio::placeholders::error));
	}

	return true;
}

void TcpSocket_boostasio::handle_write(const boost::system::error_code& error){

	if (!error){
		common::Locker locker(&write_message_mutex_);

		write_message_.front().clear_data();
		write_message_.pop_front();
		if (!write_message_.empty()){
			boost::asio::async_write(socket_,
				boost::asio::buffer(write_message_.front().body(), write_message_.front().body_length()),
				boost::bind(&TcpSocket_boostasio::handle_write, this, boost::asio::placeholders::error));
		}
	}
	// Զ�˹ر��׽���.
	else if (error == boost::asio::error::connection_reset || error == boost::asio::error::eof) {
		if (disconn_spi_)disconn_spi_->OnDisconnect(this);
		delete this;
	}
}

void TcpSocket_boostasio::handle_read_header(const boost::system::error_code& error)
{
	if (!error) {
		if (is_server_) server_recv_data_ = true;
		else client_ii_a = 0;

		read_message_.decode_header();
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_message_.body(), read_message_.body_length()),
			boost::bind(&TcpSocket_boostasio::handle_read_body, this, boost::asio::placeholders::error));
	}
	else if (error == boost::asio::error::connection_reset || error == boost::asio::error::eof) {
		if (disconn_spi_)disconn_spi_->OnDisconnect(this);
		delete this;
	}
}

void TcpSocket_boostasio::handle_read_body(const boost::system::error_code& error)
{
	if (!error) {
		if (read_message_.data_is_leagle()) {
			if (is_server_ && read_message_.is_heart_beat()) {
				// �������յ���������Ӧ���������������ͻ���.
				string err;
				SEND_HEART_BEAT
					//APP_LOG(LOG_LEVEL_INFO) << "receive heart beat and send back";
			}
			else {
				if (read_message_.is_heart_beat()) {
					//APP_LOG(LOG_LEVEL_INFO) << "receive heart beat";
				}
				else {
					// �����ϲ�������.
					if (read_spi_) read_spi_->OnReceive(this, read_message_.body(), read_message_.body_length());
				}
			}

			read_message_.clear_data();

			boost::asio::async_read(socket_,
				boost::asio::buffer(read_message_.header(), read_message_.head_length()),
				boost::bind(&TcpSocket_boostasio::handle_read_header, this, boost::asio::placeholders::error));
		}
		else {
			if (disconn_spi_)disconn_spi_->OnDisconnect(this);
			read_message_.clear_data();
			// ��������
			delete this;
		}
	}
	else if (error == boost::asio::error::connection_reset || error == boost::asio::error::eof) {
		if (disconn_spi_)disconn_spi_->OnDisconnect(this);
		read_message_.clear_data();
		delete this;
	}
}

}