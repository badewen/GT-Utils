#pragma once

#include <vector>
#include <stdint.h>

enum PacketReportTypeEnum {
    PacketReportTypeEnum_Sent,
    PacketReportTypeEnum_Received
};

struct PacketReport
{
    PacketReportTypeEnum Type;
    std::vector<uint8_t> Buffer;
};

