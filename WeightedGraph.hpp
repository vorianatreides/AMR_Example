/*
 * WeightedGraph.hpp
 *
 */

#ifndef WEIGHTEDGRAPH_HPP_
#define WEIGHTEDGRAPH_HPP_

#include <vector>
#include <cmath>
#include <exception>

class WeightedGraph
{
private:
  unsigned m_n;
  std::vector<double> m_edges;
public:
  WeightedGraph() : m_n(0) {}
  WeightedGraph(std::vector<double>& edges) : m_n((1 + sqrt(8*edges.size() - 7))/2) {m_edges = edges;}
  WeightedGraph(unsigned n) : m_n(n) {m_edges.reserve(n*(n-1)/2 + 1); m_edges[0] = 0.0;}
  WeightedGraph(unsigned n, double value) : m_n(n), m_edges(std::vector<double>(((n*(n-1)/2 + 1),value))) {m_edges[0] = 0.0;}
  WeightedGraph(const WeightedGraph& other) {m_n = other.m_n; m_edges = other.m_edges;}
  WeightedGraph& operator=(const WeightedGraph& other) {m_n = other.m_n; m_edges = other.m_edges; return *this;}

  const unsigned getN() const {return m_n;}
  const std::vector<double>& getEdges() const {return m_edges;}

  void setN(const unsigned n)
  {
    if(m_n < n) m_edges.resize(n*(n-1)/2 + 1);
    m_n = n;
  }
  void setN(const unsigned n, double value)
    {
      if(m_n < n) m_edges.resize((n*(n-1)/2 + 1), value);
      m_n = n;
    }
  void setEdges(const std::vector<double>& edges)
  {
    m_n = (1 + sqrt(8*edges.size() - 7))/2;
    m_edges = edges;
  }
  void setValue(const double value)
  {
    m_edges.assign(m_edges.size(),value);
    if (m_edges.size() != 0) m_edges[0] = 0.0;
  }

  double& at(unsigned i, unsigned j)
  {
    unsigned min_n = ((i < j) ? (j+1) : (i+1));
    if(min_n > m_n)
      {
	m_edges.resize(min_n*(min_n-1)/2 + 1);
	if(m_n == 0) m_edges[0] = 0.0;
	m_n = min_n;
      }
    return ((i == j) ? m_edges[0] : (i < j) ? m_edges[i*(m_n-1) + j] : m_edges[j*(m_n-1) + i]);
  }

  double& at(unsigned i, unsigned j, double value)
    {
      unsigned min_n = ((i < j) ? (j+1) : (i+1));
      if(min_n > m_n)
        {
          m_edges.resize((min_n*(min_n-1)/2 + 1), value);
          if(m_n == 0) m_edges[0] = 0.0;
          m_n = min_n;
        }
      return ((i == j) ? m_edges[0] : (i < j) ? m_edges[i*(m_n-1) + j] : m_edges[j*(m_n-1) + i]);
    }

  const double& get(unsigned i, unsigned j) const
    {
      unsigned min_n = ((i < j) ? (j+1) : (i+1));
      if(min_n > m_n) {throw std::out_of_range("Attempted access to member of weighted graph out of bounds!");}
      return ((i == j) ? m_edges[0] : (i < j) ? m_edges[i*(m_n-1) + j] : m_edges[j*(m_n-1) + i]);
    }
};

#endif /* WEIGHTEDGRAPH_HPP_ */
