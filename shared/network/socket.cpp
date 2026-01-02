#include "socket.h"
#include <cstring>
#include <sstream>

namespace phaethon {
namespace network {

bool UDPSocket::s_wsaInitialized = false;

void UDPSocket::InitializeWSA() {
#ifdef _WIN32
    if (!s_wsaInitialized) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        s_wsaInitialized = (result == 0);
    }
#endif
}

UDPSocket::UDPSocket()
    : m_socket(INVALID_SOCKET_HANDLE)
    , m_bound(false)
    , m_connected(false) {
    InitializeWSA();
    std::memset(&m_address, 0, sizeof(m_address));
}

UDPSocket::~UDPSocket() {
    Close();
}

bool UDPSocket::Bind(uint16_t port) {
    if (m_socket != INVALID_SOCKET_HANDLE) {
        Close();
    }

    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET_HANDLE) {
        return false;
    }

    // Allow address reuse
    int opt = 1;
    setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(port);

    if (bind(m_socket, (sockaddr*)&m_address, sizeof(m_address)) < 0) {
        Close();
        return false;
    }

    m_bound = true;
    return true;
}

bool UDPSocket::Connect(const std::string& host, uint16_t port) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET_HANDLE) {
            return false;
        }
    }

    m_address.sin_family = AF_INET;
    m_address.sin_port = htons(port);

    // Try to convert IP address
    if (inet_pton(AF_INET, host.c_str(), &m_address.sin_addr) <= 0) {
        // Failed to parse IP, might be hostname (not implemented for simplicity)
        return false;
    }

    // UDP "connect" just sets default destination
    if (connect(m_socket, (sockaddr*)&m_address, sizeof(m_address)) < 0) {
        Close();
        return false;
    }

    m_connected = true;
    return true;
}

int UDPSocket::Send(const void* data, size_t size) {
    if (!m_connected || m_socket == INVALID_SOCKET_HANDLE) {
        return -1;
    }

    return send(m_socket, (const char*)data, (int)size, 0);
}

int UDPSocket::SendTo(const void* data, size_t size, const std::string& host, uint16_t port) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket == INVALID_SOCKET_HANDLE) {
            return -1;
        }
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        return -1;
    }

    return sendto(m_socket, (const char*)data, (int)size, 0,
                  (sockaddr*)&addr, sizeof(addr));
}

int UDPSocket::Receive(void* buffer, size_t bufferSize) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        return -1;
    }

    return recv(m_socket, (char*)buffer, (int)bufferSize, 0);
}

int UDPSocket::ReceiveFrom(void* buffer, size_t bufferSize, std::string& fromHost, uint16_t& fromPort) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        return -1;
    }

    sockaddr_in from;
    socklen_t fromLen = sizeof(from);

    int bytesReceived = recvfrom(m_socket, (char*)buffer, (int)bufferSize, 0,
                                  (sockaddr*)&from, &fromLen);

    if (bytesReceived > 0) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ipStr, sizeof(ipStr));
        fromHost = ipStr;
        fromPort = ntohs(from.sin_port);
    }

    return bytesReceived;
}

bool UDPSocket::SetNonBlocking(bool nonBlocking) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        return false;
    }

#ifdef _WIN32
    u_long mode = nonBlocking ? 1 : 0;
    return ioctlsocket(m_socket, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(m_socket, F_GETFL, 0);
    if (flags < 0) return false;

    flags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    return fcntl(m_socket, F_SETFL, flags) == 0;
#endif
}

bool UDPSocket::SetTimeout(int timeoutMs) {
    if (m_socket == INVALID_SOCKET_HANDLE) {
        return false;
    }

#ifdef _WIN32
    DWORD timeout = timeoutMs;
    return setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == 0;
#else
    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;
    return setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0;
#endif
}

std::string UDPSocket::GetLastError() const {
    int error = SOCKET_ERROR_CODE;
    std::ostringstream oss;
    oss << "Socket error code: " << error;
    return oss.str();
}

void UDPSocket::Close() {
    if (m_socket != INVALID_SOCKET_HANDLE) {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET_HANDLE;
    }
    m_bound = false;
    m_connected = false;
}

} // namespace network
} // namespace phaethon
