#include <iostream>
#include <iomanip>
#include <array>
#include <cassert>

#include "CanTpGenerator.h"
#include "CanTpGeneratorUserCfg.h"

#define _WRITE_PCI_INFO(pciInfo, payload) \
    payload[0] = (pciInfo << 4) & 0xF

CanTpGenerator::CanTpGenerator()
{
    CanTpFrameFormat[CanTpFrames::CANTP_SINGLE_FRAME] = {CANTP_PCI_SINGLE_FRAME, NULL};
    CanTpFrameFormat[CanTpFrames::CANTP_FIRST_FRAME] = {CANTP_PCI_FIRST_FRAME, NULL};
    CanTpFrameFormat[CanTpFrames::CANTP_CONSECUTIVE_FRAME] = {CANTP_PCI_CONSECUTIVE_FRAME, NULL};
    CanTpFrameFormat[CanTpFrames::CANTP_FLOW_CONTROL_FRAME] = {CANTP_PCI_FLOWCONTROL_FRAME, NULL};
}

bool CanTpGenerator::ReadConfig()
{
    return true;
}

bool CanTpGenerator::SendFrame()
{
    return true;
}

bool CanTpGenerator::GenerateMsg(uint16_t msgLength, std::vector<std::vector<uint8_t>> &msg)
{
    bool success = false;

    uint16_t reqFrames = RequiredFrames(msgLength);
    msg.resize(reqFrames, std::vector<uint8_t>(CANTP_FRAME_LENGTH, m_defaultFill));

    if (msgLength < CANTP_MAX_PAYLOAD_LENGTH)
    {
        if (msgLength < CANTP_PAYLOAD_BYTES_IN_SF)
        {
            GenerateFrame(CanTpFrames::CANTP_SINGLE_FRAME, msgLength, msg[0]);
        }
        else
        {
            GenerateFrame(CanTpFrames::CANTP_FIRST_FRAME, msgLength, msg[0]);

            uint8_t seqNum = 0;

            for (uint16_t idx = 1; idx < reqFrames; idx++)
            {
                seqNum %= 0x10;
                GenerateFrame(CanTpFrames::CANTP_CONSECUTIVE_FRAME, msgLength, msg[idx], seqNum);
                seqNum += 1;
            }
        }
    }

    return success;
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

void CanTpGenerator::GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, std::vector<uint8_t> &payload, uint8_t seqNum)
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
                _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
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
                _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
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
                _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
                payload[CANTP_PCI_INFO_OFFSET] |= (seqNum & 0x0F);
            }
        }
    }
    break;

    case CanTpFrames::CANTP_FLOW_CONTROL_FRAME:
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
                _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
                payload[CANTP_PCI_INFO_OFFSET] |= (seqNum & 0x0F);

                payload[CANTP_FC_CONTROLFLOW_OFFSET] |= (m_fcFlag & 0xF);
                payload[CANTP_FC_BLOCKSIZE_OFFSET] = m_blockSize;
                payload[CANTP_FC_STMIN_OFFSET] = m_stmin;
            }
        }
    }
    break;

    default:
        break;
    }
}

void CanTpGenerator::SetConfigParam(uint8_t fcFlag, uint8_t blockSize, uint8_t stmin)
{
    m_fcFlag = fcFlag;
    m_blockSize = blockSize;
    m_stmin = stmin;
}

int main()
{
    CanTpGenerator generator;
    uint16_t payloadLength = 0xffe;
    std::vector<std::vector<uint8_t>> payload;

    std::vector<uint8_t> frame(8, 0x45);

    generator.GenerateFrame(CanTpFrames::CANTP_SINGLE_FRAME, 3, frame);

    for (auto &data : frame)
    {
        std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
    }

    // generator.SetDefaultFillValue(0x45);
    // generator.GenerateMsg(payloadLength, payload);

    // for (auto &frame : payload)
    // {
    //     for (auto &data : frame)
    //     {
    //         std::cout << "0x" << std::setfill('0') << std::setw(2) << std::hex << +data << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}