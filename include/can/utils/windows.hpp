#ifndef INCLUDE_CAN_UTILS_WINDOWS_HPP
#define INCLUDE_CAN_UTILS_WINDOWS_HPP

#include <string>

#include <windows.h>

namespace can::utils::windows {

static std::string get_last_error() {
    DWORD last_error_id = GetLastError();
    if (last_error_id == 0) {
        return "";
    }

    LPSTR message_buffer = nullptr;
    size_t size =
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, last_error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, NULL);

    std::string message(message_buffer, size);
    LocalFree(message_buffer);

    return message;
}

} /* namespace can::utils::windows */

#endif /* INCLUDE_CAN_UTILS_WINDOWS_HPP */