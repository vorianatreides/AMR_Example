#include <iostream>
#include <fstream>
#include <vector>
#include <future>
#include <mutex>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <cstdint>
#include "Position.hpp"
#include "Product.hpp"
#include "Order.hpp"
#include "ProductReader.hpp"
#include "OrderReader.hpp"
#include "WeightedGraph.hpp"
#include "AMR.hpp"
#include <thread>
#include <chrono>

#define DEFAULT_WAITING_TIME 5000 // default waiting time in milliseconds
#define EXIT_OK 0
#define EXIT_ERROR 1

enum TopicCode {topic_empty, topic_current_position, topic_new_order, topic_end, topic_default};

static bool s_typing = false;

static std::vector<std::string> getApplicationDirectories(const char*);
static std::string getConfigurationPath(const char*);
static std::vector<std::string> getOrdersFilenames(const char*);
static void typingInput(std::string*);
static TopicCode receiveTopic(unsigned);
static TopicCode executeOrders(std::vector<std::string>&, std::vector<ordersmap>&, productsmap&, AMR&, unsigned);
static int readOrdersDirectory(const char*, std::vector<std::string>&, std::vector<std::map<uint32_t,Order>>&);

int main(int argc, char** argv)
{
  if((argc == 1) || (argc > 3))
    {
      std::cout << "Usage: ./[executable] [path to directory containing files] [waiting time (optional)]\n";
      return EXIT_ERROR;
    }

  std::string pathname = static_cast<std::string>(argv[1]);
    if(pathname.back() != '/')
      pathname += "/"; // ONLY WORKS ON LINUX!!!

  std::vector<std::string> directories = getApplicationDirectories(argv[1]);
  std::string filename_products = "";
  std::string directory_orders = "";
  for(auto directory : directories)
    {
      std::string tmp = directory; // temporary string for comparison
      // To lower case:
      std::transform(tmp.begin(), tmp.end(), tmp.begin(),
                     [](unsigned char c){ return std::tolower(c); });
      if(tmp == "configuration/")
	{
	  tmp = pathname + directory;
	  filename_products = getConfigurationPath(tmp.c_str());
	}
      if(tmp == "orders/")
	{
	  directory_orders = pathname + directory;
	}
    }

  productsmap products;
  int product_code = readProducts(filename_products,products);

  switch(product_code)
  {
    case product_ok:
      break;
    case product_invalid_filename:
      std::cout << "File at location: \'" << filename_products << "\' could not be opened!\n";
      return EXIT_ERROR;
      break;
    case product_invalid_id:
      std::cout << "File at location: \'" << filename_products << "\' contains an invalid product ID!\n";
      return EXIT_ERROR;
      break;
    case product_invalid_format:
      std::cout << "File at location: \'" << filename_products << "\' has an invalid format!\n";
      return EXIT_ERROR;
      break;
    default:
      std::cout << "An unknown error occurred while reading file at location: \'" << filename_products << "\'\n";
      return EXIT_ERROR;
      break;
  }

  std::vector<ordersmap> all_orders;
  std::vector<std::string> filenames_orders = getOrdersFilenames(directory_orders.c_str());
  AMR amr;

  unsigned waiting_time = DEFAULT_WAITING_TIME;
  if(argc == 3)
    {
      std::string set_waiting_time = argv[2];
      if(std::find_if(set_waiting_time.begin(), set_waiting_time.end(), ::isdigit) == set_waiting_time.end())
	{
	  std::cout << "Usage: waiting time must contain a number! Setting to default value of 5s\n";
	  set_waiting_time = std::to_string(DEFAULT_WAITING_TIME);
	}
      waiting_time = std::stoul(set_waiting_time);
    }

  TopicCode tc = topic_empty;
  do
    {
      int order_code = readOrdersDirectory(directory_orders.c_str(), filenames_orders, all_orders);
      if(order_code)
	return EXIT_ERROR;
      tc = executeOrders(filenames_orders, all_orders, products, amr, waiting_time);
    } while(tc == topic_new_order);

  return EXIT_OK;
}

static std::vector<std::string> getApplicationDirectories(const char* absolute_path)
{
  DIR* dir;
  struct dirent* diread;
  std::vector<std::string> directories;
  if((dir = opendir(absolute_path)) != nullptr)
    {
      while((diread = readdir(dir)) != nullptr)
  	{
	  if(diread->d_type == DT_DIR)
	    directories.emplace_back(static_cast<std::string>(diread->d_name) + "/");
  	}
      closedir(dir);
    }
  else
    {
      perror("opendir");
      return directories;
    }

  directories.erase(std::find(directories.begin(),directories.end(),std::string("./")));
  directories.erase(std::find(directories.begin(),directories.end(),std::string("../")));
  std::sort(directories.begin(),directories.end()); // probably unnecessary
  return directories;
}

static std::string getConfigurationPath(const char* absolute_path)
{
  std::string configuration_path = absolute_path;
  if(configuration_path.back() != '/')
    configuration_path += "/"; // ONLY WORKS ON LINUX!!!

  std::vector<std::string> filenames;
  DIR* dir;
  struct dirent* diread;
  if((dir = opendir(absolute_path)) != nullptr)
    {
      while((diread = readdir(dir)) != nullptr)
	{
  	  filenames.emplace_back(configuration_path + static_cast<std::string>(diread->d_name));
    	}
        closedir(dir);
    }
  else
    {
      perror("opendir");
      return configuration_path;
    }

  filenames.erase(std::find(filenames.begin(),filenames.end(),std::string(".")));
  filenames.erase(std::find(filenames.begin(),filenames.end(),std::string("..")));
  std::sort(filenames.begin(),filenames.end()); // probably unnecessary

  if(filenames.size() != 1)
    std::cout << "Error! More than one file in configuration directory!\n";

  return filenames.front();
}

static std::vector<std::string> getOrdersFilenames(const char* absolute_path)
{
  std::string orders_path = absolute_path;
  if(orders_path.back() != '/')
    orders_path += "/"; // ONLY WORKS ON LINUX!!!

  DIR* dir;
  struct dirent* diread;
  std::vector<std::string> filenames;
  if((dir = opendir(absolute_path)) != nullptr)
    {
      while((diread = readdir(dir)) != nullptr)
	{
	  filenames.emplace_back(orders_path + static_cast<std::string>(diread->d_name));
    	}
      closedir(dir);
    }
  else
    {
      perror("opendir");
      return filenames;
    }

  filenames.erase(std::find(filenames.begin(),filenames.end(),orders_path + std::string(".")));
  filenames.erase(std::find(filenames.begin(),filenames.end(),orders_path + std::string("..")));
  std::sort(filenames.begin(),filenames.end()); // probably unnecessary
  return filenames;
}

static void typingInput(std::string* input)
{
  std::cin >> *input;
  s_typing = true;
}

static TopicCode receiveTopic(unsigned waiting_time = DEFAULT_WAITING_TIME)
{
  std::cout << "Waiting for new topic> ";
  std::string input = "";
  std::thread th_typing(typingInput, &input);
  std::this_thread::sleep_for (std::chrono::milliseconds(waiting_time));
  if(s_typing)
    {
      th_typing.join();
      s_typing = false;
    }
  else
    {
      th_typing.detach();
      th_typing.~thread();
    }
  std::cout << std::endl;

  // Remove white space:
  input.erase(std::remove_if(input.begin(), input.end(), isspace), input.end());
  // To lower case:
  std::transform(input.begin(), input.end(), input.begin(),
                 [](unsigned char c){ return std::tolower(c); });

  if(!input.size())
    return topic_empty;
  if(input.find("currentposition") != std::string::npos)
    return topic_current_position;
  if(input.find("neworder") != std::string::npos)
    return topic_new_order;
  if(input.find("end") != std::string::npos)
    return topic_end;

  return topic_default;
}

static TopicCode executeOrders(std::vector<std::string>& filenames, std::vector<ordersmap>& all_orders,
                               productsmap& products, AMR& amr, unsigned waiting_time = DEFAULT_WAITING_TIME)
{
  for(auto orders_file : all_orders)
    {
      for(auto order : orders_file)
	{
	  amr.setNextOrder(order.second);
  	  amr.executeOrder(products);
  	  int topic = receiveTopic(waiting_time);
  	  Position current_pos(amr.getPosition());
  	  std::string input = "";
  	  switch(topic)
  	  {
  	  case topic_empty:
  	    // do nothing, just continue
  	    break;
  	  case topic_current_position:
  	    std::cout << "The AMR's current position is: \nx: " << current_pos.getX()
  		<< "\ny: " << current_pos.getY() << "\nyaw: " << amr.getYawDeg() << std::endl;
  	    break;
  	  case topic_new_order:
  	    return topic_new_order;
  	    break;
  	  case topic_end:
  	    return topic_end;
  	    break;
  	  case topic_default:
  	    std::cout << "Do you wish to continue? [Y/y or N/n] ";
  	    std::cin >> input;
  	    if((input == "N") || (input == "n"))
  	      return topic_end;
  	    break;
  	  default:
  	  std::cout << "Do you wish to continue? [Y/y or N/n] ";
  	  std::cin >> input;
  	  if((input == "N") || (input == "n"))
  	    return topic_end;
  	  break;
  	  }
	}
    }
  return topic_empty;
}

static int readOrdersDirectory(const char* absolute_path, std::vector<std::string>& filenames,
                               std::vector<std::map<uint32_t,Order>>& all_orders)
{
  all_orders.clear();
  filenames.clear();
  filenames = getOrdersFilenames(absolute_path);
  return readOrders(filenames, all_orders);
}
