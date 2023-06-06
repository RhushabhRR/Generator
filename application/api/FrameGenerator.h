#pragma once

#include <vector>

static const uint8_t CanTpFrameLength = 8;
static const uint16_t CanTpMaxMsgLength = 4096;

void GenerateSingleFrame(uint16_t msgLength, std::vector<uint8_t> &payload);
void GenerateFirstFrame(uint16_t msgLength, std::vector<uint8_t> &payload);
void GenerateConsecutiveFrame(uint16_t seqNum, std::vector<uint8_t> &payload);