#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "sock.hpp"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

class ssl_sock_t : public sock_t
{
	private:
	protected:
		SSL*		m_ssl;
		SSL_CTX*	m_ctx;

	public:
		ssl_sock_t(void)
			: m_ssl(nullptr), m_ctx(nullptr)
		{
			::OpenSSL_add_all_algorithms();
			::ERR_load_BIO_strings();
			::ERR_load_crypto_strings();
			::SSL_load_error_strings();

			if (0 > ::SSL_library_init())
				throw std::runtime_error("ssl_sock_t::ssl_sock_t(): error initializing OpenSSL");
		}

		virtual ~ssl_sock_t(void)
		{
			if (nullptr != m_ssl)
				::SSL_free(m_ssl);
			if (nullptr != m_ctx)
				::SSL_CTX_free(m_ctx);

			m_ssl = nullptr;
			m_ctx = nullptr;
			return;
		}

		virtual bool
		connect(const std::string& h, const std::string& p)
		{
			const SSL_METHOD* method(::TLSv1_2_client_method());

			if (false == sock_t::connect(h, p))
				return false;

			if (nullptr != m_ctx)
				::SSL_CTX_free(m_ctx);
			
			m_ctx = SSL_CTX_new(method);

			if (nullptr == m_ctx)
				return false;

			if (nullptr != m_ssl)
				::SSL_free(m_ssl);

			m_ssl = ::SSL_new(m_ctx);

			::SSL_set_fd(m_ssl, static_cast< int >(m_sock));

			if (1 != ::SSL_connect(m_ssl)) {
				::SSL_CTX_free(m_ctx);
				::SSL_free(m_ssl);
				m_ssl = nullptr;
				m_ctx = nullptr;
				return false;
			}

			return true;
		}

		virtual bool
		read(std::vector< uint8_t >& data, std::size_t len = 8192)
		{
			std::size_t     offset(0);
			ssize_t         rlen(0);

			data.clear();
			data.resize(len + 1);

			if (static_cast<size_t>(std::numeric_limits< ssize_t >::max()) <= len)
				return false;

			do {
				rlen = static_cast< ssize_t >(::SSL_read(m_ssl, reinterpret_cast< char* >(&data[offset]), len - offset));

				if (0 > rlen)
					return false;

				else if (0 == rlen)
					return true;

				else
					if (static_cast<size_t>(rlen) > len)
						throw std::runtime_error("ssl_sock_t::read(): Invalid state error (len > rlen)");

				offset += rlen;

			} while (offset < len);

			data[len] = '\0';
			return true;
		}

		virtual bool
		read(std::string& s, std::size_t len = 8192)
		{
			std::vector< uint8_t > d;

			if (false == this->read(d, len))
				return false;

			s.clear();
			s.insert(s.begin(), d.begin(), d.end());

			return true;
		}

		virtual bool
		write(const char* str, std::size_t len)
		{
			std::vector< uint8_t > d(len);

			if (0 == len)
				return true;

			std::memcpy(&d[0], str, len);
			return this->write(d);
		}

		virtual bool
		write(const std::string& str)
		{
			std::vector< uint8_t > d(str.begin(), str.end());

			return this->write(d);
		}


		virtual bool
		write(std::vector< uint8_t >& data)
		{
			const std::size_t       len(data.size());
			std::size_t             offset(0);
			ssize_t                 rlen(0);

			if (static_cast<size_t>(std::numeric_limits< ssize_t >::max()) <= len)
				return false;

			do {
				errno = 0;
				rlen = static_cast< ssize_t >(::SSL_write(m_ssl, reinterpret_cast< const char* >(&data[offset]), data.size() - offset));

				if (0 > rlen)
					return false;

				else if (0 == rlen && 0 != offset)
					return true;

				else if (0 == rlen && 0 == offset)
					return false;

				else
					if (static_cast<size_t>(rlen) > len ||
						static_cast<size_t>(rlen) > std::numeric_limits< std::size_t >::max() - offset ||
						static_cast<size_t>(rlen) + offset > len)
						throw std::runtime_error("sock_t::write(): Invalid state error (rlen)");

				offset += rlen;

			} while (offset < len);

			return true;
		}
};

