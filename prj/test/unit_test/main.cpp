
#include <stdlib.h>
#include <iostream>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <gtest/gtest.h>
#include "common/Mutex.h"
#include "common/Global.h"
#include "common/Directory.h"
#include "common/AppLog.h"
#include "common/QueueBuffer.h"

using namespace std;
using namespace zhongan;
using namespace zhongan::common;

TEST(GlobalUT, name)
{
	EXPECT_STREQ("123", "12") << "456";
	EXPECT_EQ(Global::GetInstance()->GetAppName(), "test_common");
};

class IntQueueBuffer : public QueueBuffer<int, 50>{
	virtual bool Comsume(const int& val){
		cout << val << endl;
		return true;
	}
};

class IntQueueBufferUT : public ::testing::Test {
	
public:
	void SetUp(){
		
		int_queue_.Push(1);
		int_queue_.Push(1);
		int_queue_.Push(1);
	}
	void TearDown(){
		//int_queue_.Terminate();
	}


	IntQueueBuffer int_queue_;
};

TEST_F(IntQueueBufferUT, A)
{
	int_queue_.Start();
	//EXPECT_TRUE(int_queue_.siz)
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	return a.exec();
}
