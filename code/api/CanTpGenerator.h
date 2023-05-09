#pragma once

#include <vector>
#include <unordered_map>

#define CANTP_PCI_INFO_OFFSET 0

#define CANTP_PCI_SINGLE_FRAME 0
#define CANTP_PCI_FIRST_FRAME 1
#define CANTP_PCI_CONSECUTIVE_FRAME 2
#define CANTP_PCI_FLOWCONTROL_FRAME 3

enum class CanTpFrames : uint8_t
{
    CANTP_SINGLE_FRAME = 0,
    CANTP_FIRST_FRAME = 1,
    CANTP_CONSECUTIVE_FRAME = 2,
    CANTP_FLOW_CONTROL_FRAME = 3,
    TOTAL_FRAME_TYPES = 4
};

struct CanTpProtocolData
{
    uint8_t pciInfo; // Use '+' unary oerator to print uint8_t on console
    uint16_t maxPayloadLength;
};

class FrameGenerator
{

public:
    virtual ~FrameGenerator(){};

    /**
     * @brief Generated Frame based on @param frameType
     *
     * @param frameType         Type of frame to be generated
     * @param payloadLength     Length of paylaod in the frame (Protocol related data)
     * @param defaultFillValue  Default value to be filled as payload
     * @param frame             Frame to which values have to be filled
     * @return bool
     */
    virtual bool GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, uint8_t defaultFillValue,
                               std::vector<uint8_t> &frame) const = 0;

    /**
     * Read the JSON config file containing list of frames to be generated
     */
    virtual bool ReadConfig() = 0;

    /**
     * Send the generated frame to registered callback
     */
    virtual bool SendFrame() = 0;
};

class CanTpGenerator : public FrameGenerator
{
private:
    // Default Parameters
    uint16_t dataLength = 0;
    // std::vector<uint8_t> payload;
    std::unordered_map<CanTpFrames, CanTpProtocolData> CanTpFrameFormat;

public:
    CanTpGenerator()
    {
        CanTpFrameFormat[CanTpFrames::CANTP_SINGLE_FRAME] = {CANTP_PCI_SINGLE_FRAME, 7};
        CanTpFrameFormat[CanTpFrames::CANTP_FIRST_FRAME] = {CANTP_PCI_FIRST_FRAME, 4096};
        CanTpFrameFormat[CanTpFrames::CANTP_CONSECUTIVE_FRAME] = {CANTP_PCI_CONSECUTIVE_FRAME, 4096};
    }

    ~CanTpGenerator(){};

public:
    virtual bool GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, uint8_t defaultFillValue,
                               std::vector<uint8_t> &frame) const override;
    virtual bool ReadConfig() override;
    virtual bool SendFrame() override;
};
