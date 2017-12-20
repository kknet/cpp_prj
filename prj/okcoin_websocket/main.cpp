#include "okcoinwebsocketapi.h"
#include <fstream>
#include "JsonParser.h"

OKCoinWebSocketApiCom *comapi = 0;	//�˴�Ϊȫ�ֱ����������û����ɵ���ģʽ.
OKCoinWebSocketApiCn *cnapi = 0;	//�˴�Ϊȫ�ֱ����������û����ɵ���ģʽ.

//������webstocket�ص������ڹ���վ�͹���վ�ֱ����������ӣ�.
//Ҳ�����������������̷߳����ģ��ʹ���վ�͹���վ�и��ԵĻص�������.
//������߳�Ӧ�ò�̫�����Ŀ����ߣ�������Ҫ��һ���ص�����ͬʱ��������վ�����ݣ�������ʹ��ĳ����ø��ӡ�.
void cn_callbak_open()
{
	//���������������.
	
	std::cout << "����վ���ӳɹ���3���Ӻ�ʼ�������ݣ�����3�Ͽ����ӡ� " << std::endl;
	Sleep(3000);
	//���ӳɹ�����������tick��depth����.
	//���⣬�ѽ������������������open�ص�����������:.
	//������Ͽ����������Ӻ󴥷����ص����Զ����ͽ�������.
	//���Ծ���Ҫ��������Ľ���������ڱ��ص��.
	if(cnapi != 0)
	{
		cnapi->ok_spotcny_btc_ticker();
		cnapi->ok_spotcny_btc_depth_20();
		cnapi->ok_spotcny_btc_kline_1min();
	}
};
void cn_callbak_close()
{
	std::cout << "�����Ѿ��Ͽ��� " << std::endl;
};
void cn_callbak_message(const char *message)
{
	/*static ofstream ofs("ticker.txt");
	ofs << message << std::endl;
	std::cout << "Message: " << message << std::endl;*/

	Json2DrData(message);
};

//////////////////////////////////.

void com_callbak_open()
{
	//���������������.
	
	std::cout << "����վ���ӳɹ���3���Ӻ�ʼ�������ݣ�����3�Ͽ����� " << std::endl;
	Sleep(3000);
	//���ӳɹ�����������tick��depth����.
	//���⣬�ѽ������������������open�ص�����������:.
	//������Ͽ����������Ӻ󴥷����ص����Զ����ͽ�������.
	//���Ծ���Ҫ��������Ľ���������ڱ��ص��.
	if(comapi != 0)
	{
		comapi->ok_spotusd_btc_ticker();
	}
};
void com_callbak_close()
{
	std::cout << "�����Ѿ��Ͽ��� " << std::endl;
};
void com_callbak_message(const char *message)
{
	std::cout << "Message: " << message << std::endl;
};

int main(int argc, char* argv[]) 
{	
	//ʵ����API.
	std::string cn_apiKey		= "f1c5d942-beab-448a-8996-4d502e42cd96";									//�뵽www.okcoin.cn���롣.
	std::string cn_secretKey		= "8E0C2C77443BB43C1034E016055DC545";								//�뵽www.okcoin.cn���롣.
	cnapi = new OKCoinWebSocketApiCn(cn_apiKey,cn_secretKey);			//����վ.
	cnapi->SetCallBackOpen(cn_callbak_open);
	cnapi->SetCallBackClose(cn_callbak_close);
	cnapi->SetCallBackMessage(cn_callbak_message);
	cnapi->Run();//�������ӷ������߳�.
	

	/*std::string com_apiKey		= "";									//�뵽www.okcoin.com���롣.
	std::string com_secretKey	= "";									//�뵽www.okcoin.com���롣.
	comapi = new OKCoinWebSocketApiCom(com_apiKey,com_secretKey);		//����վ.
	comapi->SetCallBackOpen(com_callbak_open);
	comapi->SetCallBackClose(com_callbak_close);
	comapi->SetCallBackMessage(com_callbak_message);
	comapi->Run();//�������ӷ������߳�.*/
	
	Sleep(3000);

	cout << "����1���ı��رҵ��ܺ�Լ���飬����2���ı��ر��ڻ�ָ��������3ȡ�����ģ�����4�ر����ӣ�������:\n";
	string i;
	cin >> i;
	if(i == "1")
	{
		comapi->ok_futureusd_btc_ticker_this_week();
	}
	
	if(i == "2")
	{
		comapi->ok_futureusd_btc_ticker_this_week();
	}

	//�����Ѿ�ע��������ڲ���Ҫʱע���������ٽ��գ�.
	//ע��̫��Ǳ�Ҫ���ݣ���ʹ���ĳ������������½����뿪����ע�⴦��.
	if(i == "3")
	{
		if(comapi != 0)
		{
			comapi->remove_ok_spotusd_btc_ticker();
		}
		system("pause");
	}

	//�ر�����.
	//cnapi->Close();	.
	comapi->Close();

	system("pause");

	//�ͷ�APIʵ��.
	//delete cnapi;.
	delete comapi;
	cout << "�ͷ�APIʵ�����.";

	system("pause");
}
