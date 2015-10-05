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

void OKCoinChina::getBestBidAndAsk(CURL* curl, double &bid, double &ask)
{
	double quote = 0.0;
	json_t *root = getJsonFromUrl(curl, "https://www.okcoin.cn/api/v1/ticker.do", "");
	if(root)
	{
		bid = atof(json_string_value(json_object_get(json_object_get(root, "ticker"), "buy")));
		ask = atof(json_string_value(json_object_get(json_object_get(root, "ticker"), "sell")));
	}
}

bool OKCoinChina::userInfo(CURL *curl, UserSetting &userSetting, std::string &error, UserInfo &userInfo)
{
	std::string url = "https://www.okcoin.cn/api/v1/userinfo.do?";
	std::string params = "api_key=" + userSetting.getKey()._apiKey;
	params += "&secret_key=" + userSetting.getKey()._secretKey;
	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;

	json_t *root = getJsonFromUrl(curl, url + params, params);
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

bool OKCoinChina::getOrderInfo(CURL *curl, UserSetting &userSetting, int orderId, std::string &error, OrderInfo &orderInfo)
{
	std::string url = "https://www.okcoin.cn/api/v1/order_info.do?";
	std::string params = "api_key=" + userSetting.getKey()._apiKey + 
                             "&order_id=" + convertToType(orderId) + 
			     "&symbol=" + userSetting.getSymbol();
	params += "&secret_key=" + userSetting.getKey()._secretKey;
	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;

	json_t *root = getJsonFromUrl(curl, url + params, params);
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

bool OKCoinChina::sendOrder(CURL *curl, UserSetting &userSetting, std::string direction, double price,std::string &error, int &orderId)
{
	std::string url = "https://www.okcoin.cn/api/v1/trade.do?";
	std::string params = "amount=" + userSetting.getVolumn() + 
			     "&api_key=" + userSetting.getKey()._apiKey + 
			     "&price=" + convertToType(price) + 
			     "&symbol=" + userSetting.getSymbol() + 
			     "&type=" + direction;
	params += "&secret_key=" + userSetting.getKey()._secretKey;

	std::string sign = md5::MD5String(params.c_str());
	transform(sign.begin(), sign.end(), sign.begin(), ::toupper);
	params += "&sign=" + sign;
	//std::cout << "URL: " << url + params << std::endl;
	json_t *root = getJsonFromUrl(curl, url + params, params);
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



