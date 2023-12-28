#ifndef AbstractDigitalOutput_H
#define AbstractDigitalOutput_H

#include <IOutput.h>

/// @brief base class for a digital output that automatically supports the pin invert
class AbstractDigitalOutput : public IOutput {
    bool inverted = false;
    bool lastState = false;
public:
    AbstractDigitalOutput(bool inverted) : IOutput(Digital), inverted(inverted) {}

    /// @brief write a new status and invert it if defined so
    /// @param value the value, 1 for ON - everything else will set the status to false
    void write(int value) override {
        bool v = value == 1;
        if(inverted) {
            v = !v;
        }
        if(v != lastState) {
            lastState = v;
            doWrite(v);
        }
    }
};

#endif