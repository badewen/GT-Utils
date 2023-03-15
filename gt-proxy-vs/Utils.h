#pragma once

#include <string>
#include <chrono>
#include <d3d9.h>
#include <vector>

extern HWND GT_HWND;

namespace Utils
{
    extern bool valid_char(char byte_);

    // if non-ascii char is encounterd, it will  be converted to 0x01.
    extern std::string to_str_filtered(std::vector<uint8_t> bytes);

    extern std::vector<std::string> str_to_str_vec(std::string str);

    extern bool modify_text_packet(std::string& text_packet, std::string data, std::string pakcet_name);
    extern std::string random_number(size_t len, std::string min, std::string max);
    extern std::string random_number_hex(int len);
    extern std::string random_mac();
    extern void* get_vFunc_addr(void* class_ptr, uint32_t index);
    extern HWND get_hwnd();
    extern std::string to_hex(uint64_t num);
    extern std::string to_readable_hex(uint64_t num);
    extern std::string random_ip();
    extern std::vector<BYTE> mac_to_bytes(std::string mac);

    extern BOOL __enum_windows_callback(HWND hwnd, LPARAM lparam);

    template<typename T>
    void append_vec_to_vec(std::vector<T>& src, std::vector<T>& dest) {
        for (T item : src) {
            dest.push_back(item);
        }
    }
};

// credit : https://github.com/TheCherno/Walnut/blob/master/Walnut/src/Walnut/Timer.h
class Timer
{
public:
    Timer() {
        Reset();
    }

    void Reset() {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Elapsed() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
    }

    float ElapsedMillis() {
        return Elapsed() * 1000.0f;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};
