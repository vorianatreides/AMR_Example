/*
 * Position.hpp
 * Simple class to contain the position of different products and orders
 */

#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <cmath>
#define POSITION_EPSILON 1E-6;

class Position
{
protected:
  static constexpr double c_epsilon = POSITION_EPSILON;
  double m_x;
  double m_y;
public:
  Position() : m_x(0.0), m_y(0.0) {}
  Position(double x, double y) : m_x(x), m_y(y) {}
  Position(const Position& other) {m_x = other.m_x; m_y = other.m_y;}
  virtual ~Position() {};

  const double getX() const {return m_x;}
  const double getY() const {return m_y;}

  void setX(const double& x) {m_x = x;}
  void setY(const double& y) {m_y = y;}

  const double getL2Norm() const {return sqrt(pow(m_x,2) + pow(m_y,2));}
  const double dotProduct(Position& other) const {return ((m_x*other.m_x) + (m_y*other.m_y));}
  void normalize() {const double l2_norm = this->getL2Norm(); m_x /= l2_norm; m_y /= l2_norm;}
  const Position getNormalized() const {Position pos(*this); pos.normalize(); return pos;}

  bool operator==(const Position& other);
  bool operator!=(const Position& other);
  Position& operator=(const Position& other);
  Position operator+(const Position& other);
  Position operator-(const Position& other);
  const double operator*(const Position& other);
};

#endif /* POSITION_HPP_ */
