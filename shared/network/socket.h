#pragma once
#include <string>
#include <cstdint>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketHandle;
    #define INVALID_SOCKET_HANDLE INVALID_SOCKET
    #define SOCKET_ERROR_CODE WSAGetLastError()
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketHandle;
    #define INVALID_SOCKET_HANDLE -1
    #define SOCKET_ERROR_CODE errno
    #define closesocket close
#endif

namespace phaethon {
namespace network {

class UDPSocket {
public:
    UDPSocket();
    ~UDPSocket();

    // Disable copy
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

    // Server: bind to port
    bool Bind(uint16_t port);

    // Client: connect to remote address
    bool Connect(const std::string& host, uint16_t port);

    // Send data (must Connect first or use SendTo)
    int Send(const void* data, size_t size);

    // Send to specific address
    int SendTo(const void* data, size_t size, const std::string& host, uint16_t port);

    // Receive data (blocking)
    int Receive(void* buffer, size_t bufferSize);

    // Receive from any source
    int ReceiveFrom(void* buffer, size_t bufferSize, std::string& fromHost, uint16_t& fromPort);

    // Set socket to non-blocking mode
    bool SetNonBlocking(bool nonBlocking);

    // Set socket timeout (milliseconds, 0 = no timeout)
    bool SetTimeout(int timeoutMs);

    // Get last error message
    std::string GetLastError() const;

    // Check if socket is valid
    bool IsValid() const { return m_socket != INVALID_SOCKET_HANDLE; }

    // Close socket
    void Close();

private:
    SocketHandle m_socket;
    sockaddr_in m_address;
    bool m_bound;
    bool m_connected;

    static bool s_wsaInitialized;
    static void InitializeWSA();
};

} // namespace network
} // namespace phaethon
