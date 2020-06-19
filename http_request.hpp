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
#include "rand.hpp"

typedef std::map< std::string, std::string > header_map_t;

enum class http_method_t 
{ 
	HTTP_METHOD_GET = 0,
	HTTP_METHOD_POST,
	HTTP_METHOD_PUT,
	HTTP_METHOD_DELETE,
	HTTP_METHOD_HEAD,
	HTTP_METHOD_CONNECT,
	HTTP_METHOD_OPTIONS,
	HTTP_METHOD_TRACE,
	HTTP_METHOD_PATCH
};

enum class http_version_t 
{
	HTTP_VERSION_09 = 0,
	HTTP_VERSION_10,
	HTTP_VERSION_11
};

enum class http_content_type_t
{
	CONTENT_TYPE_URL_ENCODED,
	CONTENT_TYPE_MULTIPART_FORM_DATA,
	CONTENT_TYPE_JSON
};

class http_request_data_t
{
	private:
	protected:
	public:
		http_request_data_t(void) = default;
		virtual ~http_request_data_t(void) = default;

		virtual std::string parameter_name(void) = 0;
		virtual void parameter_name(std::string) = 0;

		virtual std::string parameter_value(void) = 0;
		virtual void parameter_value(std::string) = 0;
};

class string_pair_request_data_t : public http_request_data_t
{
	private:
	protected:
		std::string m_name;
		std::string m_value;

	public:
		string_pair_request_data_t(std::string n = "", std::string v = "") : m_name(n), m_value(v)
		{}

		virtual ~string_pair_request_data_t(void) { m_name.clear(); m_value.clear(); return; }
		
		virtual std::string parameter_name(void) { return m_name; }
		virtual void parameter_name(std::string n) { m_name = n; return; }
		
		virtual std::string parameter_value(void) { return m_value; }
		virtual void parameter_value(std::string v) { m_value = v; return; }
};

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
