#include "Utils.h"

#include <random>
#include <sstream>
#include <iostream>
#include "MinHook/include/MinHook.h"
#include <algorithm>
#include <Ip2string.h>
#include <vector>

#define LOGI std::cout

// min < max will be swapped
// len < min || len < max, result will be truncated
// len > min || len > max, remaining space will be filled with zero from index 0
std::string Utils::random_number(size_t len, std::string min, std::string max) {
    if (max < min)
        std::swap(min, max);

    std::random_device rd;
    std::mt19937 gen{ rd() };
    std::uniform_int_distribution<> distr{ std::stoi(min.c_str()), std::stoi(max.c_str()) };

    std::string gen_num = std::to_string(distr(gen));

    std::string result{};

    if (gen_num.length() < len) {
        for (int i = 0; i < len - gen_num.length(); i++) {
            result.append("0");
        }
        if (gen_num.find('-') != std::string::npos) {
            result.append(gen_num.substr(1, gen_num.length()));
            result.replace(0, 1, ":");
        }
        else {
            result.append(gen_num.substr(0, gen_num.length()));
        }
        return result;
    }
    else if (gen_num.length() > len) {
        result = gen_num.substr(0, len);
        return result;
    }

    return gen_num;
}

std::string Utils::random_number_hex(int len) {
    std::stringstream str_stream{};

    for (int i = 0; i < len; i++) {
        str_stream << std::hex << (std::stoi(random_number(1, "0", "8")) + std::stoi(random_number(1, "0", "8")));
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
    std::transform(mac.begin(), mac.end(), mac.begin(), toupper);

    return mac;
}

char allowed_chars[] = { ' ','a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0xa, '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '[', ']', '\\', ';', '\'', ',', '.', '/', '{', '}', '|', ':', '"', '<', '>', '?' };

bool Utils::valid_char(char byte_) {
    for (int i = 0; i < sizeof(allowed_chars); i++) {
        if (allowed_chars[i] == byte_) {
            return true;
        }
    }
    return false;
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

    EnumWindows((WNDENUMPROC)__enum_windows_callback, (LPARAM)&found_hwnd);

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

std::string Utils::random_ip() {
    std::string gen_ip{};
    gen_ip.append(random_number(3, "100", "240"));
    for (int i = 0; i < 3; i++) {
        gen_ip.append(".");
        gen_ip.append(random_number(3, "100", "240"));
    }
    return gen_ip;
}

std::vector<BYTE> Utils::mac_to_bytes(std::string mac) {
    std::vector<BYTE> gen_mac{};

    std::unique_ptr<char> hehehha = std::make_unique<char>(mac.length());
    mac.copy(hehehha.get(), mac.length());

    char* ctx = 0;

    char* token = strtok_s(hehehha.get(), ":", &ctx);

    while (token != NULL) {
        gen_mac.push_back(std::stoi(token, nullptr, 16));
        token = strtok_s(NULL, ":", &ctx);
    }

    return gen_mac;
}

std::string Utils::to_str_filtered(std::vector<uint8_t> bytes) {
    std::stringstream temp{};

    for (uint8_t byte : bytes) {
        if (!Utils::valid_char(byte)) {
            temp << "\xFE";
        }
        else {
            temp << byte;
        }
    }
    return temp.str();
}

std::vector<std::string> Utils::str_to_str_vec(std::string str) {
    std::vector<std::string> temp{};

    size_t indx = 0;
    size_t last_indx = 0;

    while (indx != std::string::npos) {
        indx = str.find('\n', indx + 1);

        temp.push_back(str.substr(last_indx, indx - last_indx));

        last_indx = indx + 1;
    }
    temp.push_back(str.substr(str.find_last_of('\n') + 1));

    return temp;
}

std::string Utils::to_readable_hex(uint64_t num) {
    std::string hex = Utils::to_hex(num);
    std::string temp{ "0x" };
    if (hex.length() == 1) {
        temp.append("0");
        temp.append(hex);
        return temp;
    }
    if (hex.length() == 2) {
        temp.append(hex);
        return temp;
    }
}