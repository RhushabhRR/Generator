#include <iostream>
#include <iomanip>
#include <cassert>

#include "CanTpGenerator.h"

#define _WRITE_PCI_INFO(pciInfo, payload) \
    payload[0] = ((pciInfo & 0x0F) << 4)

CanTpGenerator::CanTpGenerator()
{
    m_canTpFrames[CanTpFrames::CANTP_SINGLE_FRAME] = {CANTP_PCI_SINGLE_FRAME, NULL};
    m_canTpFrames[CanTpFrames::CANTP_FIRST_FRAME] = {CANTP_PCI_FIRST_FRAME, NULL};
    m_canTpFrames[CanTpFrames::CANTP_CONSECUTIVE_FRAME] = {CANTP_PCI_CONSECUTIVE_FRAME, NULL};
    m_canTpFrames[CanTpFrames::CANTP_FLOW_CONTROL_FRAME] = {CANTP_PCI_FLOWCONTROL_FRAME, NULL};
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

    if (reqFrames != 0)
    {
        msg.resize(reqFrames, std::vector<uint8_t>(CANTP_FRAME_LENGTH, m_defaultFill));

        if (msgLength < CANTP_MAX_PAYLOAD_LENGTH)
        {
            if (msgLength < CANTP_PAYLOAD_BYTES_IN_SF)
            {
                success = GenerateSingleFrame(msgLength, msg[0]);
            }
            else
            {
                success = GenerateFirstFrame(msgLength, msg[0]);

                uint8_t seqNum = 0;

                for (uint16_t idx = 1; success && (idx < reqFrames); idx++)
                {
                    seqNum %= 0x10;
                    success = GenerateConsecutiveFrame(msg[idx], seqNum);
                    seqNum += 1;
                }
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
        msgLength -= CANTP_PAYLOAD_BYTES_IN_FF;
        totalFrames += 1;

        while (msgLength >= CANTP_PAYLOAD_BYTES_IN_CF)
        {
            totalFrames += 1;
            msgLength -= CANTP_PAYLOAD_BYTES_IN_CF;
        }

        if ((msgLength != 0) && (msgLength < CANTP_PAYLOAD_BYTES_IN_CF))
        {
            totalFrames++;
        }
    }

    return totalFrames;
}

void CanTpGenerator::SetConfigParam(uint8_t fcFlag, uint8_t blockSize, uint8_t stmin)
{
    m_fcFlag = fcFlag;
    m_blockSize = blockSize;
    m_stmin = stmin;
}

void CanTpGenerator::SetCustomFrameGenerator(CanTpFrames frameType, pfGenerator pfGenerateFrame)
{
    assert(frameType < CanTpFrames::TOTAL_FRAME_TYPES);
    assert(pfGenerateFrame != NULL);

    switch (frameType)
    {
    case CanTpFrames::CANTP_SINGLE_FRAME:
    {
        m_canTpFrames[CanTpFrames::CANTP_SINGLE_FRAME].pfGenerateFrame = pfGenerateFrame;
    }
    break;

    case CanTpFrames::CANTP_FIRST_FRAME:
    {
        m_canTpFrames[CanTpFrames::CANTP_FIRST_FRAME].pfGenerateFrame = pfGenerateFrame;
    }
    break;

    case CanTpFrames::CANTP_CONSECUTIVE_FRAME:
    {
        m_canTpFrames[CanTpFrames::CANTP_CONSECUTIVE_FRAME].pfGenerateFrame = pfGenerateFrame;
    }
    break;

    case CanTpFrames::CANTP_FLOW_CONTROL_FRAME:
    {
        m_canTpFrames[CanTpFrames::CANTP_FLOW_CONTROL_FRAME].pfGenerateFrame = pfGenerateFrame;
    }
    break;

    default:
        break;
    }
}

bool CanTpGenerator::GenerateSingleFrame(uint16_t payloadLength, std::vector<uint8_t> &payload)
{
    bool success = false;

    if (payload.capacity() != CANTP_FRAME_LENGTH)
    {
        payload.resize(CANTP_FRAME_LENGTH);
    }

    if (auto itrFrameType = m_canTpFrames.find(CanTpFrames::CANTP_SINGLE_FRAME);
        itrFrameType != m_canTpFrames.end())
    {
        if (itrFrameType->second.pfGenerateFrame != NULL)
        {
            // Custom payload generator
            itrFrameType->second.pfGenerateFrame(payloadLength, payload);
        }
        else
        {
            _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
            payload[CANTP_PCI_INFO_OFFSET] |= (payloadLength & 0x0F);
        }
        success = true;
    }
    return success;
}

bool CanTpGenerator::GenerateFirstFrame(uint16_t payloadLength, std::vector<uint8_t> &payload)
{
    bool success = false;

    if (payload.capacity() != CANTP_FRAME_LENGTH)
    {
        payload.resize(CANTP_FRAME_LENGTH);
    }

    if (auto itrFrameType = m_canTpFrames.find(CanTpFrames::CANTP_FIRST_FRAME);
        itrFrameType != m_canTpFrames.end())
    {
        if (itrFrameType->second.pfGenerateFrame != NULL)
        {
            // Custom payload generator
            itrFrameType->second.pfGenerateFrame(payloadLength, payload);
        }
        else
        {
            _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
            payload[CANTP_PCI_INFO_OFFSET] |= ((payloadLength >> 8) & 0x0F);

            payload[CANTP_FF_DL_INFO_OFFSET] &= 0x00; // Clear 2nd byte
            payload[CANTP_FF_DL_INFO_OFFSET] |= ((payloadLength)&0xFF);
        }
        success = true;
    }
    return success;
}

bool CanTpGenerator::GenerateConsecutiveFrame(std::vector<uint8_t> &payload, uint8_t sequenceNumber)
{
    bool success = false;

    if (payload.capacity() != CANTP_FRAME_LENGTH)
    {
        payload.resize(CANTP_FRAME_LENGTH);
    }

    if (auto itrFrameType = m_canTpFrames.find(CanTpFrames::CANTP_CONSECUTIVE_FRAME);
        itrFrameType != m_canTpFrames.end())
    {
        if (itrFrameType->second.pfGenerateFrame != NULL)
        {
            // Custom payload generator
            itrFrameType->second.pfGenerateFrame(sequenceNumber, payload);
        }
        else
        {
            assert(sequenceNumber <= 0xF);
            _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);
            payload[CANTP_PCI_INFO_OFFSET] |= (sequenceNumber & 0x0F);
        }
        success = true;
    }

    return success;
}

bool CanTpGenerator::GenerateFlowControlFrame(std::vector<uint8_t> &payload, uint8_t fcFlag, uint8_t blockSize, uint8_t stmin)
{
    bool success = false;

    if (payload.capacity() != CANTP_FRAME_LENGTH)
    {
        payload.resize(CANTP_FRAME_LENGTH);
    }

    if (auto itrFrameType = m_canTpFrames.find(CanTpFrames::CANTP_FLOW_CONTROL_FRAME);
        itrFrameType != m_canTpFrames.end())
    {
        if (itrFrameType->second.pfGenerateFrame != NULL)
        {
            // Custom payload generator
            itrFrameType->second.pfGenerateFrame(0xFF, payload);
        }
        else
        {
            _WRITE_PCI_INFO(itrFrameType->second.pciInfo, payload);

            payload[CANTP_FC_CONTROLFLOW_OFFSET] |= (m_fcFlag & 0xF);
            payload[CANTP_FC_BLOCKSIZE_OFFSET] = m_blockSize;
            payload[CANTP_FC_STMIN_OFFSET] = m_stmin;
        }
        success = true;
    }

    return success;
}
