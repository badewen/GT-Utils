#include "Utils.h"

#include <random>
#include <sstream>
#include <iostream>
#include "MinHook/MinHook.h"

#define LOGI std::cout

// same as random_number_uint but can go negative.
// returned size is len + 1 because negative sign.
std::string Utils::random_number_int(size_t len, char min_ch, char max_ch) {
    std::string min_str{};
    std::string max_str{};

    min_str.reserve(len);
    max_str.reserve(len);

    for (int i = 0; i < len; i++) {
        min_str.push_back(min_ch);
        max_str.push_back(max_ch);
    }

    min_str.replace(0, 1, "1");


    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> distr{ -(std::stoi(min_str)), std::stoi(max_str) };

    return std::to_string(distr(gen));
}

std::string Utils::random_number_uint(size_t len, char min_ch, char max_ch) {

    std::string min_str{};
    std::string max_str{};

    min_str.reserve(len);
    max_str.reserve(len);

    for (int i = 0; i < len; i++) {
        min_str.push_back(min_ch);
        max_str.push_back(max_ch);
    }

    min_str.replace(0, 1, "1");

    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> distr{ std::stoi(min_str), std::stoi(max_str) };

    return std::to_string(distr(gen));
}

std::string Utils::random_number_hex(int len) {
    std::stringstream str_stream{};

    for (int i = 0; i < len; i++) {
        str_stream << std::hex << (std::stoi(random_number_uint(1, '0', '8')) + std::stoi(random_number_uint(1, '0', '8')));
    }

    return str_stream.str();

}

std::string Utils::random_mac() {

    std::string mac{};
    // 17
    mac.reserve(17);
    std::string gen_num = random_number_hex(2);

    for (int i = 0; i < 6; i++) {
        gen_num = random_number_hex(2);
        if (gen_num.length() > 2) {
            i--;
            continue;
        }
        mac.append(gen_num);
        if (i == 5)
            break;
        mac.push_back(':');
    }
    return mac;
}

char allowed_chars[] = { ' ','a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0xa, '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '[', ']', '\\', ';', '\'', ',', '.', '/', '{', '}', '|', ':', '"', '<', '>', '?' };

void Utils::print_filtered(char byte_) {
    for (int i = 0; i < sizeof(allowed_chars); i++) {
        if (allowed_chars[i] == byte_) {
            LOGI << byte_;
            return;
        }
    }
    return;
}

bool Utils::modify_text_packet(std::string& text_packet, std::string data, std::string packet_name) {
    if (data.length() == 0) return false;
    size_t packet_name_pos = text_packet.find(packet_name);

    if (packet_name_pos == std::string::npos) { return false; }

    size_t text_packet_end_pos = text_packet.find("\n", packet_name_pos);

    if (text_packet_end_pos == std::string::npos) {
        text_packet_end_pos = text_packet.length();
    }

    size_t packet_data_field_length = text_packet_end_pos - packet_name_pos - packet_name.length();

    size_t writeable_length = std::clamp(data.length(), (size_t)1, packet_data_field_length);

    text_packet.replace(packet_name_pos + packet_name.length(), writeable_length, data);

    return true;

}

void* Utils::get_vFunc_addr(void* class_ptr, uint32_t index) {
    return (*static_cast<void***>(class_ptr))[index];
}

HWND Utils::get_hwnd() {

    HWND found_hwnd = 0;

    EnumWindows((WNDENUMPROC)__enum_windows_callback, (LPARAM) &found_hwnd);

    return found_hwnd;
    

}

BOOL Utils::__enum_windows_callback(HWND hwnd, LPARAM lparam) {
    DWORD w_pid = 0;
    GetWindowThreadProcessId(hwnd, &w_pid);
    if (w_pid == GetProcessId(GetCurrentProcess())) {
        *((HWND*)lparam) = hwnd;
        return false;
    }
    return true;
}

std::string Utils::to_hex(uint64_t num) {
    std::stringstream str{};
    str << std::hex << num;
    return str.str();
}
