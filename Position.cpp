/*
 * Position.cpp
 * Implementation of some of the Position class functions
 */

#include "Position.hpp"

// Operator overloading for equality using a predefined precision c_epsilon
bool Position::operator==(const Position& other)
{
  if((std::abs(m_x - other.m_x) <= Position::c_epsilon) && (std::abs(m_y - other.m_y) <= Position::c_epsilon))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Operator overloading for inequality using a predefined precision c_epsilon
bool Position::operator!=(const Position& other)
{
  return !Position::operator==(other);
}

// Operator overloading for assignment
Position& Position::operator=(const Position& other)
{
  m_x = other.m_x;
  m_y = other.m_y;
  return *this;
}

// Operator overloading for addition
Position Position::operator+(const Position& other)
{
  return Position(m_x + other.m_x,m_y + other.m_y);
}

// Operator overloading for subtraction
Position Position::operator-(const Position& other)
{
  return Position(m_x - other.m_x,m_y - other.m_y);
}

// Operator overloading for dot product
const double Position::operator*(const Position& other)
{
  return ((m_x*other.m_x) + (m_y*other.m_y));
}
