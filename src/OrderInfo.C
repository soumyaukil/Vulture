#include "OrderInfo.H"
#include <iostream>

bool OrderInfo::populateData(json_t *root)
{
	json_t *orderArray = json_object_get(root, "orders");
	json_t *firstElement = json_array_get(orderArray, 0);


	volumn = json_real_value(json_object_get(firstElement, "amount"));
	avg_price = json_real_value(json_object_get(firstElement, "avg_price"));
	create_date = json_integer_value(json_object_get(firstElement, "create_date"));
	deal_amount = json_real_value(json_object_get(firstElement, "deal_amount"));
	order_id = json_integer_value(json_object_get(firstElement, "order_id"));
	price = json_real_value(json_object_get(firstElement, "price"));
	execStatus = json_integer_value(json_object_get(firstElement, "status"));
	symbol = json_string_value(json_object_get(firstElement, "symbol"));
	type = json_string_value(json_object_get(firstElement, "type"));
	return true;
}

void OrderInfo::print()
{
}
