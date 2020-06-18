#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <limits>
#include <stdexcept>

#if defined(_MSC_VER)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <BaseTsd.h>
#define CLOSE_SOCKET ::closesocket
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define CLOSE_SOCKET ::close
#endif

class sock_t {
    private:
    protected:
        ssize_t m_sock;
//#if defined(_MSC_VER)
        //SOCKET m_sock;
//#else
  //      int64_t m_sock;
//#endif

    public:
        sock_t(void) : m_sock(-1)
        {
#if defined(_MSC_VER)
            WSADATA wsaData;

            WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        }

        virtual ~sock_t(void)
        {
            if (-1 != m_sock)
                this->close();

            return;
        }

        virtual bool
        connect(const std::string& h, const std::string& p)
        {
            signed int          ret(0);
            struct addrinfo     hints = { 0 };
            struct addrinfo*    sinfo(nullptr);
            struct addrinfo*    rp(nullptr);

            hints.ai_family     = AF_UNSPEC;
            hints.ai_socktype   = SOCK_STREAM;
            hints.ai_flags      = AI_PASSIVE;
            hints.ai_protocol   = 0;
            hints.ai_addrlen    = 0;
            hints.ai_canonname  = nullptr;
            hints.ai_addr       = nullptr;
            hints.ai_next       = nullptr;

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
                    this->close();
                    errno = ret;
                } else
                    break;
            }

            if (nullptr == rp || 0 > ret) {
                ret = errno;
                ::freeaddrinfo(sinfo);
                this->close(); 
                errno = ret;
                return false;
            }

            ::freeaddrinfo(sinfo);
            return true;
        }

        virtual void
        close(void)
        {
            CLOSE_SOCKET(m_sock);
            m_sock = -1;
            return;
        }

        virtual bool
        read(std::vector< uint8_t >& data, std::size_t len = 8192)
        {
            std::size_t     offset(0);
            ssize_t         rlen(0);

            data.clear();
            data.resize(len + 1);

            if (static_cast< size_t >(std::numeric_limits< ssize_t >::max()) <= len)
                return false;

            do {
                rlen = ::recv(m_sock, reinterpret_cast< char* >(&data[offset]), len - offset, 0);

                if (0 > rlen)
                    return false;

                else if (0 == rlen)
                    return true;

                else
                    if (static_cast< size_t >(rlen) > len)
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

            if (false == this->read(d, len))
                return false;

            s.clear();
            s.insert(s.begin(), d.begin(), d.end());
            //for (std::size_t idx = 0; idx < d.size(); idx++)
            //    s += d[idx];

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

            if (static_cast< size_t >(std::numeric_limits< ssize_t >::max()) <= len)
                return false;

            do {
                errno = 0;
                rlen = ::send(m_sock, reinterpret_cast< const char* >(&data[offset]), data.size() - offset, 0);

                if (0 > rlen) 
                    return false;

                else if (0 == rlen && 0 != offset)
                    return true;

                else if (0 == rlen && 0 == offset)
                    return false;

                else
                    if (static_cast< size_t >(rlen) > len || 
                        static_cast< size_t >(rlen) > std::numeric_limits< std::size_t >::max() - offset || 
                        static_cast< size_t >(rlen) + offset > len)
                            throw std::runtime_error("sock_t::write(): Invalid state error (rlen)");

                offset += rlen;

            } while (offset < len);

            return true;
        }
};
