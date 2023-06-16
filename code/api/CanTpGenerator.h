#pragma once

#include <vector>
#include <unordered_map>

#include "CanTpGeneratorUserCfg.h"

// Offsets
#define CANTP_PCI_INFO_OFFSET 0
#define CANTP_FF_DL_INFO_OFFSET 1
#define CANTP_FC_CONTROLFLOW_OFFSET 0
#define CANTP_FC_BLOCKSIZE_OFFSET 1
#define CANTP_FC_STMIN_OFFSET 2

// PCI info
#define CANTP_PCI_SINGLE_FRAME 0
#define CANTP_PCI_FIRST_FRAME 1
#define CANTP_PCI_CONSECUTIVE_FRAME 2
#define CANTP_PCI_FLOWCONTROL_FRAME 3

// Payload Lengths
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

typedef void (*pfGenerator)(uint16_t, std::vector<uint8_t> &);

struct CanTpFrameInfo
{
    uint8_t pciInfo;
    pfGenerator pfGenerateFrame; // Custom Function to generate frame
};

class FrameGenerator
{

public:
    virtual ~FrameGenerator() {}

    /**
     * @brief Generates a complete message consisting of single/multiple frames depending on @ref msgLength
     *
     * @param msgLength - Length of overall message to be generated
     * @param msg - Provide an empty 2D Vector - [frame][data] - will be resized inside the method
     * @return true
     * @return false
     */
    virtual bool GenerateMsg(uint16_t msgLength, std::vector<std::vector<uint8_t>> &msg) = 0;

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
    std::unordered_map<CanTpFrames, CanTpFrameInfo> m_canTpFrames;

    uint8_t m_fcFlag = 0;
    uint8_t m_blockSize = CANTP_DEFAULT_BLOCK_SIZE;
    uint8_t m_stmin = CANTP_DEFAULT_SEPERATION_TIME; // ms
    uint8_t m_defaultFill = CANTP_DEFAULT_FILL_VALUE;

private: // helper
    uint16_t RequiredFrames(uint16_t msgLength);

public:
    CanTpGenerator();
    ~CanTpGenerator(){};

    /**
     * @brief Generates a complete message consisting of single/multiple frames depending on @ref msgLength
     *
     * @param msgLength - Length of overall message to be generated
     * @param msg - Provide an empty 2D Vector - [frame][data] - will be resized inside the method
     * @return true
     * @return false
     */
    virtual bool GenerateMsg(uint16_t msgLength, std::vector<std::vector<uint8_t>> &msg) override;

    /**
     * @brief Unused - Intended for future use
     *
     * @return true
     * @return false
     */
    virtual bool ReadConfig() override;

    /**
     * @brief Unused - Intended for future use
     *
     * @return true
     * @return false
     */
    virtual bool SendFrame() override;

    /**
     * @brief Set the Config Parameters used for Generation of Flow Control Frame
     *
     * @param fcFlag - Control Flow flag (0/1/2)
     * @param blockSize - BlockSize
     * @param stmin - Minimum Separation Time in ms
     */
    void SetConfigParam(uint8_t fcFlag, uint8_t blockSize, uint8_t stmin);

    /**
     * @brief Generate a single CanTp Frame based on @ref frameType
     *
     * @param frameType - Type of CanTp frame to be generated
     * @param payloadLength - Length of payload in corresponding CanTp frame.
     * @note - For flow control frame - @ref payloadLength is unused
     * @param payload - an empty vector in which frame will be generated
     * @param seqNum - Used only for Consecutive frame
     * @return true
     * @return false
     */
    bool GenerateFrame(CanTpFrames frameType, uint16_t payloadLength, std::vector<uint8_t> &payload, uint8_t seqNum = 0xFF);

    /**
     * @brief Set the Default Fill Value for CanTp frame in case payloadLength < CanTp frame length (8 bytes)
     *
     * @param defaultFill - fill pattern
     */
    void SetDefaultFillValue(uint8_t defaultFill)
    {
        m_defaultFill = defaultFill;
    }

    /**
     * @brief Set the Custom Frame Generator function, to generate custom payload
     * @note PCI info and other protocol related has to be taken care by the function itself
     *
     * @param frameType CanTp Frame Type
     * @param pfGenerateFrame Custom GenerateFrame() method for corresponding @ref frameType
     */
    void SetCustomFrameGenerator(CanTpFrames frameType, pfGenerator pfGenerateFrame);
};
