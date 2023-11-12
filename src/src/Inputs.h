
#ifndef INPUTS_H
#define INPUTS_H

#include <Arduino.h>
#include <Button.h>
#include <Outputs.h>

class IInput {
    public:
    enum Type {
        Analog,
        Digital
    };
    Type type;
    IInput(Type type) : type(type) {
    }
    virtual ~IInput() {
    }
 
    virtual void begin() = 0;
    virtual int read() = 0;
};

class InputGpio : public IInput {
    pin_size_t gpio;
    public:
    InputGpio(pin_size_t gpio) : IInput(Digital), gpio(gpio) {
        pinMode(gpio, INPUT); 
    }

    void begin() {

    }

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

    InputButton(uint8_t pin, bool toggle, IOutput* output) : IInput(Digital), button(pin), toggle(toggle), output(output) {
        button.begin();
    }

    void begin() {
        if (output) {
            output->write(buffer);
        }       
    }

    int read() {
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

        if (changed) {
            if (output) {
                output->write(buffer);
            }
        }

        return buffer;
    }
};

class InputAnalog : public IInput {
    pin_size_t pin;
    public:
    InputAnalog(pin_size_t pin) : IInput(Digital), pin(pin) {
    }

    void begin() {
    }

    virtual int read() {
        return analogRead(pin);
    }
};

#endif // INPUTS_H