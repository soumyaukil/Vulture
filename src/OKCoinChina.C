#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sstream>
#include <sys/time.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <string>
#include <ctype.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include "base64.H"
#include <jansson.h>
#include "curl_fun.H"
#include "OKCoinChina.H"
#include "md5.H"


template<typename T>
std::string OKCoinChina::convertToType(T value) 
{
	std::ostringstream o;
	if (!(o << value))
		return "";
	return o.str();
}

OKCoinChina::~OKCoinChina()
{
	t1.join();
}

json_t* OKCoinChina::executeCall(std::string url,std::string postData)
{
	CURL *curl = curl_easy_init();
	return getJsonFromUrl(curl,url,postData);
}

bool OKCoinChina::initialize(UserSetting &userSetting)
{
	_apiKey = userSetting.getKey()._apiKey;
	_secretKey = userSetting.getKey()._secretKey;
	curl_global_init(CURL_GLOBAL_ALL);
	populateBestBidAndLowestAsk();
	sleep(userSetting.getTickerDataQueryTime());
	t1 = std::thread(&OKCoinChina::populateBestBidAndLowestAskPeriodicallyWithDelay, this, userSetting.getTickerDataQueryTime());
	return true;
}

void OKCoinChina::populateBestBidAndLowestAskPeriodicallyWithDelay(double delay)
{
	while(1)
	{
		populateBestBidAndLowestAsk();
		sleep(delay);
	}
}

void OKCoinChina::populateBestBidAndLowestAsk()
{
	json_t *root = executeCall("https://www.okcoin.cn/api/v1/ticker.do", "");
	if(root)
	{
		bid = atof(json_string_value(json_object_get(json_object_get(root, "ticker"), "buy")));
		ask = atof(json_string_value(json_object_get(json_object_get(root, "ticker"), "sell")));
	}
}

bool OKCoinChina::userInfo(std::string &error, UserInfo &userInfo)
{
	std::string url = "https://www.okcoin.cn/api/v1/userinfo.do?";
	std::string params = "api_key=" + _apiKey;
	params += "&secret_key=" + _secretKey;
	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;

	json_t *root = executeCall(url + params, url + params);
	bool result = json_boolean_value(json_object_get(root, "result"));
	if(result)
	{
		userInfo.populateData(root);
		return true;
	}
	else
	{
		int errorCode = json_integer_value(json_object_get(root, "error_code"));
		populateError(errorCode, error);
	}
	return false;
}

bool OKCoinChina::cancelOrder(int orderId, const std::string &symbol,std::string &error)
{
	std::string url = "https://www.okcoin.cn/api/v1/cancel_order.do?";
	std::string params = "api_key=" + _apiKey +
			     "&order_id=" + convertToType(orderId) +
			     "&symbol=" + symbol;
	params += "&secret_key=" + _secretKey;
	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;

	json_t *root = executeCall(url + params, params);
	bool result = json_boolean_value(json_object_get(root, "result"));
	if(result)
	{
		return true;
	}
	else
	{
		int errorCode = json_integer_value(json_object_get(root, "error_code"));
		populateError(errorCode, error);
	}
	return false;
}

bool OKCoinChina::getOrderInfo(int orderId, const std::string &symbol,std::string &error, OrderInfo &orderInfo)
{
	std::string url = "https://www.okcoin.cn/api/v1/order_info.do?";
	std::string params = "api_key=" + _apiKey + 
                             "&order_id=" + convertToType(orderId) + 
			     "&symbol=" + symbol;
	params += "&secret_key=" + _secretKey;
	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;

	json_t *root = executeCall(url + params, params);
	bool result = json_boolean_value(json_object_get(root, "result"));
	if(result)
	{
		orderInfo.populateData(root);
		return true;
	}
	else
	{
		int errorCode = json_integer_value(json_object_get(root, "error_code"));
		populateError(errorCode, error);
	}
	return false;
}

bool OKCoinChina::sendOrder(const std::string &symbol,const std::string &volumn,const std::string &direction, double price,std::string &error, int &orderId)
{
	std::string url = "https://www.okcoin.cn/api/v1/trade.do?";
	std::string params = "amount=" + volumn + 
			     "&api_key=" + _apiKey + 
			     "&price=" + convertToType(price) + 
			     "&symbol=" + symbol + 
			     "&type=" + direction;
	params += "&secret_key=" + _secretKey;

	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;
	//std::cout << "URL: " << url + params << std::endl;
	json_t *root = executeCall(url + params, params);
	bool result = json_boolean_value(json_object_get(root, "result"));
	if(result)
	{
		orderId = json_integer_value(json_object_get(root, "order_id"));
		return true;
	}
	else
	{
		int errorCode = json_integer_value(json_object_get(root, "error_code"));
		populateError(errorCode, error);
	}

	return false;
}

bool OKCoinChina::sendBatchOrder(const std::string &symbol,const std::string &volumn,const std::string &direction, std::vector<double> &price,int count,std::string &error, std::vector<int> &orderIds)
{
	std::string orders_data = "[";

	for(int i=0;i<count;++i)
	{
		std::string individual_data = "{";
		individual_data = individual_data + "price:" + convertToType(price[i]) + "," + 
						    "amount:" + volumn + "," + 
						    "type:'" + direction + "'";
		individual_data = individual_data + "}";
		if(i < (count - 1))
			individual_data += ",";
		orders_data += individual_data;
	}
	orders_data += "]";
	std::string url = "https://www.okcoin.cn/api/v1/batch_trade.do?";
	std::string params = 
		"api_key=" + _apiKey +
		"&orders_data=" + orders_data + 
		"&symbol=" + symbol + 
		"&type=" + direction;
	params += "&secret_key=" + _secretKey;

	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;
	//std::cout << "URL: " << url + params << std::endl;
	json_t *root = executeCall(url + params, params);
	bool result = json_boolean_value(json_object_get(root, "result"));
	if(result)
	{
		int orderId;
		json_t *array = json_object_get(root, "order_info");
		int size = json_array_size(array);
		for(int i=0;i<size;++i)
		{
			orderId = json_integer_value(json_object_get(json_array_get(array, i), "order_id"));
			if(orderId != -1)
				orderIds.push_back(orderId);
			else
			{
				int errorCode = json_integer_value(json_object_get(json_array_get(array, i), "error_code"));
				populateError(errorCode, error);
				return false;
			}
		}
		return true;
	}
	else
	{
		int errorCode = json_integer_value(json_object_get(root, "error_code"));
		populateError(errorCode, error);
	}
	return false;
}


void OKCoinChina::populateError(int errorCode,std::string &error_)
{
	switch(errorCode)
	{
		case 10000:
			error_ = "Required parameter can not be null";
			break;
		case 10001:
			error_ = "Requests are too frequent";
			break;
		case 10002:
			error_ = "System Error'";
			break;
		case 10003:
			error_ = "Restricted list request, please try again later";
			break;
		case 10004:
			error_ = "IP restriction";
			break;
		case 10005:
			error_ = "Key does not exist";
			break;
		case 10006:
			error_ = "User does not exist";
			break;
		case 10007:
			error_ = "Signatures do not match";
			break;
		case 10008:
			error_ = "Illegal parameter";
			break;
		case 10009:
			error_ = "Order does not exist";
			break;
		case 10010:
			error_ = "Insufficient balance";
			break;
		case 10011:
			error_ = "Order is less than minimum trade amount";
			break;
		case 10012:
			error_ = "Unsupported symbol (not btc_cny or ltc_cny)";
			break;
		case 10013:
			error_ = "This interface only accepts https requests";
			break;
		default:
			error_ = "Generic error";
			break;
	}
}



