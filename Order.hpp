/*
 * Order.hpp
 * A simple class to hold an order object with its information
 */

#ifndef ORDER_HPP_
#define ORDER_HPP_

#include "Position.hpp"
#include <cstdint>
#include <vector>

class Order
{
private:
  uint32_t m_id;
  Position m_position;
  std::vector<uint32_t> m_product_ids;

public:
  Order() : m_id(0), m_position(Position()) {}
  Order(uint32_t id, Position position) : m_id(id), m_position(position) {}
  Order(uint32_t id, Position position, std::vector<uint32_t>& product_ids) : m_id(id),
      m_position(position), m_product_ids(product_ids) {}
  Order(const Order& other) {m_id = other.m_id; m_position = other.m_position; m_product_ids = other.m_product_ids;}
  Order& operator=(const Order& other)
  {
    m_id = other.m_id; m_position = other.m_position; m_product_ids = other.m_product_ids;
    return *this;
  }

  const uint32_t getID() const {return m_id;}
  const Position getPosition() const {return m_position;}
  const std::vector<uint32_t>& getProductIDs() const {return m_product_ids;}

  void setID(const uint32_t id) {m_id = id;}
  void setPosition(const Position position) {m_position = position;}
  void setProductIDs(const std::vector<uint32_t> product_ids) {m_product_ids = product_ids;}

  void push_backProductID(const uint32_t product_id) {m_product_ids.push_back(product_id);}
  void emplace_backProductID(const uint32_t product_id) {m_product_ids.emplace_back(product_id);}
};

#endif /* ORDER_HPP_ */
