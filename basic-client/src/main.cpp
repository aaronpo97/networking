#include <iostream>
#include <memory>
#include <string>
#include "../includes/HTTPClient.hpp"

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      throw std::runtime_error("Usage: ./http_client <url>");
    }
    std::string url = argv[1];

    // Remove "http://" or "https://" if present
    if (url.find("http://") == 0) {
      url.erase(0, 7);
    } else if (url.find("https://") == 0) {
      url.erase(0, 8);
    }
    // Remove trailing slash if present
    if (url.back() == '/') {
      url.pop_back();
    }
    std::cout << "Performing GET request to: " << url << std::endl;
    HTTPClient::get(url);
    std::cout << "Request completed successfully." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
