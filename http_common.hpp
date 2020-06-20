#pragma once
#include <string>
#include <map>
#include <vector>

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

class http_parameter_t
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
};

typedef std::vector< http_parameter_t* > http_parameters_t;
