#include "UserSetting.H"
#include <iostream>

bool UserSetting::initialize(json_t *root)
{
	_verbose = json_boolean_value(json_object_get(root, "Verbose"));
	_waitTime = json_real_value(json_object_get(root, "WaitTimeBetweenTrades"));
	_waitTimeForExitTrade = json_real_value(json_object_get(root, "WaitTimeForExitTrade"));
	_tickerDataQueryTime = json_real_value(json_object_get(root, "TickerDataQueryTime"));
	json_t *array = json_object_get(root, "PriceLevel");
	int size = json_array_size(array);
	for(int i=0;i<size;++i)
		_priceLevel.push_back(json_real_value(json_array_get(array, i)));
	_maxLossAmount = json_integer_value(json_object_get(root, "MaxLossAmount"));
	_okCoinKeys._apiKey = json_string_value(json_object_get(root, "OkCoinChinaApiKey"));
	_okCoinKeys._secretKey = json_string_value(json_object_get(root, "OkCoinChinaSecretKey"));
	
	_symbol = json_string_value(json_object_get(root, "symbol"));
	_volumn = json_string_value(json_object_get(root, "volumn"));
	return true;
}
