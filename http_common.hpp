#pragma once
#include <string>
#include <map>
#include <vector>
#include <unordered_map>

//typedef std::map< std::string, std::string > header_map_t;
typedef std::unordered_map< std::string, std::string > header_map_t;

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

/*class http_parameter_t
{
	private:
	protected:
	public:
		http_parameter_t(void) = default;
		virtual ~http_parameter_t(void) = default;

		virtual std::string parameter_name(void) = 0;
		virtual void parameter_name(std::string) = 0;

		virtual std::string parameter_value(void) = 0;
		virtual void parameter_value(std::string) = 0;
};*/

//typedef std::vector< http_parameter_t* > http_parameters_t;

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
	string_pair_request_data_t(std::string n = "", std::string v = "")
		: m_name(n), m_value(v)
	{
	}

	virtual ~string_pair_request_data_t(void) { m_name.clear(); m_value.clear(); return; }

	virtual std::string parameter_name(void) { return m_name; }
	virtual void parameter_name(std::string n) { m_name = n; return; }

	virtual std::string parameter_value(void) { return m_value; }
	virtual void parameter_value(std::string v) { m_value = v; return; }
};

typedef std::vector< http_request_data_t* > http_parameters_t;
