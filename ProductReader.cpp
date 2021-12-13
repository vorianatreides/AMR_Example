/*
 * ProductReader.cpp
 * Implementation of the ProductReader function
 */

#include "ProductReader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>

ProductExitCode readProducts(const std::string& filename, productsmap& products)
{
  std::ifstream read_yaml(filename);
  if(!read_yaml.is_open())
    return product_invalid_filename;

  uint32_t id = 0;

  std::pair<productsmap::iterator,bool> product_it;
  std::string part_name = "";
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
        	if(token.find("id:") != std::string::npos)
        	  {
        	    ss >> token;
        	    if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
        	      return product_invalid_id;
        	    id = std::stoi(token);
        	  }
        	break;
              case 1:
        	if(token.find("product:") != std::string::npos)
        	  {
        	    std::string product_name = "";
        	    while(ss >> token) product_name += token + " ";
        	    product_name.pop_back();
        	    product_name.erase(std::remove(product_name.begin(), product_name.end(), '\"'), product_name.end());
        	    // No need to check for a correct product name
        	    product_it = products.emplace(id,Product(id,product_name));
        	  }
        	if(token.find("part:") != std::string::npos)
        	  {
        	    part_name = "";
        	    while(ss >> token) part_name += token + " ";
        	    part_name.pop_back();
        	    part_name.erase(std::remove(part_name.begin(), part_name.end(), '\"'), part_name.end());
        	    if(part_name.size() == 0)
        	      return product_invalid_format;
        	  }
        	break;
              case 2:
        	if(token.find("cx:") != std::string::npos)
        	  {
        	    ss >> token;
        	    if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
        	      return product_invalid_format;
        	    cx = std::stod(token);
        	  }
        	if(token.find("cy:") != std::string::npos)
        	  {
        	    ss >> token;
        	    if(std::find_if(token.begin(), token.end(), ::isdigit) == token.end())
        	      return product_invalid_format;
        	    cy = std::stod(token);
        	    product_it.first->second.emplacePart(part_name,Position(cx,cy));
        	  }
        	break;
              default: return product_invalid_format;
            }
  	}
    }
  return product_ok;
}
