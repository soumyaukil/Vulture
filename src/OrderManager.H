#ifndef OrderManager_H
#define OrderManager_H

#include "OKCoinChina.H"
#include "UserSetting.H"
#include <vector>

class OrderManager
{
	public:
		OrderManager(OKCoinChina&,UserSetting&);
		bool initialize();
		void start();
	private:
		void sendOrdersInBatch(const double &bid,const double &ask, const std::string &symbol, const std::string &volumn,const std::vector<double> &priceLevel,const double &waitTime);
		OKCoinChina &okCoinChina;
		UserSetting &userSettings;
		std::vector<int> bidOrderIDs, askOrderIDs;
};

#endif
