#ifndef OKCoinChina_H
#define OKCoinChina_H

#include <curl/curl.h>
#include <string>
#include <thread>
#include "UserInfo.H"
#include "OrderInfo.H"
#include "UserSetting.H"

class OKCoinChina
{
	public:
		double getBid(){return bid;}
		double getAsk(){return ask;}
		bool sendOrder(UserSetting &userSetting, std::string direction, double price,std::string &error,int&);
		bool userInfo(UserSetting &userSetting, std::string &error, UserInfo &userInfor);
		bool getOrderInfo(UserSetting &userSetting, int orderId, std::string &error, OrderInfo &orderInfo);
		bool initialize(UserSetting&);
		~OKCoinChina();
	private:
		void populateBestBidAndLowestAsk();
		void populateBestBidAndLowestAskPeriodicallyWithDelay(double);
		void populateError(int,std::string&);
		template<typename T>
		std::string convertToType(T value);
		CURL *curl;
		double bid;
		double ask;
		std::thread t1;
};

#endif
