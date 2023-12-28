
#ifndef IINPUT_H
#define IINPUT_H

#include <Arduino.h>
class IOutput;

/// @brief basic interface for any input
class IInput {
public:
    enum Type {
        Analog,
        Digital
    };
    Type type;

    /// @brief constructor
    /// @param type the type of input
    IInput(Type type) : type(type) {}
    virtual ~IInput() {}

    /// @brief initialize the input
    virtual void begin() = 0;

    /// @brief read the input
    /// @return the value of the input
    virtual int read() {
        return onRead();
    }

    /// @brief the basic read function, override it for enhanced functionality
    virtual int onRead() = 0;
};
#endif