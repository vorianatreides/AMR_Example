/*
 * OrderReader.cpp
 *
 */

#include "OrderReader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <future>
#include <mutex>
#include <iostream>

static std::mutex s_value_mutex;
static std::vector<std::future<int>> s_futures;

static OrderExitCode readOrdersFromFile(const std::string& filename, ordersmap& orders)
{
  std::ifstream read_yaml(filename);
  if(!read_yaml.is_open())
    return order_invalid_filename;

  uint32_t id = 0;

  std::pair<ordersmap::iterator,bool> order_it;
  double cx = 0.0;
  double cy = 0.0;

  while(!read_yaml.eof())
    {
      std::string line;
      std::getline(read_yaml,line);
      if(!line.size())
        break;
      int current_level = 0;
      for(auto c : line)
        {
          if(c != ' ')
            break;
          ++current_level;
        }
      current_level /= 2;

      std::stringstream ss(line);
      std::string token;
      while(ss >> token)
	{
	  // To lower case:
	  std::transform(token.begin(), token.end(), token.begin(),
	                 [](unsigned char c){ return std::tolower(c); });
	  switch(current_level)
	  {
	    case 0:
	      if(token.find("order:") != std::string::npos)
		{
		  ss >> token;
		  if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
		    return order_invalid_id;
		  id = std::stoi(token);
		}
	      break;
	    case 1:
	      if(token.find("cx:") != std::string::npos)
		{
		  ss >> token;
		  if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
		    return order_invalid_format;
		  cx = std::stod(token);
		}
	      if(token.find("cy:") != std::string::npos)
		{
		  ss >> token;
		  if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
		    return order_invalid_format;
		  cy = std::stod(token);
		  order_it = orders.emplace(id,Order(id,Position(cx,cy)));
		}
	      if(token.find("-") != std::string::npos)
		{
		  ss >> token;
		  if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
		    return order_invalid_format;
		  order_it.first->second.emplace_backProductID(std::stoi(token));
		}
	      break;
	    default: return order_invalid_format;
	    break;
	  }
	}
    }
  return order_ok;
}

static int loadOrdersMap(std::vector<ordersmap>* all_orders, const std::string filename)
{
  ordersmap orders;
  int return_value = readOrdersFromFile(filename,orders);
  if(return_value)
    return return_value;
  std::lock_guard<std::mutex> lock(s_value_mutex);
  all_orders->emplace_back(orders);
  return 0;
}

int readOrders(const std::vector<std::string>& filenames, std::vector<ordersmap>& all_orders)
{
  if(filenames.size() == 0)
    {
      std::cout << "Empty directory while trying to read orders!\n";
      return order_empty_directory;
    }
  s_futures.reserve(filenames.size());
  int order_code = order_ok;
  for(const auto filename : filenames)
    {
      s_futures.emplace_back(std::async(std::launch::async, loadOrdersMap, &all_orders, filename));
      order_code = s_futures.back().get();
      switch(order_code)
      {
	case order_ok:
	  break;
	case order_invalid_filename:
	  std::cout << "File at location: \'" << filename << "\' could not be opened!\n";
	  return order_code;
	  break;
	case order_invalid_id:
	  std::cout << "File at location: \'" << filename << "\' contains an invalid order ID!\n";
	  return order_code;
	  break;
	case order_invalid_format:
	  std::cout << "File at location: \'" << filename << "\' has an invalid format!\n";
	  return order_code;
	  break;
	default:
	  std::cout << "An unknown error occurred while reading file at location: \'" << filename << "\'\n";
	  return order_code;
	  break;
      }
    }
  return order_code;
}
