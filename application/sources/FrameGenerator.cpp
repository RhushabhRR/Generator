#include <iostream>
#include <cassert>
#include <iomanip>

#include "FrameGenerator.h"
#include "CanTpGenerator.h"

void GenerateSingleFrame(uint16_t msgLength, std::vector<uint8_t> &payload)
{
    assert(msgLength <= 6);

    payload.resize(CanTpFrameLength, 0xAA);

    // PCI Info
    payload[0] = 0x00;
    payload[0] |= (msgLength & 0xF);

    // Custom Payload
    for (uint8_t idx = 1; idx < CanTpFrameLength; idx++)
    {
        payload[idx] = idx + 2;
    }
}
void GenerateFirstFrame(uint16_t msgLength, std::vector<uint8_t> &payload)
{
    assert(msgLength <= CanTpMaxMsgLength);

    payload.resize(CanTpFrameLength, 0xAA);

    // PCI Info
    payload[0] = 0x10;
    payload[0] |= ((msgLength >> 8) & 0xF);
    payload[1] = (msgLength & 0xFF);

    // Custom payload
    for (uint8_t idx = 2; idx < CanTpFrameLength; idx++)
    {
        payload[idx] = idx * 5;
    }
}
void GenerateConsecutiveFrame(uint16_t seqNum, std::vector<uint8_t> &payload)
{
    assert(seqNum <= 0xF);

    payload.resize(CanTpFrameLength, 0xAA);

    // PCI Info
    payload[0] = 0x20;
    payload[0] |= ((seqNum >> 8) & 0xF);

    // Custom payload
    for (uint8_t idx = 2; idx < CanTpFrameLength; idx++)
    {
        payload[idx] = idx * 2;
    }
}

int main()
{
    CanTpGenerator generator;

    generator.SetCustomFrameGenerator(CanTpFrames::CANTP_SINGLE_FRAME, GenerateSingleFrame);
    generator.SetCustomFrameGenerator(CanTpFrames::CANTP_FIRST_FRAME, GenerateFirstFrame);

    uint16_t payloadLength = 4;
    std::vector<std::vector<uint8_t>> payload;

    std::vector<uint8_t> frame(8, 0x45);

    generator.GenerateFrame(CanTpFrames::CANTP_CONSECUTIVE_FRAME, payloadLength, frame, 3);

    for (auto &data : frame)
    {
        // Use '+' unary oerator to print uint8_t on console
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
    }

    return 0;
}