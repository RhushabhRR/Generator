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
        payload[idx] = idx;
    }
}
void GenerateConsecutiveFrame(uint16_t seqNum, std::vector<uint8_t> &payload)
{
    assert(seqNum <= 0xF);

    payload.resize(CanTpFrameLength, 0xAA);

    // PCI Info
    payload[0] = 0x20;
    payload[0] |= (seqNum & 0xF);

    // Custom payload
    for (uint8_t idx = 1; idx < CanTpFrameLength; idx++)
    {
        payload[idx] = idx * 2;
    }
}

int main()
{
    CanTpGenerator generator;

    // OPTIONAL - if not provided, default frame generator will be used, with deafult fill value
    // Use SetDefaultFillValue(uint8_t defaultVal) to change default fill
    {
        generator.SetCustomFrameGenerator(CanTpFrames::CANTP_SINGLE_FRAME, GenerateSingleFrame);
        generator.SetCustomFrameGenerator(CanTpFrames::CANTP_FIRST_FRAME, GenerateFirstFrame);
        generator.SetCustomFrameGenerator(CanTpFrames::CANTP_CONSECUTIVE_FRAME, GenerateConsecutiveFrame);
    }

    uint16_t payloadLength = 140;

    // Example to generate entire message
    {
        std::vector<std::vector<uint8_t>> payload;
        generator.GenerateMsg(payloadLength, payload);

        for (auto &frame : payload)
        {
            for (auto &data : frame)
            {
                // Use '+' unary oerator to print uint8_t on console
                std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
            }
            std::cout << std::endl;
        }
    }

    // Example to generate specific frame
    {
        // std::vector<uint8_t> frame(8, 0x45); //Or an empty frame can be passed
        // generator.GenerateSingleFrame(payloadLength, frame);

        // for (auto &data : frame)
        // {
        //     // Use '+' unary oerator to print uint8_t on console
        //     std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
        // }
        // std::cout << std::endl;
    }

    return 0;
}