/*!
* \brief       ��ʱ��.
* \author      ���@�ڰ��Ƽ�����ʵ����.
* \date        -
*
* \usage
class MySpi : public TimerSpi {
public:
	virtual void OnTimer(){
		//cout << boost::posix_time::second_clock::local_time()<<"\t";
	}

	MySpi() {
		// ÿ��1���Ӵ���һ��.
		timer_ =  Timer::CreateTimer("2017-6-27 10:09:10", 5000, this);
		string err;
		timer_->StartUp(err);
	}

	Timer *timer_;
};
*
*/


#pragma once

#include <string>

#ifdef TIMER_EXPORT
#define TIMER_API  __declspec(dllexport)
#else 
#define TIMER_API  __declspec(dllimport)
#endif

class TimerSpi {
public:
	virtual void OnTimer() = 0;
};

class TIMER_API Timer
{
public:
	// begin_timeʱ���ʽ"2017-06-26 11:11:00". 
	// interval_time���ʱ�� ��λΪ����.
	static Timer *CreateTimer(char begin_time[31], int interval, TimerSpi *spi);
	virtual ~Timer();

	// ����.
	virtual bool StartUp(std::string &err) = 0;
	// ֹͣ.
	virtual void TearDown() = 0;

	virtual bool isActive() const = 0;

protected:
	TimerSpi *spi_;
	

};

