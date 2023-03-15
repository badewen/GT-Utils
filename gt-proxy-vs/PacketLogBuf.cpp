#include "PacketLogBuf.h"

#include <iostream>
#include "Utils.h"

std::mutex backBuffer_Mutex{};
std::mutex frontBuffer_Mutex{};

std::mutex packetByteQueues_Mutex{};
static std::vector<PacketReport> PacketByteQueues;

// after packetqueues has been processed by ProcessQueue, it will end up in backbuffer.

static std::vector<std::string> FrontBuffer;
std::vector<std::string> BackBuffer;

static std::vector<std::string> FrontBufferCopy{};

bool hasProcessedQueue = false;

void PacketLogBuf::Init() {
   PacketByteQueues = {};
   BackBuffer = {};
   FrontBuffer = {};
}

void PacketLogBuf::AppedToQueue(uint8_t* buf, size_t buf_len, PacketReportTypeEnum type) {
    packetByteQueues_Mutex.lock();

    std::vector<uint8_t> burh( buf_len );

    memcpy(&burh[0], buf, buf_len);

    PacketReport rep = { .Type = type, .Buffer = burh };

    PacketByteQueues.push_back(rep);

    packetByteQueues_Mutex.unlock();
}

void PacketLogBuf::ProcessQueue() {
    if (PacketByteQueues.size() == 0) {
        return;
    }
    
    packetByteQueues_Mutex.lock();

    std::vector<PacketReport> packetQueues_Copy{};
    PacketByteQueues.reserve(PacketByteQueues.size());
    {
        int sz = PacketByteQueues.size();
        for (int i = 0; i < sz; i++) {
            packetQueues_Copy.push_back(PacketByteQueues.back());
            PacketByteQueues.pop_back();
        }
    }
    packetByteQueues_Mutex.unlock();

    std::vector<std::string> processedQueues{};

    for (PacketReport rep : packetQueues_Copy) {
        std::vector<std::string> temp = process_packets(rep);

        Utils::append_vec_to_vec(temp, processedQueues);
    }

    packetQueues_Copy.clear();

    backBuffer_Mutex.lock();

    Utils::append_vec_to_vec(processedQueues, BackBuffer);

    backBuffer_Mutex.unlock();

    processedQueues.clear();

    hasProcessedQueue = true;

}

void PacketLogBuf::Apply() {
    if (!hasProcessedQueue)
        return;

    std::vector<std::string> backBuffer_Copy{};
    backBuffer_Copy.reserve(BackBuffer.size());

    backBuffer_Mutex.lock();

    Utils::append_vec_to_vec(BackBuffer, backBuffer_Copy);
    BackBuffer.clear();

    backBuffer_Mutex.unlock();


    frontBuffer_Mutex.lock();

    Utils::append_vec_to_vec(backBuffer_Copy, FrontBuffer);

    frontBuffer_Mutex.unlock();

    Utils::append_vec_to_vec(backBuffer_Copy, FrontBufferCopy);

    backBuffer_Copy.clear();
    hasProcessedQueue = false;
    return;
}

std::vector<std::string> PacketLogBuf::process_packets(PacketReport rep) {
    std::string temp{};

    if (rep.Type == PacketReportTypeEnum_Received) {
        temp.append("[INCOMING PACKET]\n\n");
    }
    else {
        temp.append("[OUTGOING PACKET]\n\n");
    }

    temp.append("DATA : \n");

    for (uint8_t byte : rep.Buffer) {
        temp.append(Utils::to_readable_hex(byte) + " ");
    }

    temp.append("\n");

    temp.append("In string : \n");

    temp.append(Utils::to_str_filtered(rep.Buffer));

    temp.append("\n\n");

    return Utils::str_to_str_vec(temp);

}

const std::vector<std::string> PacketLogBuf::GetProcessedQueue() {
    if (frontBuffer_Mutex.try_lock()) {
        //return std::vector<std::string>{FrontBuffer.begin() + line_begin, FrontBuffer.begin() + line_end};
        frontBuffer_Mutex.unlock();
        return FrontBuffer;
    }
    else {
        //return std::vector<std::string>{FrontBufferCopy.begin() + line_begin, FrontBufferCopy.begin() + line_end};
        return FrontBufferCopy;
    }
}
