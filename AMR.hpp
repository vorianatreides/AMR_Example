/*
 * AMR.hpp
 * The class of the AMR, which communicates with the rest of the program via an API
 */

#ifndef AMR_HPP_
#define AMR_HPP_

#include "AMRPosition.hpp"
#include "Order.hpp"
#include "Product.hpp"
#include "WeightedGraph.hpp"

class AMR
{
private:
  AMRPosition m_position;
  Order m_next_order;
  WeightedGraph m_parts_edges;
public:
  AMR() : m_position(AMRPosition()), m_next_order(Order()), m_parts_edges(WeightedGraph()) {}
  AMR(AMRPosition position) : m_position(position), m_next_order(Order()), m_parts_edges(WeightedGraph()) {}
  AMR(Order& next_order) : m_position(AMRPosition()), m_next_order(next_order), m_parts_edges(WeightedGraph()) {}
  AMR(WeightedGraph& parts_edges) : m_position(AMRPosition()), m_next_order(Order()), m_parts_edges(parts_edges) {}
  AMR(AMRPosition position, Order& next_order) :
    m_position(position), m_next_order(next_order), m_parts_edges(WeightedGraph()) {}
  AMR(AMRPosition position, WeightedGraph& parts_edges) :
    m_position(position), m_next_order(Order()), m_parts_edges(parts_edges) {}
  AMR(Order& next_order, WeightedGraph& parts_edges) :
    m_position(AMRPosition()), m_next_order(next_order), m_parts_edges(parts_edges) {}
  AMR(AMRPosition position, Order& next_order, WeightedGraph& parts_edges) :
    m_position(position), m_next_order(next_order), m_parts_edges(parts_edges) {}

  AMR(const AMR& other)
    {m_position = other.m_position; m_next_order = other.m_next_order; m_parts_edges = other.m_parts_edges;}
  AMR& operator=(const AMR& other)
      {m_position = other.m_position; m_next_order = other.m_next_order; m_parts_edges = other.m_parts_edges; return *this;}

  const Position getPosition() const {return m_position.getPosition();}
  const double getYawRad() const {return m_position.getYawRad();}
  const double getYawDeg() const {return m_position.getYawDeg();}
  const Order& getNextOrder() const {return m_next_order;}
  const WeightedGraph& getWeightedGraph() const {return m_parts_edges;}

  void setPosition(const Position& position) {m_position.setPosition(position);}
  void setYawRad(const double yaw) {m_position.setYawRad(yaw);}
  void setYawDeg(const double yaw) {m_position.setYawDeg(yaw);}
  void setNextOrder(const Order& next_order) {m_next_order = next_order;}
  void setWeightedGraph(const WeightedGraph& parts_edges) {m_parts_edges = parts_edges;}

  void executeOrder(std::unordered_map<uint32_t,Product>& products);
};

#endif /* AMR_HPP_ */
