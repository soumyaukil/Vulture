#ifndef UserSetting_H
#define UserSetting_H

#include <string>
#include <vector>
#include <jansson.h>

class UserSetting
{
    struct ApiAuthKeys
    {
        std::string _apiKey;
        std::string _secretKey;
    };
    typedef ApiAuthKeys OKCoinChinaKeys;

    public:
	bool initialize(json_t *root);
	bool isVerbosed(){return _verbose;}
	ApiAuthKeys getKey(){return _okCoinKeys;}
	std::vector<double> getPriceLevel(){return _priceLevel;}
	double getWaitTime(){return _waitTime;}
	double getWaitTimeForExitTrade(){return _waitTimeForExitTrade;}
	unsigned getMaxLossAmount(){return _maxLossAmount;}
	std::string getSymbol(){return _symbol;}
	std::string getVolumn(){return _volumn;}

    private:
	OKCoinChinaKeys     _okCoinKeys;
	bool                _verbose;
	std::vector<double> _priceLevel;
	double              _waitTime;
	double		    _waitTimeForExitTrade;
	unsigned            _maxLossAmount;

	std::string         _symbol;
	std::string         _volumn;
};

#endif

