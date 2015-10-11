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
		std::cerr << "OKCoinChina failed to initialized. Quiting." << std::endl;
		return false;
	}
	if(!okCoinChina.userInfo(errorMessage, userInfo))
	{
		std::cerr << "Error occured. Message: " << errorMessage << std::endl;
		return false;
	}
	if(userSettings.isVerbosed())
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
		if(userSettings.isVerbosed())
		{
			std::cout << "Best   Bid: " << bid << std::endl;
			std::cout << "Lowest Ask: " << ask << std::endl;
		}

		//Send all the initial bids and asks request.
		sendOrdersInBatch(bid,ask,userSettings.getSymbol(), userSettings.getVolumn(),userSettings.getPriceLevel(),userSettings.getWaitTime());

		/*
		 * Find any of bids has been fully filled or not.
		 */
		for(i=0;i<bidOrderIDs.size();++i)
		{
			OrderInfo orderInfo;
			if(userSettings.isVerbosed())
				std::cout << "Checking buy order. orderID: " << bidOrderIDs[i] << " ";
			if(okCoinChina.getOrderInfo(bidOrderIDs[i], userSettings.getSymbol(), errorMessage, orderInfo))
			{
				if(userSettings.isVerbosed())
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
				if(userSettings.isVerbosed())
					std::cout << "Cancelling buy order, ID: " << bidOrderIDs[i] << std::endl;
				if(!okCoinChina.cancelOrder(bidOrderIDs[i], userSettings.getSymbol(), errorMessage))
					std::cerr << errorMessage;
			}
			for(i=0;i<askOrderIDs.size();++i)
			{
				if(userSettings.isVerbosed())
					std::cout << "Cancelling ask order, ID: " << askOrderIDs[i] << std::endl;
				if(!okCoinChina.cancelOrder(askOrderIDs[i], userSettings.getSymbol(), errorMessage))
					std::cerr << errorMessage;
			}
			bidOrderIDs.clear();
			askOrderIDs.clear();
			continue;
		}
		//At least one bid has been fully filled.
		else
		{
			if(userSettings.isVerbosed())
				std::cout << "Cancelling all ask order since there is at least one fully filled bid request." << std::endl;
			for(int j=0;j<askOrderIDs.size();++j)
			{
				if(!okCoinChina.cancelOrder(askOrderIDs[j], userSettings.getSymbol(), errorMessage))
					std::cerr << "Cancelling ask order failed. OrderID: " << askOrderIDs[j] << ". Message: " << errorMessage << std::endl;
			}
			if(userSettings.isVerbosed())
				std::cout << "Cancelling all bid order since there is at least one fully filled bid request." << std::endl;
			for(int j=0;j<bidOrderIDs.size();++j)
			{
				if(j != i)
				{
					if(!okCoinChina.cancelOrder(bidOrderIDs[i], userSettings.getSymbol(), errorMessage))
						std::cerr << "Cancelling bid order failed. OrderID: " << bidOrderIDs[j] << ". Message: " << errorMessage;
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

	for(int i=0;i<priceLevel.size();i+=5)
	{
		int j;
		std::vector<double> priceList;

		for(j=i;j < priceLevel.size() && j < (5 + i); ++j)
			priceList.push_back(bid - priceLevel[j]);
		if(!okCoinChina.sendBatchOrder(symbol, volumn,"buy",priceList, j, errorMessage, bidOrderIDs))
		{
			std::cerr << "Error occured while placing buy order. Message: " << errorMessage << std::endl;
		}
	}
	if(userSettings.isVerbosed())
	{
		std::cout << "Placed buy order.  IDs: [";
		for(int j=0;j<bidOrderIDs.size();++j)
		{
			std::cout << bidOrderIDs[j];
			if(j < bidOrderIDs.size() - 1)
				std::cout << ",";
		}
		std::cout << "]" << std::endl;
	}
	for(int i=0;i<priceLevel.size();i+=5)
	{
		int j;
		std::vector<double> priceList;

		for(j=i;j < priceLevel.size() && j < (5 + i); ++j)
			priceList.push_back(ask + priceLevel[j]);

		if(!okCoinChina.sendBatchOrder(symbol, volumn, "sell", priceList, j, errorMessage, askOrderIDs))
		{
			std::cerr << "Error occured while placing ask order. Message: " << errorMessage << std::endl;
		}
	}
	if(userSettings.isVerbosed())
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
