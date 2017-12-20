
#ifndef __OKCOINWEBSOCKETAPI_H__
#define __OKCOINWEBSOCKETAPI_H__
#include <string>

using namespace std;

#include "websocket.h"

#define URL_CN						"wss://real.okcoin.cn:10440/websocket/okcoinapi"
#define URL_COM						"wss://real.okcoin.com:10440/websocket/okcoinapi"

//#define MAX_RETRY_COUNT		3000.

class OKCoinWebSocketApi
{
protected:
	OKCoinWebSocketApi();
	void SetKey(string api_key,string secret_key);
	void SetUri(string uri);
	string m_api_key;			//�û������apiKey.
	string m_secret_key;		//�������ǩ����˽Կ.
	string m_uri;

	void Emit(const char *channel,string &parameter);
	void Emit(const char *channel);
	void Emit(string &channel);
	void Remove(string channel);
private:
	websocketpp_callbak_open		m_callbak_open;
	websocketpp_callbak_close		m_callbak_close;
	websocketpp_callbak_message		m_callbak_message;

public:
	virtual ~OKCoinWebSocketApi();
	void Run();
	void Close();
	static unsigned __stdcall OKCoinWebSocketApi::RunThread( LPVOID arg );
	WebSocket *pWebsocket;
	HANDLE hThread;

	void SetCallBackOpen(websocketpp_callbak_open callbak_open);
	void SetCallBackClose(websocketpp_callbak_close callbak_close);
	void SetCallBackMessage(websocketpp_callbak_message callbak_message);
};

class OKCoinWebSocketApiCn : public OKCoinWebSocketApi //����վ..
{
public:
	OKCoinWebSocketApiCn(string api_key,string secret_key)
	{
		SetKey(api_key,secret_key);
		SetUri(URL_CN); //����վ.
	};
	~OKCoinWebSocketApiCn(){};

	//��ȡOKCoin�ֻ���������.
	void ok_spotcny_btc_ticker();				//���ر���������.
	void ok_spotcny_btc_depth_20();				//���ر�20���г����.
	void ok_spotcny_btc_trades();				//���رҳɽ���¼.
	void ok_spotcny_btc_kline_1min();			//���ر�һ����K������.

	//��OKCoin���н���.
	void ok_spotcny_trades();				//ʵʱ��������.
	void ok_spotcny_trade(string &symbol,string &type,string &price,string &amount);	//�µ�����.
	void ok_spotcny_cancel_order(string &symbol,string &order_id);				//ȡ������.

	//ע������.
	void remove_ok_spotcny_btc_ticker();			//���ر���������.
};


class OKCoinWebSocketApiCom:public OKCoinWebSocketApi //����վ.
{
public:
	OKCoinWebSocketApiCom(string api_key,string secret_key)
	{
		SetKey(api_key, secret_key);
		SetUri(URL_COM); //����վ.
	};
	~OKCoinWebSocketApiCom(){};

	//��ȡOKCoin�ֻ���������.
	void ok_spotusd_btc_ticker();				//���ر���������.

	//��ȡOKCoin��Լ��������.
	void ok_futureusd_btc_ticker_this_week();		//���رҵ��ܺ�Լ����.
	void ok_futureusd_btc_index();				//���رҺ�Լָ��.
	
	//ע������.
	void remove_ok_spotusd_btc_ticker();			//���ر���������.
};

#endif /* __OKCOINWEBSOCKETAPI_H__ */
