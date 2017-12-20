#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "common/Thread.h"
#include "common/DateTime.h"
#include "common/SpinLock.h"

using namespace zhongan;
using namespace zhongan::common;


#include "ThostFtdcUserApiStruct.h"

struct kline
{
	char tday[9];//����
	char updatetime[9];//ʱ��
	double open;//���̼�
	double high;//��߼�
	double low;//��ͼ�
	double close;//���̼�
	int    vol;//�ɽ���
	double amt;//�ɽ���
	int    ccl;//�ֲ���

	kline() { memset(this, 0, sizeof(kline)); }
};


/*
class KlineMergerSpi {
public:
	virtual void OnKline(kline *) = 0;
};*/

// ����˿̵���һ���ӵĺ�����.
inline int GetMilSecNow2NextMin(){
	DateTime now(NULL);
	DateTime next_min = now;
#if 1
	next_min.AddMin(1);	
	next_min.time.sec = 0;
	next_min.time.milsec = 0;
#else
	next_min.AddSec(1);
#endif
	
	return 1000 * (next_min - now);
}

// ����1min K��.
class KlineMergerApi : public Thread
{
public:
	KlineMergerApi();
	~KlineMergerApi();

	//void PushTick(CThostFtdcDepthMarketDataField *);

private:
	virtual void OnRun(){
		while (IsRuning())
		{
			boost::asio::io_service::work work(io_service_);
			io_service_.run();
		}
	}
	// ÿ��һ����ִ��һ��.
	void OnTimer(const boost::system::error_code&ec){
		if (ec) return;
		cout << "1\n";
		CreateBar();

		timer_.expires_from_now(boost::posix_time::milliseconds(GetMilSecNow2NextMin()));
		timer_.async_wait(boost::bind(&KlineMergerApi::OnTimer, this, boost::asio::placeholders::error));
	}

	boost::asio::io_service io_service_;
	boost::asio::deadline_timer timer_;

	void CreateBar();
};

