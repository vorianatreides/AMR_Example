/*
 * ProductReader.hpp
 * A header to contain the declaration of the function to read the products configuration into an unordered map
 */

#ifndef PRODUCTREADER_HPP_
#define PRODUCTREADER_HPP_

#include "Product.hpp"
#include <unordered_map>

enum ProductExitCode {product_ok, product_invalid_filename, product_invalid_id, product_invalid_format };
typedef std::unordered_map<uint32_t,Product> productsmap;

ProductExitCode readProducts (const std::string& filename, productsmap& products);

#endif /* PRODUCTREADER_HPP_ */
