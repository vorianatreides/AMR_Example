/*
 * Product.hpp
 * A class to hold the information for a given product
 */

#ifndef PRODUCT_HPP_
#define PRODUCT_HPP_

#include "Position.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

typedef std::unordered_multimap<std::string,Position> partsmap;

class Product
{
private:
  uint32_t m_id;
  std::string m_name;
  partsmap m_parts;

public:
  Product() : m_id(0), m_name("") {partsmap m_parts;}
  Product(uint32_t id, std::string name) : m_id(id), m_name(name) {partsmap m_parts;}
  Product(uint32_t id, std::string name, partsmap& parts) :
      m_id(id), m_name(name), m_parts(parts) {}
  Product(const Product& other) {m_id = other.m_id; m_name = other.m_name; m_parts = other.m_parts;}
  Product& operator=(const Product& other)
  {
    m_id = other.m_id; m_name = other.m_name; m_parts = other.m_parts;
    return *this;
  }

  const uint32_t getID() const {return m_id;}
  const std::string getName() const {return m_name;}
  const partsmap& getParts() const {return m_parts;}

  void setName(const std::string name) {m_name = name;}
  void setParts(const partsmap& parts) {m_parts = parts;} // Please avoid!!!
  //void insertPart(const std::string& name, const Position& position) {m_parts.insert(std::make_pair<std::string,Position>(name, position));}
  void emplacePart(const std::string& name, const Position& position) {m_parts.emplace(name, position);}
};

#endif /* PRODUCT_HPP_ */
