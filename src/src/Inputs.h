
#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>
#include <Button.h>
#include <IOutput.h>
#include <AbstractDigitalInput.h>

class InputGpio : public IInput {
    pin_size_t gpio;
public:
    InputGpio(pin_size_t gpio) : IInput(Digital), gpio(gpio) {
        pinMode(gpio, INPUT);
    }

    void begin() {}

    int read() {
        return digitalRead(gpio);
    }
};

class InputButton : public IInput {
    Button button;
    bool buffer = false;
    bool toggle = false;
    IOutput* output = nullptr;

public:
    InputButton(uint8_t pin, bool toggle) : IInput(Digital), button(pin), toggle(toggle) {
        button.begin();
    }

    void connectOutput(IOutput* output) {
        this->output = output;
    }

    void begin() override {
        if(output) {
            output->write(buffer);
        }
    }

    int onRead() override {
        bool changed = false;
        if(toggle) {
            if(button.released()) {
                buffer = !buffer;
                changed = true;
            }
        } else {
            if(button.pressed()) {
                buffer = true;
                changed = true;
            }
            if(button.released()) {
                buffer = false;
                changed = true;
            }
        }

        if(changed) {
            if(output) {
                output->write(buffer);
            }
        }

        return buffer;
    }
};

class InputAnalog : public IInput {
    pin_size_t pin;
public:
    InputAnalog(pin_size_t pin) : IInput(Digital), pin(pin) {}

    void begin() override {}

    int onRead() override {
        return analogRead(pin);
    }
};

#endif // INPUTS_H