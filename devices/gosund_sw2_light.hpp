#pragma once
#include "esphome.h"

namespace esphome
{
    namespace gosund
    {
        class GosundLight : public Component, public light::LightOutput, public uart::UARTDevice {

            public:
                GosundLight(output::BinaryOutput *status_led, uart::UARTComponent *uart) : UARTDevice(uart)
                { 
                    status_led_ = status_led;
                }
                void setup() override;
                void loop() override;
                light::LightTraits get_traits() override;
                void setup_state(light::LightState *state) override { state_ = state; }
                void write_state(light::LightState *state) override;

            protected:
                light::LightState *state_{nullptr};
                output::BinaryOutput *status_led_;
        };

    } // namespace gosund
} // namespace esphome
