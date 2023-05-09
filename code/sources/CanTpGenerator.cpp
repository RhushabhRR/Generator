#include <iostream>
#include <array>
#include <cassert>

#include "CanTpGenerator.h"
#include "CanTpGeneratorUserCfg.h"

#define CAN_FRAME_LENGTH 8

#define _WRITE_PCI_INFO(payload, pci_info) \
    payload[0] |= ((0xF & pci_info) << 4)

static void WritePayloadLength(CanTpFrames frameType, std::vector<uint8_t> &payload, uint16_t payloadLength);

int main()
{
    CanTpFrames frameType = CanTpFrames::CANTP_FIRST_FRAME;
    uint16_t payloadLength = 0x122;
    uint8_t defaultValue = 0x34;

    CanTpGenerator TpGenerator;
    std::vector<uint8_t> frame;
    TpGenerator.GenerateFrame(frameType, payloadLength, defaultValue, frame);

    for (auto element : frame)
    {
        // if ((element & 0xF0) == 0x2)
        // {
        // std::cout << std::endl;
        // }
        std::cout << std::hex << +element << ", ";
    }

    return 0;
}

bool CanTpGenerator::GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, uint8_t defaultFillValue,
                                   std::vector<uint8_t> &frame) const
{
    bool success = false;

    auto itrFrameType = CanTpFrameFormat.find(frameType);
    if (itrFrameType != CanTpFrameFormat.end())
    {
        if (payloadLength <= itrFrameType->second.maxPayloadLength)
        {
            std::cout << "Frame filled" << std::endl;
            frame.resize(CAN_FRAME_LENGTH, defaultFillValue);
            // Fill the protocol Data
            frame[0] = 0x00;
            _WRITE_PCI_INFO(frame.begin(), itrFrameType->second.pciInfo);
            WritePayloadLength(itrFrameType->first, frame, payloadLength);

            success = true;
        }
        else
        {
            std::cout << "Payload length too long" << std::endl;
            std::cout << "Max paylaod length = " << itrFrameType->second.maxPayloadLength << std::endl;
        }
    }

    else
    {
        std::cout << "Frame type not found" << std::endl;
    }

    return success;
}

bool CanTpGenerator::ReadConfig()
{
    return true;
}

bool CanTpGenerator::SendFrame()
{
    return true;
}

static void WritePayloadLength(CanTpFrames frameType, std::vector<uint8_t> &payload, uint16_t payloadLength)
{
    switch (frameType)
    {
    case CanTpFrames::CANTP_SINGLE_FRAME:
    {
        payload[0] &= 0xF0; // Clear lower nibble
        payload[0] |= (0x0F & static_cast<uint8_t>(payloadLength));
    }
    break;

    case CanTpFrames::CANTP_FIRST_FRAME:
    {
        payload[0] &= 0xF0; // Clear lower nibble
        payload[0] |= (0x0F & (payloadLength >> 8));
        payload[1] = 0x00; // Clear payloadLength area
        payload[1] |= static_cast<uint8_t>(payloadLength);
    }
    break;

    default:
        break;
    }
}