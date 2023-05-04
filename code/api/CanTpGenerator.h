#pragma once

class Generator
{

public:
    virtual ~Generator();

    /**
     * Generate frame
     */
    virtual bool GenerateFrame() = 0;

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

public:
    virtual bool GenerateFrame() override;
    virtual bool ReadConfig() override;
    virtual bool SendFrame() override;
};
