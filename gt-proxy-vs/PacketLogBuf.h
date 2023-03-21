#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <mutex>

#include "PacketReports.h"

namespace PacketLogBufGlobalVars {
    extern std::string RawStrFrontBuf;
};

class PacketLogBuf
{
public:
    static void Init();

    static void AppedToQueue(uint8_t* buf, size_t buf_len, PacketReportTypeEnum type);

    // after queue is processed, it will be placed in BackBuffer.
    static void ProcessQueue();

    // append BackBuffer's content to FrontBuffer.
    static void Apply();

    //static void DumpToFile(std::string fileName);

    // return FrontBuffer;
    static const std::vector<std::string> GetProcessedQueue();

private:
    static std::vector<std::string> process_packets(PacketReport rep);
};
