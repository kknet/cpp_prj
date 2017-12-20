#ifndef TCP_MESSAGE_H
#define TCP_MESSAGE_H

namespace network {


class TcpMessage
{
	enum {
		check_code_length = 8, // У����Ϊ"Zhongan"��8���ֽ�
		body_length_value = 12, // ���ݳ���ֵ
		max_buf_length = 1024, // ���������ò���
		max_body_length = 99999999
	};

public:
	TcpMessage(void) ;
	~TcpMessage(void);

	const char* header() const
	{
		return header_;
	}

	char* header()
	{
		return header_;
	}

	size_t head_length() const
	{
		return check_code_length + body_length_value;
	}

	const char* body() const
	{
		return body_msg_;
	}

	char* body()
	{
		return body_msg_;
	}

	size_t body_length() const
	{
		return body_length_;
	}

	void set_body_length(size_t new_length)
	{
		body_length_ = new_length;
		if (body_length_ > max_body_length)
			body_length_ = max_body_length;
	}

	bool data_is_leagle() const { return data_is_leagle_; }
	bool is_heart_beat() const;

	void decode_header(); //��Ϣͷ�Ѿ�����header_,��ͷ���н���.
	void encode_header(const char *, int);

	void clear_data();

private:
	char header_[check_code_length + body_length_value];
	size_t body_length_;
	char *body_msg_;
	bool data_is_leagle_;
};

}

#endif