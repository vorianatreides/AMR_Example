/*
 * AMRPosition.hpp
 * Class to inherit Position and adapt it to the AMR's position
 */

#ifndef AMRPOSITION_HPP_
#define AMRPOSITION_HPP_

#include "Position.hpp"
#define PI 3.14159265358979
#define Xx 1.0
#define Yx 0.0

class AMRPosition : public Position
{
private:
  double m_yaw;
  double calculateYaw(const double x, const double y);
  double calculateYaw(Position& destination);

public:
  AMRPosition() : Position(), m_yaw(0.0) {}
  AMRPosition(Position pos) : Position(pos) {m_yaw = calculateYaw(Xx, Yx);}
  AMRPosition(Position pos, double yaw) : Position(pos), m_yaw(yaw) {}
  AMRPosition(const AMRPosition& other) {m_x = other.m_x; m_y = other.m_y; m_yaw = other.m_yaw;}
  AMRPosition(const Position& other) {m_x = other.getX(); m_y = other.getY(); m_yaw = calculateYaw(Xx, Yx);}

  const Position getPosition() const {return Position(m_x, m_y);}
  const double getYawRad() const {return m_yaw;}
  const double getYawDeg() const {return m_yaw*180.0/PI;}
  const double getYawRad(Position &destination) {m_yaw = calculateYaw(destination); return m_yaw;}
  const double getYawDeg(Position &destination) {m_yaw = calculateYaw(destination); return m_yaw*180.0/PI;}

  void setPosition(const Position pos) {m_x = pos.getX(); m_y = pos.getY(); m_yaw = calculateYaw(Xx, Yx);}
  void setDestination(Position dest) {m_yaw = calculateYaw(dest); m_x = dest.getX(); m_y = dest.getY();}
  void setYawRad(const double yaw) {m_yaw = yaw;}
  void setYawDeg(const double yaw) {m_yaw = yaw*PI/180.0;}

  void determineYaw(const double x, const double y) {m_yaw = calculateYaw(x, y);}
  void determineYaw(Position& destination) {m_yaw = calculateYaw(destination);}

  AMRPosition& operator=(const AMRPosition& other) {m_x = other.m_x; m_y = other.m_y; m_yaw = other.m_yaw; return *this;}
  AMRPosition& operator=(const Position& other) {m_x = other.getX(); m_y = other.getY(); m_yaw = calculateYaw(Xx, Yx); return *this;}
};

#endif /* AMRPOSITION_HPP_ */
