#include "parser.hpp"

#include <iostream>
#include <sstream>

void APEXDirectSDK::Gantry::parseResponse(std::string response, int &ec, double &value) {
  std::istringstream istream(response);

  std::string buffer;
  std::getline(istream, buffer, ' ');
  ec = std::stoi(buffer);
  if (ec) return;

  std::getline(istream, buffer, ' ');
  value = std::stod(buffer);
}