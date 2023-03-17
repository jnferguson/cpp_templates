#include "http_client_socket.hpp"

// https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
std::string
http_client_socket_t::trim_whitespace(const std::string& str)
{
	std::size_t first = str.find_first_not_of(' ');

	if (std::string::npos == first)
		return str;

	std::size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

bool
http_client_socket_t::read_line(sock_t* sock, std::string& dst)
{
	bool has_cr(false), has_nl(false);

	if (nullptr == sock)
		return false;

	do {
		std::string data("");

		if (false == sock->read(data, 1))
			return false;

		if ('\r' == data[0]) {
			if (true == has_cr)
				return false; // malformed

			has_cr = true;
		}
		else if ('\n' == data[0]) {
			if (true == has_nl)
				return false; // malformed

			has_nl = true;
		}
		else
			dst += data[0];
	} while (false == has_cr || false == has_nl);

	return true;
}

bool
http_client_socket_t::recv_status_line(sock_t* sock, http_response_t& resp)
{
	bool		has_cr(false), has_nl(false);
	std::string scratch(""), value("");

	if (nullptr == sock)
		return false;

	if (false == read_line(sock, scratch))
		return false;

	if (scratch.compare(0, 5, "HTTP/")) // malformed
		return false;

	scratch = scratch.substr(5);

	if (!scratch.compare(0, 3, "0.9"))
		resp.version(http_version_t::HTTP_VERSION_09);
	else if (!scratch.compare(0, 3, "1.0"))
		resp.version(http_version_t::HTTP_VERSION_10);
	else if (!scratch.compare(0, 3, "1.1"))
		resp.version(http_version_t::HTTP_VERSION_11);
	else
		return false; // unsupported version

	scratch = scratch.substr(3);

	scratch = trim_whitespace(scratch);

	for (std::size_t idx = 0; idx < scratch.length(); idx++)
		if (' ' == scratch[idx])
			break;
		else
			value += scratch[idx];

	resp.status(std::atoi(value.c_str()));

	scratch = scratch.substr(value.length());
	scratch = trim_whitespace(scratch);
	resp.status_text(scratch);

	return true;
}

bool
http_client_socket_t::recv_headers(sock_t* sock, http_response_t& resp)
{
	std::string line("");
	std::string name(""), value("");

	if (nullptr == sock)
		return false;

	do {
		if (false == read_line(sock, line))
			return false;
		if (!line.length()) // line started with \r\n
			break;

		if (std::string::npos == line.find_first_of(':'))
			return false; // malformed header line

		name = trim_whitespace(line.substr(0, line.find_first_of(':')));
		value = trim_whitespace(line.substr(line.find_first_of(':') + 1));

		resp.add_header(name, value);
		line.clear();
	} while (true);

	return true;
}

bool 
http_client_socket_t::recv_body(sock_t* sock, http_response_t& resp)
{
	if (nullptr == sock)
		return false;

	if (true == resp.has_header("Content-Length")) {
		std::size_t length(std::atoi(resp.get_header("Content-Length").c_str()));
		std::string body;

		if (false == sock->read(body, length))
			return false;

		resp.body(body);
		return true;
	}

	if (true == resp.has_header("Transfer-Encoding")) {
		if (std::string::npos != resp.get_header("Transfer-Encoding").find_first_of("hunked")) {
			// oh that HTTP data stream...he's so dreamy...
			std::size_t chunk_len(0);
			std::string line("");
			std::string body("");

			do {
				if (false == read_line(sock, line))
					return false;
				if (!line.length())
					continue;
				chunk_len = std::strtoul(line.c_str(), nullptr, 16);

				if (false == sock->read(body, chunk_len))
					return false;

				resp.append_body(body);
				body.clear();
				line.clear();
			} while (0 != chunk_len);

			return true;
		}
	}

	return true;
}

bool 
http_client_socket_t::recv_resp(sock_t* sock, http_response_t& resp)
{
	if (false == recv_status_line(sock, resp))
		return false;
	if (false == recv_headers(sock, resp))
		return false;
	if (false == recv_body(sock, resp))
		return false;

	return true;
}

bool
http_client_socket_t::get(std::string host, std::string uri, http_parameters_t& data, header_map_t& headers, http_response_t& resp, bool ssl, bool verify)
{
	http_get_request_t req(uri);
	sock_t* sock(nullptr);

	if (true == ssl && true == verify)
		return false; // SSL certificate verification not implemented yet

	if (true == ssl) {
		sock = new ssl_sock_t;
	}
	else
		sock = new sock_t;

	if (false == sock->connect(host, ssl == true ? "443" : "80"))
		return false;

	for (std::size_t idx = 0; idx < data.size(); idx++)
		req.add_parameter(data[idx]);
	for (header_map_t::const_iterator itr = headers.begin(); itr != headers.end(); itr++)
		req.add_header(itr->first, itr->second);

	if (false == sock->write(req.to_string()))
		return false;

	if (false == recv_resp(sock, resp))
		return false;

	return true;
}

bool 
http_client_socket_t::post(std::string host, std::string uri, http_parameters_t& data, header_map_t& headers, http_response_t& resp, http_content_type_t content_type, bool ssl, bool verify)
{
	http_post_request_t req(uri);
	sock_t* sock(nullptr);

	if (true == ssl && true == verify)
		return false; // SSL certificate verification not implemented yet

	if (true == ssl) {
		sock = new ssl_sock_t;
	} else
		sock = new sock_t;

	if (false == sock->connect(host, ssl == true ? "443" : "80"))
		return false;

	for (std::size_t idx = 0; idx < data.size(); idx++)
		req.add_parameter(data[idx]);
	for (header_map_t::const_iterator itr = headers.begin(); itr != headers.end(); itr++)
		req.add_header(itr->first, itr->second);

	req.content_type(content_type);

	if (false == sock->write(req.to_string()))
		return false;

	if (false == recv_resp(sock, resp))
		return false;

	return true;
}
