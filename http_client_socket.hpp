#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include "http_common.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include "sock.hpp"
//#include "ssl_sock.hpp"

class http_client_socket_t
{
private:
protected:
	// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
	static std::string trim_whitespace(const std::string& str);

	static bool read_line(sock_t* sock, std::string& dst);
	static bool recv_status_line(sock_t* sock, http_response_t& resp);
	static bool recv_headers(sock_t* sock, http_response_t& resp);
	static bool recv_body(sock_t* sock, http_response_t& resp);

	static bool recv_resp(sock_t* sock, http_response_t& resp);

public:
	http_client_socket_t(void) = default;
	~http_client_socket_t(void) = default;

	static bool get(std::string host, std::string port, std::string uri, http_parameters_t& data, header_map_t& headers, http_response_t& resp, bool ssl = false, bool verify = false);
	static bool post(std::string host, std::string port, std::string uri, http_parameters_t& data, header_map_t& headers, http_response_t& resp, http_content_type_t content_type = http_content_type_t::CONTENT_TYPE_MULTIPART_FORM_DATA, bool ssl = false, bool verify = false);
};

using http_client_type = http_client_socket_t;
