/*
 * OrderReader.hpp
 *
 */

#ifndef ORDERREADER_HPP_
#define ORDERREADER_HPP_

#include "Order.hpp"
#include <map>
#include <string>

enum OrderExitCode {order_ok, order_invalid_filename, order_invalid_id, order_invalid_format, order_empty_directory};
typedef std::map<uint32_t,Order> ordersmap;

int readOrders(const std::vector<std::string>& filenames, std::vector<ordersmap>& all_orders);

#endif /* ORDERREADER_HPP_ */
