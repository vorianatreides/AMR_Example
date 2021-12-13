/*
 * AMRPosition.cpp
 *
 */

#include "AMRPosition.hpp"

double AMRPosition::calculateYaw(Position& destination)
{
  Position direction = destination - this->getPosition();
  return acos(this->dotProduct(destination)/(this->getL2Norm()*destination.getL2Norm()));
}

double AMRPosition::calculateYaw(const double x, const double y)
{
  Position destination(x, y);
  return calculateYaw(destination);
}
