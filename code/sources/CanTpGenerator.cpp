#include <iostream>
#include <array>
#include <cassert>

#include "CanTpGenerator.h"
#include "CanTpGeneratorUserCfg.h"

#define _WRITE_PCI_INFO(payload, pci_info) \
    payload[0] = 0x0;                      \
    payload[0] |= ((0xF & pci_info) << 4)

int main()
{
    frameType frame = frameType::CANTP_FIRST_FRAME;
    uint16_t payloadLength = 8;
    std::vector<uint8_t> payload(payloadLength, 3);

    _WRITE_PCI_INFO(payload, CANTP_PCI_FIRST_FRAME);
    std::cout << "Size of vector = " << payload.size() << std::endl;

    for (auto &itr : payload)
    {
        std::cout << std::hex << +itr << ", ";
    }

    CanTpGenerator TpGenerator;
    // TpGenerator.GenerateFrame(frame, payloadLength, payload.get());

    // Pseudo Code

    // SetCallbacks();
    // ReadConfig(); - Read from JSON file
    // GenerateFrame()
    // SendFrame()

    return 0;
}

bool CanTpGenerator::GenerateFrame(frameType frame, uint16_t payloadLength,
                                   uint8_t *payload) const
{

    bool success = false;

    auto itr = canTpFrameStructure.find(frame);

    if (itr != canTpFrameStructure.end())
    {
        std::cout << "PCI INFO = " << itr->second.pciInfo << std::endl;
        std::cout << "Max payload Length = " << itr->second.maxPayloadLength << std::endl;
    }
    else
    {
        std::cout << "Frame type not found" << std::endl;
    }

    success = true;

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
