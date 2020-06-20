#include "http_response.hpp"

http_response_t::http_response_t(http_version_t version, std::size_t status, std::string text, header_map_t headers, std::string body)
	: m_version(version), m_status(status), m_status_text(text), m_headers(headers), m_body(body)
{}

const http_version_t
http_response_t::version(void) const
{
	return m_version;
}

void
http_response_t::version(const http_version_t v)
{
	m_version = v;
	return;
}

const std::size_t 
http_response_t::status(void) const 
{ 
	return m_status; 
}

void 
http_response_t::status(const std::size_t status) 
{ 
	m_status = status; 
	return; 
}

std::string 
http_response_t::status_text(void) const
{ 
	return m_status_text; 
}

void 
http_response_t::status_text(const std::string text) 
{ 
	m_status_text = text; 
	return; 
}

void 
http_response_t::add_header(const std::string name, const std::string value)
{
	m_headers[name] = value;
	return;
}

std::string 
http_response_t::get_header(const std::string name) 
{
	if (false == this->has_header(name))
		return std::string("");

	return m_headers[name];
}

bool 
http_response_t::has_header(const std::string name) const
{
	return m_headers.find(name) != m_headers.end();
}

std::string 
http_response_t::body(void) 
{ 
	return m_body; 
}

void 
http_response_t::body(const std::string body) 
{ 
	m_body = body; 
	return; 
}

void 
http_response_t::append_body(const std::string body) 
{ 
	m_body += body; 
	return; 
}
