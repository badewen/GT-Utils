#pragma once

#include <string>
#include <chrono>
#include <d3d9.h>

extern HWND GT_HWND;

namespace Utils
{
    extern void print_filtered(char byte_);
    extern bool modify_text_packet(std::string& text_packet, std::string data, std::string pakcet_name);
    extern std::string random_number_int(size_t len, char min_ch = '0', char max_ch = '9');
    extern std::string random_number_uint(size_t len, char min_ch = '0', char max_ch = '9');
    extern std::string random_number_hex(int len);
    extern std::string random_mac();
	extern void* get_vFunc_addr(void* class_ptr, uint32_t index);
	extern HWND get_hwnd();
	extern std::string to_hex(uint64_t num);

	extern BOOL __enum_windows_callback(HWND hwnd, LPARAM lparam);
};

// credit : https://github.com/TheCherno/Walnut/blob/master/Walnut/src/Walnut/Timer.h
class Timer
{
public:
	Timer()	{
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
