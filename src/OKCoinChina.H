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
		bool sendOrder(const std::string &symbol,const std::string &volumn,const std::string &direction, double price,std::string &error, int &orderId);
		bool sendBatchOrder(const std::string&,const std::string&,const std::string&,std::vector<double>&,int,std::string&,std::vector<int>&);
		bool userInfo(std::string &error, UserInfo &userInfor);
		bool getOrderInfo(int orderId, const std::string &symbol,std::string &error, OrderInfo &orderInfo);
		bool cancelOrder(int orderId, const std::string &symbol,std::string &error);
		bool initialize(UserSetting&);
		~OKCoinChina();
	private:
		json_t* executeCall(std::string,std::string);

		std::string _apiKey;
		std::string _secretKey;
		void populateBestBidAndLowestAsk();
		void populateBestBidAndLowestAskPeriodicallyWithDelay(double);
		void populateError(int,std::string&);
		template<typename T>
		std::string convertToType(T value);
		double bid;
		double ask;
		std::thread t1;
};

#endif
