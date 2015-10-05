#ifndef OKCoinChina_H
#define OKCoinChina_H

#include <curl/curl.h>
#include <string>
#include "UserInfo.H"
#include "OrderInfo.H"
#include "UserSetting.H"

class OKCoinChina
{
	public:
		void getBestBidAndAsk(CURL* curl,double&,double&);
		bool sendOrder(CURL *curl, UserSetting &userSetting, std::string direction, double price,std::string &error,int&);
		bool userInfo(CURL *curl, UserSetting &userSetting, std::string &error, UserInfo &userInfor);
		bool getOrderInfo(CURL *curl, UserSetting &userSetting, int orderId, std::string &error, OrderInfo &orderInfo);
	private:
		void populateError(int,std::string&);
		template<typename T>
		std::string convertToType(T value);
};

#endif
