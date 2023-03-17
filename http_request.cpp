#include "http_request.hpp"

/*
	https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
	https://developer.mozilla.org/en-US/docs/Glossary/percent-encoding
*/
std::string
http_request_t::percent_encode(const std::string src, bool space_in_hex)
{
	std::ostringstream oss;

	oss.fill('0');
	oss << std::hex;

	for (std::string::const_iterator itr = src.begin(); itr != src.end(); itr++) {
		std::string::value_type val(*itr);

		if (std::isalnum(val) || '-' == val || '_' == val || '.' == val || '~' == val) {
			oss << val;
			continue;
		}

		if (' ' == val && false == space_in_hex)
			oss << '+';

		else {
			oss << std::uppercase;
			oss << '%' << std::setw(2) << int(static_cast<unsigned char>(val));
			oss << std::nouppercase;
		}
	}

	return oss.str();
}

std::string
http_request_t::request_line(void)
{
	std::string ret("");

	switch (m_verb) {
	case http_method_t::HTTP_METHOD_GET:
		ret += "GET ";
		break;

	case http_method_t::HTTP_METHOD_POST:
		ret += "POST ";
		break;

	case http_method_t::HTTP_METHOD_PUT:
		ret += "PUT";
		break;

	case http_method_t::HTTP_METHOD_DELETE:
		ret += "DELETE ";
		break;

	case http_method_t::HTTP_METHOD_HEAD:
		ret += "HEAD /";
		break;

	case http_method_t::HTTP_METHOD_CONNECT:
		ret += "CONNECT ";
		break;

	case http_method_t::HTTP_METHOD_OPTIONS:
		ret += "OPTIONS ";
		break;

	case http_method_t::HTTP_METHOD_TRACE:
		ret += "TRACE ";
		break;

	case http_method_t::HTTP_METHOD_PATCH:
		ret += "PATCH ";
		break;

	default:
		throw std::runtime_error("http_request_t::request_line(): Invalid/unsupported HTTP verb encountered");
		break;
	}

	ret += m_uri;
	ret += " ";

	switch (m_version) {
	case http_version_t::HTTP_VERSION_09:
		ret += "HTTP/0.9\r\n";
		break;
	case http_version_t::HTTP_VERSION_10:
		ret += "HTTP/1.0\r\n";
		break;
	case http_version_t::HTTP_VERSION_11:
		ret += "HTTP/1.1\r\n";
		break;
	default:
		throw std::runtime_error("http_request_t::request_line(): Invalid/unsupported HTTP version encountered");
		break;
	}

	return ret;
}

std::string
http_request_t::request_headers(void)
{
	std::string ret("");
	std::size_t sz(m_headers.size());

	for (std::map< std::string, std::string >::const_iterator itr = m_headers.begin(); itr != m_headers.end(); itr++)
		ret += itr->first + ": " + itr->second + "\r\n";

	return ret;
}

http_request_t::http_request_t(const http_method_t verb, const std::string uri, http_version_t version)
	: m_verb(verb), m_uri(uri), m_version(version)
{
	return;
}

http_request_t::~http_request_t(void)
{
	std::size_t sz(m_parameters.size());

	for (std::size_t idx = 0; idx < sz; idx++) {
		delete m_parameters[idx];
		m_parameters[idx] = nullptr;
	}

	m_parameters.clear();
	return;
}

void
http_request_t::verb(const http_method_t method)
{
	m_verb = method;
	return;
}

const http_method_t
http_request_t::verb(void) const
{
	return m_verb;
}

void
http_request_t::uri(const std::string u)
{
	m_uri = u;
	return;
}

const std::string
http_request_t::uri(void) const
{
	return m_uri;
}

void
http_request_t::add_header(const std::string name, const std::string value)
{
	m_headers[name] = value;
	return;
}

std::string
http_request_t::get_header(const std::string name)
{
	if (m_headers.end() == m_headers.find(name))
		return std::string("");

	return m_headers[name];
}

void
http_request_t::add_parameter(http_request_data_t* param)
{
	m_parameters.push_back(std::move(param));
	return;
}

std::vector< http_request_data_t* >&
http_request_t::parameters(void)
{
	return m_parameters;
}

const std::vector< http_request_data_t* >&
http_request_t::parameters(void) const
{
	return m_parameters;
}


std::string
http_get_request_t::request_line(void)
{
	std::string ret("GET ");
	std::size_t sz(m_parameters.size());

	ret += m_uri;
	ret += "?";

	for (std::size_t idx = 0; idx < sz; idx++) {
		if (nullptr != m_parameters[idx]) {
			ret += percent_encode(m_parameters[idx]->parameter_name());
			ret += "=";
			ret += percent_encode(m_parameters[idx]->parameter_value());

			if (idx != sz - 1)
				ret += "&";
		}
	}

	ret += " ";

	switch (m_version) {
	case http_version_t::HTTP_VERSION_09:
		ret += "HTTP/0.9\r\n";
		break;
	case http_version_t::HTTP_VERSION_10:
		ret += "HTTP/1.0\r\n";
		break;
	case http_version_t::HTTP_VERSION_11:
		ret += "HTTP/1.1\r\n";
		break;
	default:
		throw std::runtime_error("http_request_t::request_line(): Invalid/unsupported HTTP version encountered");
		break;
	}

	return ret;
}

http_get_request_t::http_get_request_t(const std::string uri, http_version_t version)
	: http_request_t(http_method_t::HTTP_METHOD_GET, uri, version)
{
	return;
}

std::string
http_get_request_t::to_string(void)
{
	std::string ret(this->request_line());

	ret += request_headers();
	ret += "\r\n";
	return ret;
}

http_post_request_t::http_post_request_t(const std::string uri, http_version_t version)
	: http_request_t(http_method_t::HTTP_METHOD_POST, uri, version), m_content_type(http_content_type_t::CONTENT_TYPE_URL_ENCODED)
{
	return;
}

void
http_post_request_t::content_type(const http_content_type_t type)
{
	m_content_type = type;
	return;
}

const http_content_type_t
http_post_request_t::content_type(void) const
{
	return m_content_type;
}

std::string
http_post_request_t::generate_boundary_tag(void)
{
	rand_t		rnd;
	std::stringstream ss;


	ss << "----WebKitFormBoundary";

	ss.fill('0');
	for (std::size_t idx = 0; idx < 4; idx++)
		ss << std::hex << std::setw(2) << rnd.qword();

	return ss.str();

	std::string ret("----WebKitFormBoundary");

	for (std::size_t idx = 0; idx < 4; idx++)
		ret += std::to_string(rnd.qword());

	return ret;
}

std::string
http_post_request_t::to_string(void)
{
	std::string ret(request_line());
	std::string entity("");
	std::string boundary(generate_boundary_tag());
	std::size_t sz(m_parameters.size());

	switch (m_content_type) {
	case http_content_type_t::CONTENT_TYPE_URL_ENCODED:
		add_header("Content-Type", "application/x-www-form-urlencoded");
		break;
	case http_content_type_t::CONTENT_TYPE_MULTIPART_FORM_DATA:
		add_header("Content-Type", "multipart/form-data; boundary=\"" + boundary + "\"");
		break;
	case http_content_type_t::CONTENT_TYPE_JSON:
		add_header("Content-Type", "application/json");
		break;
	default:
		throw std::runtime_error("http_post_request_t::to_string(): invalid/unsupported content type specified");
		break;
	}

	//ret += request_headers();
	//ret += "\r\n";

	switch (m_content_type) {
	case http_content_type_t::CONTENT_TYPE_URL_ENCODED:

		for (std::size_t idx = 0; idx < sz; idx++) {
			entity += m_parameters[idx]->parameter_name() + "=";
			entity += percent_encode(m_parameters[idx]->parameter_value());

			if (idx != sz - 1)
				entity += "&";
			else
				entity += "\r\n";
		}
		break;
	case http_content_type_t::CONTENT_TYPE_MULTIPART_FORM_DATA:
		for (std::size_t idx = 0; idx < sz; idx++) {
			entity += "--" + boundary + "\r\n";
			entity += "Content-Disposition: form-data; name=\"";
			entity += m_parameters[idx]->parameter_name() + "\"\r\n\r\n";
			entity += m_parameters[idx]->parameter_value() + "\r\n";
			entity += "--" + boundary;

			if (idx == sz - 1)
				entity += "--\r\n";
			else
				entity += "\r\n";
		}
		break;

	case http_content_type_t::CONTENT_TYPE_JSON:
		entity += "{ ";
		for (std::size_t idx = 0; idx < sz; idx++) {
			entity += "\"" + m_parameters[idx]->parameter_name() + "\": ";
			entity += "\"" + m_parameters[idx]->parameter_value() + "\"";

			if (idx != sz - 1)
				entity += ", ";
		}
		entity += " }\r\n";
		break;

	default:
		throw std::runtime_error("http_post_request_t::to_string(): invalid/unsupported content type specified");
		break;
	}

	add_header("Content-Length", std::to_string(entity.length()));
	ret += request_headers();
	ret += "\r\n";
	ret += entity;

	return ret;
}
