#pragma once
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <vector>
#include <regex>
#include <iostream>

// Link with Iphlpapi.lib and ws2_32.lib
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")


char* ListIpAddresses() {

    char* res = new char[16];
    res[0] = '#';

    IP_ADAPTER_ADDRESSES* adapter_addresses(NULL);
    IP_ADAPTER_ADDRESSES* adapter(NULL);

    DWORD adapter_addresses_buffer_size = 16 * 1024;

    // Get adapter addresses
    for (int attempts = 0; attempts != 3; ++attempts) {
        adapter_addresses = (IP_ADAPTER_ADDRESSES*)malloc(adapter_addresses_buffer_size);

        DWORD error = ::GetAdaptersAddresses(AF_UNSPEC,
            GAA_FLAG_SKIP_ANYCAST |
            GAA_FLAG_SKIP_MULTICAST |
            GAA_FLAG_SKIP_DNS_SERVER |
            GAA_FLAG_SKIP_FRIENDLY_NAME,
            NULL,
            adapter_addresses,
            &adapter_addresses_buffer_size);

        if (ERROR_SUCCESS == error) {
            break;
        }
        else if (ERROR_BUFFER_OVERFLOW == error) {
            // Try again with the new size
            free(adapter_addresses);
            adapter_addresses = NULL;
            continue;
        }
        else {
            // Unexpected error code - log and throw
            free(adapter_addresses);
            adapter_addresses = NULL;
            return res;
        }
    }

    // Iterate through all of the adapters
    for (adapter = adapter_addresses; NULL != adapter; adapter = adapter->Next) {
        // Skip loopback adapters
        if (IF_TYPE_SOFTWARE_LOOPBACK == adapter->IfType) continue;

        if (adapter->OperStatus == IfOperStatusUp) {

            // Parse all IPv4 addresses
            for (IP_ADAPTER_UNICAST_ADDRESS* address = adapter->FirstUnicastAddress; NULL != address; address = address->Next) {
                auto family = address->Address.lpSockaddr->sa_family;
                if (AF_INET == family) {
                    SOCKADDR_IN* ipv4 = reinterpret_cast<SOCKADDR_IN*>(address->Address.lpSockaddr);
                    inet_ntop(AF_INET, &(ipv4->sin_addr), res, 16);

                    //free memory
                    free(adapter_addresses);
                    adapter_addresses = NULL;

                    return res;
                }
            }
        }
    }

    return res;
}


void PrintIPAddress()
{
    char* name = ListIpAddresses();
    printf("Your current IPv4 Address is: %s\n", name);
}

bool IsIPAddressValid(const char* ipAddress) {
    // Sử dụng biểu thức chính quy (regex) để kiểm tra định dạng IP
    std::regex ipRegex("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
        "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

    return std::regex_match(ipAddress, ipRegex);
}

bool RequestForIpAddress(char* ip_addr)
{
    printf("Nhap ip vao: ");
    std::cin.getline(ip_addr, 16);

    return true;
}

