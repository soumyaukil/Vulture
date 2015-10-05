#include <fstream>
#include <iomanip>
#include <unistd.h>
#include <numeric>
#include <math.h>
#include <algorithm>
#include <jansson.h>
#include <curl/curl.h>
#include <iostream>

#include "UserSetting.H"
#include "base64.H"
#include "result.H"
#include "time_fun.H"
#include "curl_fun.H"
#include "parameters.H"
#include "send_email.H"
#include "OKCoinChina.H"

int main(int argc, char **argv) {

    std::cout << "Vulture Arbitrage\n" << std::endl;
    std::cout << "DISCLAIMER: USE THE SOFTWARE AT YOUR OWN RISK.\n" << std::endl;

    // read the config file (config.json)
    json_error_t error;
    json_t *root = json_load_file("config.json", 0, &error);
    UserSetting userSettings;
    UserInfo userInfo;
    OKCoinChina okCoinChina;
    std::vector<double> priceLevel;
    std::string errorMessage;
    double bid,ask;
    int orderId;

    std::vector<int> bidOrderIDs, askOrderIDs;

    if (!root) {
        std::cout << "ERROR: config.json incorrect (" << error.text << ")\n" << std::endl;
        return 1;
    }
    if(userSettings.initialize(root)) {
        std::cout << "Config values have been passed successfully" << std::endl;
    }
    // cURL
    CURL* curl;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    okCoinChina.getBestBidAndAsk(curl,bid,ask);

    std::cout << "BestBid: " << bid << std::endl;
    std::cout << "BestAsk: " << ask << std::endl;
    if(!okCoinChina.userInfo(curl, userSettings, errorMessage, userInfo))
    {
        std::cout << "Error occured. Message: " << errorMessage << std::endl;
        return -1;
    }
    userInfo.print();
    priceLevel = userSettings.getPriceLevel();
    for(int i=0;i<priceLevel.size();++i)
    {
        //std::cout << priceLevel[i] << " ";
        sleep(userSettings.getWaitTime());
	if(okCoinChina.sendOrder(curl, userSettings, "buy", bid - priceLevel[i], errorMessage, orderId))
	{
		std::cout << "Buy order placed. OrderID: " << orderId << std::endl;
		bidOrderIDs.push_back(orderId);
	}
	else
	{
		std::cout << "Error occured while placing buy order. Message: " << errorMessage << std::endl;
	}
	sleep(userSettings.getWaitTime());
	if(okCoinChina.sendOrder(curl, userSettings, "sell", ask - priceLevel[i], errorMessage, orderId))
	{
		std::cout << "Ask order placed. OrderID: " << orderId << std::endl;
		askOrderIDs.push_back(orderId);
	}
	else
	{
		std::cout << "Error occured while placing ask order. Message: " << errorMessage << std::endl;
	}
    }
    for(int i=0;i<bidOrderIDs.size();++i)
    {
	int execInfo;
	OrderInfo orderInfo;
	int exitOrderId;
	if(okCoinChina.getOrderInfo(curl, userSettings, bidOrderIDs[i], errorMessage, orderInfo))
	{
		if(execInfo == 2)
		{
			sleep(userSettings.getWaitTimeForExitTrade());
			okCoinChina.sendOrder(curl, userSettings, "sell", ask - priceLevel[i], errorMessage, exitOrderId);
			std::cout << "Ask order placed. OrderID: " << exitOrderId << std::endl;
		}
	}
	else
	{
		std::cout << "Error occured. Message: " << errorMessage << std::endl;
	}
    }
    return 0;
}
