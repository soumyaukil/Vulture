#ifndef UserInfo_H
#define UserInfo_H

#include <jansson.h>

class UserInfo
{
	private:
		struct Amount
		{
			double btc;
			double cny;
			double ltc;

			Amount():btc(-1),cny(-1),ltc(-1){}
		};

	public:
		bool populateData(json_t *root);
		Amount getBorrowFunds() {return borrow;}
		Amount getFreeFunds() {return free;}
		Amount getFreezedFunds() {return freezed;}
		double getNetAsset() {return netAsset;}
		double getTotalAsset(){return totalAsset;}
		void print();

	private:
		void populateAmount(Amount&,json_t*);
	private:
		Amount borrow;
		Amount free;
		Amount freezed;
		double netAsset;
		double totalAsset;
};

#endif
