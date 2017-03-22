#include <cassert>
#include <cstdlib>
#include <ios>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "http_msgs.h"

namespace asio_http {
using namespace std;
using namespace boost;

const char*
status_code_str(http_status_code status_code) {
  switch (status_code) {
  case http_status_code::ok:
    return "OK";
  case http_status_code::created:
    return "CREATED";
  case http_status_code::not_found:
    return "NOT FOUND";
  default:
    return "?";
  }
}

const char* const http_response::content_length_key = "Content-Length";

const char* const http_response::content_type_key = "Content-Type";

string
http_response::str() const {
  ostringstream oss;
  oss << "HTTP/1.1 " << status_code_str(status_code) << '\r' << '\n';
  for (const auto& header : headers)
    oss << header.first << ": " << header.second << '\r' << '\n';
  if (!content.empty())
    oss << content_length_key << ": " << content.size() << '\r' << '\n';
  oss << '\r' << '\n';
  if (!content.empty())
    oss << content;
  return oss.str();
}

const char* const http_request::content_length_key = "content-length";

pair<string, string>
http_request::parse_resource_uri(const string& resource_uri) {
  const auto pos = resource_uri.find('?');
  assert(pos != 0 && pos != resource_uri.length() - 1);
  return pos == string::npos ? pair<string, string>{resource_uri, ""} : pair<string, string>{resource_uri.substr(0, pos), resource_uri.substr(pos + 1)};
}

pair<string, pair<string, string>>
http_request::parse_resource_line(const string& resource_line) {
  vector<string> resources;
  split(resources, resource_line, is_any_of("\t "), token_compress_on);
  assert(2 <= resources.size() && 3 >= resources.size());
  auto cit = resources.cbegin();
  auto method_cit = cit++;
  return pair<string, pair<string, string>>{*method_cit, parse_resource_uri(*cit)};
}

pair<string, string>
http_request::parse_header_line(const string& header_line) {
  const auto pos = header_line.find(':');
  assert(pos != 0);
  if (pos == string::npos) {
    string key{header_line};
    trim(key);
    to_lower(key);
    assert(!key.empty());
    return pair<string, string>{key, ""};
  }
  string key{header_line.substr(0, pos)};
  trim(key);
  to_lower(key);
  assert(!key.empty());
  string value{header_line.substr(pos + 1)};
  trim(value);
  return pair<string, string>{key, value};
}

bool
http_request::add_header(const string& header_line) {
  if (header_line.empty())
    return false;
  const bool ok = headers.insert(parse_header_line(header_line)).second;
  assert(ok);
  return true;
}

streamsize
http_request::content_length() const {
  auto cit = headers.find(content_length_key);
  if (cit == headers.cend())
    return -1;
  char* end{};
  const unsigned long cnt = strtoul(cit->second.c_str(), &end, 10);
  assert(*end == 0);
  return static_cast<streamsize>(cnt);
}

void
http_request::set_content(std::shared_ptr<vector<unsigned char>>& content) {
  this->content = content;
}

string
http_request::uri_str() const {
  if (uri_query.empty())
    return uri_path;
  return uri_path + '?' + uri_query;
}
}
