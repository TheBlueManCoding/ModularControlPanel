#ifndef IOUTPUT_H
#define IOUTPUT_H

#include <Arduino.h>
/// @brief interface definition of an output
class IOutput {
public:
    enum Type {
        Analog,
        Digital
    };

    IOutput(Type type) : type(type) {}
    virtual ~IOutput() {}

    /// @brief initialize the output
    virtual void begin() = 0;

    /// @brief write the value
    /// @param value the value
    virtual void write(int value) {
        doWrite(value);
    }

private:
    Type type;

protected:
    virtual void doWrite(int value) = 0;
};
#endif