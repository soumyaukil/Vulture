#include "UserInfo.H"
#include <iostream>

bool UserInfo::populateData(json_t *root)
{
	json_t *info = json_object_get(root, "info");
	json_t *funds = json_object_get(info, "funds");
	json_t *asset = json_object_get(funds, "asset");
	json_t *borrow = json_object_get(funds, "borrow");
	json_t *free = json_object_get(funds, "free");
	json_t *freezed = json_object_get(funds, "freezed");

	if(borrow != NULL)
		populateAmount(this->borrow, borrow);
	if(free != NULL)
		populateAmount(this->free, free);
	if(freezed != NULL)
		populateAmount(this->freezed, freezed);

	netAsset = atof(json_string_value(json_object_get(asset, "net")));
	totalAsset = atof(json_string_value(json_object_get(asset, "total")));
	return true;
}

void UserInfo::populateAmount(Amount &amount,json_t *root)
{
	amount.btc = atof(json_string_value(json_object_get(root, "btc")));
	amount.cny = atof(json_string_value(json_object_get(root, "cny")));
	amount.ltc = atof(json_string_value(json_object_get(root, "ltc")));
}

void UserInfo::print()
{
	std::cout << std::endl << "User Information" << std::endl;
	std::cout << "[" << std::endl;
	std::cout << "\t" << "NetAsset: " << netAsset << std::endl;
	std::cout << "\t" << "TotalAsset: " << totalAsset  << std::endl;

	if((borrow.btc != -1) || (borrow.cny != -1) || (borrow.ltc != -1))
	{
		std::cout << "\t" << "Borrow" << std::endl;
		if(borrow.btc != -1)
			std::cout << "\t\t" << "BTC: " << borrow.btc << std::endl;
		if(borrow.cny != -1)
			std::cout << "\t\t" << "CNY: " << borrow.cny << std::endl;
		if(borrow.ltc != -1)
			std::cout << "\t\t" << "LTC: " << borrow.ltc << std::endl;
	}
	if((free.btc != -1) || (free.cny != -1) || (free.ltc != -1))
	{
		std::cout << "\t" << "Free" << std::endl;
		if(free.btc != -1)
			std::cout << "\t\t" << "BTC: " << free.btc << std::endl;
		if(free.cny != -1)
			std::cout << "\t\t" << "CNY: " << free.cny << std::endl;
		if(free.ltc != -1)
			std::cout << "\t\t" << "LTC: " << free.ltc << std::endl;
	}

	if((freezed.btc != -1) || (freezed.cny != -1) || (freezed.ltc != -1))
	{
		std::cout << "\t" << "Freezed" << std::endl;
		if(freezed.btc != -1)
			std::cout << "\t\t" << "BTC: " << freezed.btc << std::endl;
		if(freezed.cny != -1)
			std::cout << "\t\t" << "CNY: " << freezed.cny << std::endl;
		if(freezed.ltc != -1)
			std::cout << "\t\t" << "LTC: " << freezed.ltc << std::endl;
	}
	std::cout << "]" << std::endl;
}
