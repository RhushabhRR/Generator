#include <iostream>
#include <iomanip>
#include <array>
#include <cassert>

#include "CanTpGenerator.h"
#include "CanTpGeneratorUserCfg.h"

#define CAN_FRAME_LENGTH 8

#define _WRITE_PCI_INFO(payload, pci_info) \
    payload[0] |= ((0xF & pci_info) << 4)

static void WritePayloadLength(CanTpFrames frameType, std::vector<uint8_t> &payload, uint16_t payloadLength);
static void GenerateSFpayload() {}
static void GenerateFFpayload() {}
static void GenerateCFpayload() {}
static void GenerateFCpayload() {}

CanTpGenerator::CanTpGenerator()
{
    CanTpFrameFormat[CanTpFrames::CANTP_SINGLE_FRAME] = {CANTP_PCI_SINGLE_FRAME, NULL};
    CanTpFrameFormat[CanTpFrames::CANTP_FIRST_FRAME] = {CANTP_PCI_FIRST_FRAME, NULL};
    CanTpFrameFormat[CanTpFrames::CANTP_CONSECUTIVE_FRAME] = {CANTP_PCI_CONSECUTIVE_FRAME, NULL};
}

bool CanTpGenerator::GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, uint8_t defaultFillValue,
                                   std::vector<uint8_t> &frame) const
{
    bool success = false;

    // auto itrFrameType = CanTpFrameFormat.find(frameType);
    // if (itrFrameType != CanTpFrameFormat.end())
    // {
    //     if (payloadLength <= itrFrameType->second.maxPayloadLength)
    //     {
    //         frame.resize(CAN_FRAME_LENGTH, defaultFillValue);
    //         // Fill the protocol Data
    //         frame[0] = 0x00;
    //         _WRITE_PCI_INFO(frame.begin(), itrFrameType->second.pciInfo);
    //         WritePayloadLength(itrFrameType->first, frame, payloadLength);

    //         success = true;
    //     }
    //     else
    //     {
    //         std::cout << "Payload length too long" << std::endl;
    //         std::cout << "Max paylaod length = " << itrFrameType->second.maxPayloadLength << std::endl;
    //     }
    // }

    // else
    // {
    //     std::cout << "Frame type not found" << std::endl;
    // }

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

bool CanTpGenerator::GenerateFrame(uint16_t payloadLength, std::vector<std::vector<uint8_t>> &frame)
{
    bool success = false;

    uint16_t reqFrames = RequiredFrames(payloadLength);
    frame.resize(reqFrames, std::vector<uint8_t>(CANTP_FRAME_LENGTH, m_defaultFill));

    if (payloadLength < CANTP_MAX_PAYLOAD_LENGTH)
    {
        if (payloadLength < CANTP_PAYLOAD_BYTES_IN_SF)
        {
            GeneratePayload(CanTpFrames::CANTP_SINGLE_FRAME, payloadLength, frame[0]);
        }
        else
        {
            GeneratePayload(CanTpFrames::CANTP_FIRST_FRAME, payloadLength, frame[0]);

            uint8_t seqNum = 0;

            for (uint16_t idx = 1; idx < reqFrames; idx++)
            {
                seqNum %= 0x10;
                GeneratePayload(CanTpFrames::CANTP_CONSECUTIVE_FRAME, payloadLength, frame[idx], seqNum);
                seqNum += 1;
            }
        }
    }

    return success;
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

uint16_t CanTpGenerator::RequiredFrames(uint16_t msgLength)
{
    uint16_t totalFrames = 0;
    if (msgLength < CANTP_PAYLOAD_BYTES_IN_SF)
    {
        totalFrames = 1;
    }
    else if (msgLength < CANTP_MAX_PAYLOAD_LENGTH)
    {
        uint16_t payloadCounter = 0;

        payloadCounter += CANTP_PAYLOAD_BYTES_IN_FF;
        totalFrames += 1;

        while (payloadCounter < msgLength)
        {
            totalFrames += 1;
            payloadCounter += CANTP_PAYLOAD_BYTES_IN_CF;
        }
    }

    return totalFrames;
}

void CanTpGenerator::GeneratePayload(CanTpFrames frameType, uint16_t payloadLength, std::vector<uint8_t> &payload, uint8_t seqNum)
{
    assert(frameType < CanTpFrames::TOTAL_FRAME_TYPES);

    switch (frameType)
    {
    case CanTpFrames::CANTP_SINGLE_FRAME:
    {
        auto itrFrameType = CanTpFrameFormat.find(frameType);
        if (itrFrameType != CanTpFrameFormat.end())
        {
            if (itrFrameType->second.pfWritePayload != NULL)
            {
                // Custom payload generator
                itrFrameType->second.pfWritePayload(payloadLength, payload);
            }
            else
            {
                payload[CANTP_PCI_INFO_OFFSET] &= 0x00; // Clear 1st byte
                payload[CANTP_PCI_INFO_OFFSET] |= ((itrFrameType->second.pciInfo << 4) & 0xF0);
                payload[CANTP_PCI_INFO_OFFSET] |= (payloadLength & 0x0F);
            }
        }
    }
    break;

    case CanTpFrames::CANTP_FIRST_FRAME:
    {
        auto itrFrameType = CanTpFrameFormat.find(frameType);
        if (itrFrameType != CanTpFrameFormat.end())
        {
            if (itrFrameType->second.pfWritePayload != NULL)
            {
                // Custom payload generator
                itrFrameType->second.pfWritePayload(payloadLength, payload);
            }
            else
            {
                payload[CANTP_PCI_INFO_OFFSET] &= 0x00; // Clear 1st byte
                payload[CANTP_PCI_INFO_OFFSET] |= ((itrFrameType->second.pciInfo << 4) & 0xF0);
                payload[CANTP_PCI_INFO_OFFSET] |= ((payloadLength >> 8) & 0x0F);

                payload[CANTP_FF_DL_INFO_OFFSET] &= 0x00; // Clear 2nd byte
                payload[CANTP_FF_DL_INFO_OFFSET] |= ((payloadLength)&0xFF);
            }
        }
    }
    break;

    case CanTpFrames::CANTP_CONSECUTIVE_FRAME:
    {
        auto itrFrameType = CanTpFrameFormat.find(frameType);
        if (itrFrameType != CanTpFrameFormat.end())
        {
            if (itrFrameType->second.pfWritePayload != NULL)
            {
                // Custom payload generator
                itrFrameType->second.pfWritePayload(payloadLength, payload);
            }
            else
            {
                assert(seqNum <= 0xF);
                // TODO - resize only if vector is empty
                payload[CANTP_PCI_INFO_OFFSET] &= 0x00; // Clear 1st byte
                payload[CANTP_PCI_INFO_OFFSET] |= ((itrFrameType->second.pciInfo << 4) & 0xF0);
                payload[CANTP_PCI_INFO_OFFSET] |= (seqNum & 0x0F);
            }
        }
    }
    break;

    default:
        break;
    }
}

int main()
{
    CanTpGenerator generator;
    uint16_t payloadLength = 0xffe;
    std::vector<std::vector<uint8_t>> payload;

    generator.SetDefaultFillValue(0x45);
    generator.GenerateFrame(payloadLength, payload);

    for (auto &frame : payload)
    {
        for (auto &data : frame)
        {
            std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
        }
        std::cout << std::endl;
    }

    return 0;
}