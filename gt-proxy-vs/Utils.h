#pragma once

#include <string>

class Utils
{
public:
    static void print_filtered(char byte_);
    static bool modify_text_packet(std::string& text_packet, std::string data, std::string pakcet_name);
    static std::string random_number_int(size_t len, char min_ch = '0', char max_ch = '9');
    static std::string random_number_uint(size_t len, char min_ch = '0', char max_ch = '9');
    static std::string random_number_hex(int len);
    static std::string random_mac();
};

