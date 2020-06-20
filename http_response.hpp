#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "http_common.hpp"

class http_response_t
{
	private:
	protected:
		http_version_t	m_version;
		std::size_t		m_status;
		std::string		m_status_text;
		header_map_t	m_headers;
		std::string		m_body;

	public:
		http_response_t(http_version_t version = http_version_t::HTTP_VERSION_11, std::size_t status = 0, std::string text = "", header_map_t headers = header_map_t(), std::string body = "");
		virtual ~http_response_t(void) = default;

		virtual const http_version_t version(void) const;
		virtual void version(const http_version_t version);

		virtual const std::size_t status(void) const;
		virtual void status(const std::size_t status);

		virtual std::string status_text(void) const;
		virtual void status_text(const std::string text);

		virtual void add_header(const std::string name, const std::string value);
		virtual std::string get_header(const std::string name);
		virtual bool has_header(const std::string name) const;

		virtual std::string body(void);
		virtual void body(const std::string body);
		virtual void append_body(const std::string body);
};

