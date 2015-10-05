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

	populateAmount(this->borrow, borrow);
	populateAmount(this->free, free);
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

	std::cout << "\t" << "Borrow" << std::endl;
	std::cout << "\t\t" << "BTC: " << borrow.btc << std::endl;
	std::cout << "\t\t" << "CNY: " << borrow.cny << std::endl;
	std::cout << "\t\t" << "LTC: " << borrow.ltc << std::endl;
	
	std::cout << "\t" << "Free" << std::endl;
	std::cout << "\t\t" << "BTC: " << free.btc << std::endl;
	std::cout << "\t\t" << "CNY: " << free.cny << std::endl;
	std::cout << "\t\t" << "LTC: " << free.ltc << std::endl;

	std::cout << "\t" << "Freezed" << std::endl;
	std::cout << "\t\t" << "BTC: " << freezed.btc << std::endl;
	std::cout << "\t\t" << "CNY: " << freezed.cny << std::endl;
	std::cout << "\t\t" << "LTC: " << freezed.ltc << std::endl;

	std::cout << "]" << std::endl;
}
