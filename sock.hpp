#ifndef HAVE_SOCKET_HPP
#define HAVE_SOCKET_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


class sock_t {
        private:
        protected:
                int64_t m_sock;
        public:
                sock_t(void) : m_sock(-1)
                {
                }

                virtual ~sock_t(void)
                {
                        if (-1 != m_sock) {
                                close(m_sock);
                                m_sock = -1;
                        }

                        return;
                }

                virtual bool
                connect(const std::string& h, const std::string& p)
                {
                        signed int              ret(0);
                        struct addrinfo         hints = {0};
                        struct addrinfo*        sinfo(nullptr);
                        struct addrinfo*        rp(nullptr);

                        hints.ai_family         = AF_UNSPEC;
                        hints.ai_socktype       = SOCK_STREAM;
                        hints.ai_flags          = AI_PASSIVE;
                        hints.ai_protocol       = 0;

                        if (::getaddrinfo(h.c_str(), p.c_str(), &hints, &sinfo))
                                return false;


                        for (rp = sinfo; rp != nullptr; rp = rp->ai_next) {
                                m_sock = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

                                if (0 > m_sock) {
                                        ret = errno;
                                        ::freeaddrinfo(sinfo);
                                        errno = ret;
                                        return false;
                                }

                                ret = ::connect(m_sock, rp->ai_addr, rp->ai_addrlen);

                                if (0 > ret) {
                                        ret = errno;
                                        close(m_sock);
                                        errno = ret;
                                        m_sock = -1;
                                } else
                                        break;

                        }

                        if (nullptr == rp || 0 > ret) {
                                ret = errno;
                                ::freeaddrinfo(sinfo);
                                close(m_sock);
                                m_sock = -1;
                                errno = ret;
                                return false;
                        }

                        ::freeaddrinfo(sinfo);
                        return true;
                }

                virtual bool
                read(std::vector< uint8_t >& data, std::size_t len = 8192)
                {
                        std::size_t     offset(0);
                        int64_t         rlen(0);

                        data.clear();
                        data.resize(len+1);
                        do {
                                rlen = ::read(m_sock, &data[0], len);

                                if (0 > rlen)
                                        return false;

                                else if (0 == rlen)
                                        return true;

                                else
                                        if (rlen > len)
                                                throw std::runtime_error("sock_t::read(): Invalid state error (len > rlen)");

                                offset += rlen;

                        } while (offset < len);

                        data[len] = '\0';
                        return true;
                }

                virtual bool
                read(std::string& s, std::size_t len = 8192)
                {
                        std::vector< uint8_t > d;

                        if (0 > this->read(d, len))
                                return false;

                        s.clear();

                        for (std::size_t idx = 0; idx < d.size(); idx++)
                                s += d.at(idx);

                        return true;
                }

                virtual bool
                write(const char* str, std::size_t len)
                {
                        std::vector< uint8_t > d(len);

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
                        int64_t                 rlen(0);

                        do {
                                errno = 0;
                                rlen = ::write(m_sock, &data[0], data.size());

                                if (0 > rlen) {
                                        printf("rlen: %d : %s\n", rlen, ::strerror(errno));
                                        return false;
                                }

                                else if (0 == rlen && 0 != offset)
                                        return true;

                                else if (0 == rlen && 0 == offset)
                                        return false;

                                else
                                        if (rlen > len || rlen > std::numeric_limits< std::size_t >::max() - offset || rlen+offset > len)
                                                throw std::runtime_error("sock_t::write(): Invalid state error (rlen)");

                                offset += rlen;

                        } while (offset < len);

                        return true;
                }
};


#endif
