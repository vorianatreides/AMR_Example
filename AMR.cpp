/*
 * AMR.cpp
 *
 */

#include "AMR.hpp"
#include <future>
#include <mutex>
#include <iostream>
#include <climits>
#include <algorithm>
#include <ctgmath>
//#include <map>

static std::mutex s_row_mutex;
static std::mutex s_perm_mutex;
static std::vector<std::future<int>> s_futures;
static std::vector<std::future<int>> s_permutations;

static int fillRows(WeightedGraph* parts_edges, const std::unordered_map<std::string,Position>::iterator& it_x,
                    const std::unordered_map<std::string,Position>::iterator& it_end, const unsigned i)
{
  auto it_y = it_x;
  ++it_y;
  unsigned j = i+1;
  std::lock_guard<std::mutex> lock(s_row_mutex);
  for(; it_y != it_end; ++it_y)
    {
      Position edge = it_y->second - it_x->second;
      parts_edges->at(i,j++) = edge.getL2Norm();
    }
  return 0;
}

static void fillGraph(WeightedGraph& parts_edges, std::unordered_map<std::string,Position>& parts_positions)
{
  parts_edges.setN(parts_positions.size());
  s_futures.reserve(parts_positions.size());
  unsigned i = 0;
  for(auto it_x = parts_positions.begin(); it_x != parts_positions.end(); ++it_x)
    {
      s_futures.emplace_back(std::async(std::launch::async, fillRows, &parts_edges, it_x, parts_positions.end(), i));
      s_futures.back().wait();
      ++i;
    }
}

static int tryPermutation(const WeightedGraph& parts_edges,
                          const std::vector<std::string>& names,
                          std::vector<std::string>* new_names,
                          const std::vector<double>& source_to_parts,
                          const std::vector<double>& dest_to_parts,
                          const std::vector<unsigned>& vertices,
                          double* min_path,
                          const unsigned& s)
{
  std::lock_guard<std::mutex> lock(s_perm_mutex);
  // store current Path weight(cost)
  double current_path_weight = source_to_parts[s];

  // compute current path weight
  unsigned k = s;
  for(unsigned i = 0; i < vertices.size(); i++)
    {
      current_path_weight += parts_edges.get(k,vertices[i]);
      k = vertices[i];
    }
  current_path_weight += dest_to_parts[k];

  // store the permutation with the shorter path and update the minimum
  if(current_path_weight < *min_path)
    {
      *min_path = current_path_weight;
      new_names->at(0) = names[s];
      unsigned ctr = 0;
      for(auto vertex : vertices)
	new_names->at(++ctr) = names[vertex];
    }

  return 0;
}

static double travllingSalesmanProblem(const WeightedGraph& parts_edges,
                                       const std::vector<std::string>& names,
                                       std::vector<std::string>& new_names,
                                       const std::vector<double>& source_to_parts,
                                       const std::vector<double>& dest_to_parts,
                                       const unsigned s)
{
  // store all vertices apart from source vertices
  std::vector<unsigned> vertices;
  vertices.reserve(parts_edges.getN());
  for (unsigned i = 0; i < parts_edges.getN(); i++)
    if (i != s)
      vertices.push_back(i);

  unsigned long int num_permutations = tgamma(parts_edges.getN());
  s_permutations.reserve(num_permutations);
  double min_path = INT_MAX; // store minimum weight of the tested path
  do
    {
      s_permutations.emplace_back(std::async(std::launch::async, tryPermutation,
                                             parts_edges, names, &new_names, source_to_parts,
                                             dest_to_parts, vertices, &min_path, s));
      s_permutations.back().wait();

    } while(std::next_permutation(vertices.begin(), vertices.end()));

  return min_path;
}


static std::vector<std::string> calculateShortestPath(const WeightedGraph& parts_edges,
                                  const std::unordered_map<std::string,Position>& parts_positions,
                                  const Position& source, const Position& destination)
{
  std::vector<double> source_to_parts;
  source_to_parts.reserve(parts_positions.size());
  std::vector<double> dest_to_parts;
  dest_to_parts.reserve(parts_positions.size());
  std::vector<std::string> parts_names;
  parts_names.reserve(parts_positions.size());
  std::vector<std::string> new_names;
  new_names.reserve(parts_positions.size());
  std::vector<std::string> shortest_permutation;
  shortest_permutation.reserve(parts_positions.size());

  for(auto part : parts_positions)
    {
      Position source_part = part.second - source;
      source_to_parts.push_back(source_part.getL2Norm());
      Position dest_part = part.second - destination;
      dest_to_parts.push_back(dest_part.getL2Norm());
      parts_names.emplace_back(part.first);
      new_names.emplace_back("");
    }

  double min_path = INT_MAX;
  for(unsigned i = 0; i < parts_positions.size(); ++i)
    {
      double current_path_weight = INT_MAX;
      if(parts_edges.getN() > 1)
	{
	  current_path_weight = travllingSalesmanProblem(parts_edges, parts_names, new_names,
	                                                   source_to_parts, dest_to_parts, i);
	  if(current_path_weight < min_path)
	    {
	      min_path = current_path_weight;
	      shortest_permutation = new_names;
	    }
	}
      else
	{
	  min_path = source_to_parts.at(0) + dest_to_parts.at(0);
	  shortest_permutation = parts_names;
	}
    }
  return shortest_permutation;
}

void AMR::executeOrder(std::unordered_map<uint32_t,Product>& products)
{
  std::unordered_map<std::string,std::vector<uint32_t>> parts_products;
  std::unordered_map<std::string,Position> parts_positions;
  unsigned ctr = 0;
  for(auto product_id : m_next_order.getProductIDs())
    {
      if(products.find(product_id) == products.end())
	{
	  std::cout << "Error! An invalid product ID " << product_id <<
	      " while working on order " << m_next_order.getID() << "\nTerminating order!\n";
	  return;
	}
      auto parts = products[product_id].getParts();
      for(auto part : parts)
        {
          auto tmp_it = parts_positions.find(part.first);
          bool flag = false;
          // UNSTABLE BEHAVIOUR !!!
          if(tmp_it != parts_positions.end() && tmp_it->second != part.second)
            {
              // std::string new_part_name = part.first + std::to_string(ctr++);
              // parts_positions.emplace(new_part_name, part.second);
              // parts_products.emplace(new_part_name, std::vector<uint32_t>(1,product_id));
              // flag = true;
            }
          parts_positions.insert(part);
          auto tmp_it_2 = parts_products.find(part.first);
          if(tmp_it_2 == parts_products.end())
            {
              parts_products.emplace(part.first,std::vector<uint32_t>(1,product_id));
            }
          else if(!flag) tmp_it_2->second.emplace_back(product_id);
        }
    }

  fillGraph(m_parts_edges, parts_positions);
  std::vector<std::string> shortest_permutation = calculateShortestPath(m_parts_edges, parts_positions,
                                                                  m_position.getPosition(),
                                                                  m_next_order.getPosition());

  ctr = 0;
  std::cout << "Working on order " << m_next_order.getID() << " at x: " << m_next_order.getPosition().getX()
            << ", y: " << m_next_order.getPosition().getY() << std::endl;
  for(auto part : shortest_permutation)
    {
      for(auto product : parts_products.at(part))
	{
	  Position part_position = parts_positions.at(part);
	  m_position.setDestination(part_position);
	  std::cout << ++ctr << ". Fetching \'" << part << "\' for product \'" << product << "\' ";
	  std::cout << "at x: " << part_position.getX() << ", y: " << part_position.getY() << std::endl;
	}
    }
  m_position.setDestination(m_next_order.getPosition());
  std::cout << std::endl;
}
