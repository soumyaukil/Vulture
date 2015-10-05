#ifndef OrderInfo_H
#define OrderInfo_H

#include <jansson.h>
#include <string>

class OrderInfo
{
	public:
		bool populateData(json_t *root);
		double getVolumn() {return volumn;}
		double getAvgPrice() {return avg_price;}
		int getCreateDate() {return create_date;}
		double getDealAmount() {return deal_amount;}
		int getOrderID(){return order_id;}
		double getPrice(){return price;}
		int getExecStatus(){return execStatus;}
		std::string getSymbol(){return symbol;}
		std::string getType(){return type;}
		void print();

	private:
		double volumn;
		double avg_price;
		int create_date;
		double deal_amount;
		int order_id;
		double price;
		int execStatus;
		std::string symbol;
		std::string type;
};

#endif
