#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>
#include <TCA9534.h>
#include <PCF8574.h>
#include <IOutput.h>

class OutputGpio : public AbstractDigitalOutput {
    pin_size_t gpio;
public:
    OutputGpio(pin_size_t gpio, bool inverted) : AbstractDigitalOutput(inverted), gpio(gpio) {}

    void begin() {
        pinMode(gpio, OUTPUT);
    }

    void doWrite(int value) {
        digitalWrite(gpio, value);
    }
};

class OutputTCA9534 : public AbstractDigitalOutput {
    TCA9534* ioex;
    uint8_t pin;

public:
    OutputTCA9534(TCA9534* ioex_, uint8_t pin, bool inverted) : AbstractDigitalOutput(inverted), ioex(ioex_), pin(pin) {}

    void begin() {
        ioex->output(pin, 0);
        ioex->config(pin, TCA9534::Config::OUT);
    }

    void doWrite(int value) {
        ioex->output(pin, (value == 0) ? TCA9534::Level::L : TCA9534::Level::H);
    }
};

class OutputPCF8574 : public AbstractDigitalOutput {
    PCF8574* ioex;
    uint8_t pin;

public:
    OutputPCF8574(PCF8574* ioex, uint8_t pin, bool inverted) : AbstractDigitalOutput(inverted), ioex(ioex), pin(pin) {}

    void begin() {
        ioex->output(pin, 0);
    }

    void doWrite(int value) {
        ioex->output(pin, value);
    }
};

class OutputAnalog : public IOutput {
    pin_size_t pin;
public:
    OutputAnalog(pin_size_t pin) : IOutput(Digital), pin(pin) {}

    void begin() {}

    virtual void doWrite(int value) {
        analogWrite(pin, value);
    }
};

#endif //OUTPUTS_H