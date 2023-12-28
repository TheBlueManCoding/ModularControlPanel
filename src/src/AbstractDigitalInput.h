
#ifndef AbstractDigitalInput_H
#define AbstractDigitalInput_H

#include <IInput.h>
#include <IOutput.h>

/// @brief abstract base for digitel inputs which can connecct an output to view the state
class AbstractDigitalInput : public IInput {
    bool buffer = false;

protected:
    IOutput* output = nullptr;

public:
    AbstractDigitalInput() : IInput(Digital) {}

    /// @brief connect a output to the input, it will automaticalle be changed with the input
    /// @param output the output
    void connectOutput(IOutput* output) {
        this->output = output;
    }

    /// @brief initialize the input
    void begin() override {
        if(output) {
            output->write(buffer);
        }
    }

    /// @brief the basic read function
    virtual int onRead() = 0;

private:
    int read() final {
        bool state = onRead();

        if(state != buffer) {
            buffer = state;
            if(output) {
                output->write(buffer);
            }
        }

        return buffer;
    }
};

#endif