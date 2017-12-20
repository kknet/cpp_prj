#ifndef EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_
#define EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_

#include "common/Global.h"

namespace itstation {
namespace marketapi {

class TradeApi;

class MARKET_TRADE_API MarketTradeApiFactory
{
public:
	static const std::string kCtpFutureApi; // CTP
	static const std::string kJZStockApi; // ��֤
	static const std::string kHSStockApi; // ����
	static const std::string kJSDStockApi; // ���˴�
	static const std::string kBackTestApi; // �ز�

	static TradeApi* CreateMarketTradeApi(std::string kApi);
};

}
}

#endif	//EYEGLE_MARKETAPI_MARKETRADEAPI_FACTORY_H_

