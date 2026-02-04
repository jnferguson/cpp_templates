#pragma once
#include <cstdint>
#include <cstdlib>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "http_common.hpp"
#include "rand.hpp"



class http_request_t
{
private:
protected:
	http_method_t						m_verb;
	std::string							m_uri;
	http_version_t						m_version;
	header_map_t						m_headers;
	std::vector< http_request_data_t* > m_parameters;

	virtual std::string percent_encode(const std::string src, bool space_in_hex = true);
	virtual std::string request_line(void);
	virtual std::string request_headers(void);

public:
	http_request_t(const http_method_t verb = http_method_t::HTTP_METHOD_GET, const std::string uri = "/", http_version_t version = http_version_t::HTTP_VERSION_11);

	virtual ~http_request_t(void);

	virtual void verb(const http_method_t method);
	virtual const http_method_t verb(void) const;

	virtual void uri(const std::string u);
	virtual const std::string uri(void) const;

	virtual void add_header(const std::string name, const std::string value);
	virtual std::string get_header(const std::string name);

	virtual void add_parameter(http_request_data_t* param);
	virtual std::vector< http_request_data_t* >& parameters(void);
	virtual const std::vector< http_request_data_t* >& parameters(void) const;


	virtual std::string to_string(void) = 0;
};

class http_get_request_t : public http_request_t
{
private:
protected:
	virtual std::string request_line(void);

public:
	http_get_request_t(const std::string uri = "/", http_version_t version = http_version_t::HTTP_VERSION_11);

	virtual ~http_get_request_t(void) = default;

	virtual std::string to_string(void);
};

class http_post_request_t : public http_request_t
{
private:
protected:
	http_content_type_t m_content_type;

	std::string generate_boundary_tag(void);

public:
	http_post_request_t(const std::string uri = "/", http_version_t version = http_version_t::HTTP_VERSION_11);
	virtual ~http_post_request_t() = default;

	virtual void content_type(const http_content_type_t type);
	virtual const http_content_type_t content_type(void) const;

	virtual std::string to_string(void);
};
