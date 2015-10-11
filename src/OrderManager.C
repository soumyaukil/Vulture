#include "OrderManager.H"
#include <iostream>

OrderManager::OrderManager(OKCoinChina &okCoinChina_,UserSetting &userSettings_):okCoinChina(okCoinChina_),userSettings(userSettings_)
{
}

bool OrderManager::initialize()
{
	UserInfo userInfo;
	std::string errorMessage;

	if(!okCoinChina.initialize(userSettings))
	{
		std::cout << "OKCoinChina failed to initialized. Quiting." << std::endl;
		return false;
	}
	if(!okCoinChina.userInfo(errorMessage, userInfo))
	{
		std::cout << "Error occured. Message: " << errorMessage << std::endl;
		return false;
	}
	userInfo.print();
	return true;
}

void OrderManager::start()
{
	std::string errorMessage;
	while(1)
	{
		int i=0;
		//Get the best bid and lowest ask from market
		double bid = okCoinChina.getBid();
		double ask = okCoinChina.getAsk();
		std::cout << bid << " " << ask << std::endl;

		//Send all the initial bids and asks request.
		sendOrdersInBatch(bid,ask,userSettings.getSymbol(), userSettings.getVolumn(),userSettings.getPriceLevel(),userSettings.getWaitTime());

		/*
		 * Find any of bids has been fully filled or not.
		 */
		for(i=0;i<bidOrderIDs.size();++i)
		{
			OrderInfo orderInfo;
			std::cout << "Checking orderID: " << bidOrderIDs[i] << " ";
			if(okCoinChina.getOrderInfo(bidOrderIDs[i], userSettings.getSymbol(), errorMessage, orderInfo))
			{
				std::cout << "execInfo: " << orderInfo.getExecStatus() << std::endl;
				if(orderInfo.getExecStatus() == 2)
					break;
			}
		}
		//No bid has been fully filled. So cancel all the initial bids and asks, and restart the process.
		if(i == bidOrderIDs.size())
		{
			for(i=0;i<bidOrderIDs.size();++i)
			{
				std::cout << "Cancelling order, ID: " << bidOrderIDs[i] << std::endl;
				if(!okCoinChina.cancelOrder(bidOrderIDs[i], userSettings.getSymbol(), errorMessage))
					std::cout << errorMessage;
			}
			for(i=0;i<askOrderIDs.size();++i)
			{
			std::cout << "Cancelling order, ID: " << askOrderIDs[i] << std::endl;
				if(!okCoinChina.cancelOrder(askOrderIDs[i], userSettings.getSymbol(), errorMessage))
					std::cout << errorMessage;
			}
			bidOrderIDs.clear();
			askOrderIDs.clear();
			continue;
		}
		//At least one bid has been fully filled.
		else
		{
			for(int j=0;j<askOrderIDs.size();++j)
			{
				if(!okCoinChina.cancelOrder(askOrderIDs[j], userSettings.getSymbol(), errorMessage))
					std::cout << errorMessage;
			}
			for(int j=0;j<bidOrderIDs.size();++j)
			{
				if(j != i)
				{
					if(!okCoinChina.cancelOrder(bidOrderIDs[i], userSettings.getSymbol(), errorMessage))
						std::cout << errorMessage;
					sleep(1);
				}
			}
		}
		bidOrderIDs.clear();
		askOrderIDs.clear();
	}
}

void OrderManager::sendOrdersInBatch(const double &bid,const double &ask,const std::string &symbol, const std::string &volumn,const std::vector<double> &priceLevel,const double &waitTime)
{
	std::string errorMessage;
	int orderId;

	for(int i=0;i<priceLevel.size();i+=5)
	{
		int j;
		std::vector<double> priceList;

		for(j=0;j < priceLevel.size() && j < 5; ++j)
			priceList.push_back(bid - priceLevel[j]);
		if(!okCoinChina.sendBatchOrder(symbol, volumn,"buy",priceList, j, errorMessage, bidOrderIDs))
		{
			std::cout << "Error occured while placing buy order. Message: " << errorMessage << std::endl;
		}
		else
		{
			std::cout << "Placed buy order. IDs: [";
			for(int j=0;j<bidOrderIDs.size();++j)
			{
				std::cout << bidOrderIDs[j];
				if(j < bidOrderIDs.size() - 1)
					std::cout << ",";
			}
			std::cout << "]" << std::endl;
		}
	}
	for(int i=0;i<priceLevel.size();i+=5)
	{
		int j;
		std::vector<double> priceList;

		for(j=0;j < priceLevel.size() && j < 5; ++j)
			priceList.push_back(ask + priceLevel[j]);

		if(!okCoinChina.sendBatchOrder(symbol, volumn, "sell", priceList, j, errorMessage, askOrderIDs))
		{
			std::cout << "Error occured while placing ask order. Message: " << errorMessage << std::endl;
		}
		else
		{
			std::cout << "Placed sell order. IDs: [";
			for(int j=0;j<askOrderIDs.size();++j)
			{
				std::cout << askOrderIDs[j];
				if(j < askOrderIDs.size() - 1)
					std::cout << ",";
			}
			std::cout << "]" << std::endl;
		}
	}
}
