#ifndef HTTP_MSGS_H
#define HTTP_MSGS_H

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace asio_http {

enum class http_status_code : uint16_t { ok = 200, created = 201, not_found = 404 };

const char* status_code_str(http_status_code status_code);

using http_headers = std::unordered_map<std::string, std::string>;

class http_response {
  http_status_code status_code;
  http_headers headers;
  std::string content;

 public:
  static const char* const content_length_key;
  static const char* const content_type_key;

  http_response(http_status_code status_code = http_status_code::not_found) : status_code{status_code} {}
  http_response(http_status_code status_code, std::initializer_list<std::unordered_map<std::string, std::string>::value_type> headers, const std::string& content) : status_code{status_code}, headers{headers}, content{content} {}

  const http_status_code get_status_code() const { return status_code; }
  const http_headers& get_headers() const { return headers; }
  const std::string get_content() const { return content; }

  std::string str() const;
};

class http_request {
 protected:
  static const char* const content_length_key;

  std::string method;
  std::string uri_path;
  std::string uri_query;
  http_headers headers;
  std::shared_ptr<std::vector<unsigned char>> content;

  static std::pair<std::string, std::string> parse_resource_uri(const std::string& resource_uri);
  static std::pair<std::string, std::pair<std::string, std::string>> parse_resource_line(const std::string& resource_line);
  static std::pair<std::string, std::string> parse_header_line(const std::string& header_line);

 public:
  bool add_header(const std::string& header_line);
  std::streamsize content_length() const;
  void set_content(std::shared_ptr<std::vector<unsigned char>>& content);

  const std::string get_method() const { return method; }
  const std::string get_uri_path() const { return uri_path; }
  const std::string get_uri_query() const { return uri_query; }
  const http_headers& get_headers() const { return headers; }
  const std::vector<unsigned char>* get_content() const { return content ? content.get() : nullptr; };

  std::string uri_str() const;

  virtual http_response response() const = 0;
};
}
#endif
