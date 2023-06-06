#pragma once

#include <vector>
#include <unordered_map>

#define CANTP_PCI_INFO_OFFSET 0
#define CANTP_FF_DL_INFO_OFFSET 1

#define CANTP_PCI_SINGLE_FRAME 0
#define CANTP_PCI_FIRST_FRAME 1
#define CANTP_PCI_CONSECUTIVE_FRAME 2
#define CANTP_PCI_FLOWCONTROL_FRAME 3

#define CANTP_MAX_PAYLOAD_LENGTH 4096

#define CANTP_PAYLOAD_BYTES_IN_SF 7
#define CANTP_PAYLOAD_BYTES_IN_FF 6
#define CANTP_PAYLOAD_BYTES_IN_CF 7

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
    uint8_t pciInfo;                                                               // Use '+' unary oerator to print uint8_t on console
    void (*pfWritePayload)(uint16_t payloadLength, std::vector<uint8_t> &payload); // Function to be called to generate payload
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
    const uint8_t CANTP_FRAME_LENGTH = 8; // not FD
    uint8_t m_defaultFill = 0xcc;
    std::unordered_map<CanTpFrames, CanTpProtocolData> CanTpFrameFormat;

public:
    CanTpGenerator();

    ~CanTpGenerator(){};

public:
    virtual bool GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, uint8_t defaultFillValue,
                               std::vector<uint8_t> &frame) const override;
    virtual bool ReadConfig() override;
    virtual bool SendFrame() override;

    /**
     * @brief
     *
     * @param payloadLength
     * @param defaultFillValue
     * @param frame - Pass an empty vector as input
     * @param seqNum - Used only for consecutive frame
     * @return true
     * @return false
     */
    bool GenerateFrame(uint16_t payloadLength, std::vector<std::vector<uint8_t>> &frame);

    void SetDefaultFillValue(uint8_t defaultFill)
    {
        m_defaultFill = defaultFill;
    }

private:
    void GeneratePayload(CanTpFrames frameType, uint16_t payloadLength, std::vector<uint8_t> &payload, uint8_t seqNum = 0xFF);

    uint16_t RequiredFrames(uint16_t msgLength);
};