#pragma once

#include <vector>
#include <unordered_map>

#define CANTP_PCI_INFO_OFFSET 0

#define CANTP_PCI_SINGLE_FRAME 0
#define CANTP_PCI_FIRST_FRAME 1
#define CANTP_PCI_CONSECUTIVE_FRAME 2
#define CANTP_PCI_FLOWCONTROL_FRAME 3

enum class frameType : uint8_t
{
    CANTP_SINGLE_FRAME = 0,
    CANTP_FIRST_FRAME = 1,
    CANTP_CONSECUTIVE_FRAME = 2,
    CANTP_FLOW_CONTROL_FRAME = 3,
    TOTAL_FRAME_TYPES = 4
};

struct CanTpframeData
{
    uint8_t pciInfo; // Use '+' unary oerator to print uint8_t on console
    uint16_t maxPayloadLength;
};

class Generator
{

public:
    virtual ~Generator(){};

    /**
     * Generate frame
     */
    virtual bool GenerateFrame(frameType frame, uint16_t payloadLength,
                               uint8_t *payload) const = 0;

    /**
     * Read the JSON config file containing list of frames to be generated
     */
    virtual bool ReadConfig() = 0;

    /**
     * Send the generated frame to registered callback
     */
    virtual bool SendFrame() = 0;
};

class CanTpGenerator : public Generator
{
private:
    // Default Parameters
    uint16_t dataLength = 0;
    // std::vector<uint8_t> payload;
    std::unordered_map<frameType, CanTpframeData> canTpFrameStructure;

public:
    CanTpGenerator()
    {
        canTpFrameStructure[frameType::CANTP_SINGLE_FRAME] = {CANTP_PCI_SINGLE_FRAME, 8};
        canTpFrameStructure[frameType::CANTP_FIRST_FRAME] = {CANTP_PCI_FIRST_FRAME, 8};
        canTpFrameStructure[frameType::CANTP_CONSECUTIVE_FRAME] = {CANTP_PCI_CONSECUTIVE_FRAME, 4096};
    }

    ~CanTpGenerator(){};

public:
    virtual bool GenerateFrame(frameType frame, uint16_t payloadLength,
                               uint8_t *payload) const override;
    virtual bool ReadConfig() override;
    virtual bool SendFrame() override;
};
