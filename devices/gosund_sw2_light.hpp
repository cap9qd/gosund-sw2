#pragma once
#include "esphome.h"

namespace esphome
{
    namespace gosund
    {
        class GosundLight : public Component, public light::LightOutput, public uart::UARTDevice {

            public:
                GosundLight(output::BinaryOutput *status_led, uart::UARTComponent *uart, int _mcuVer = 2, bool _debugPrint = false) : UARTDevice(uart)
                { 
                    status_led_ = status_led;
                    mcuVer = _mcuVer;
                    debugPrint = _debugPrint;
                    if(mcuVer > 1)
                        uint32_t touchCompare = 0x24016423;
                }
                void setup() override;
                void loop() override;
                light::LightTraits get_traits() override;
                void setup_state(light::LightState *state) override { state_ = state; }
                void write_state(light::LightState *state) override;

            protected:
                light::LightState *state_{nullptr};
                output::BinaryOutput *status_led_;
                int mcuVer = 0;
                bool debugPrint = false;
                //MCU v1 = 0x24 0xYY 0x01 0x1E 0x23 where 0xYY is the dimmer value
                //MCU v2 = 0x24 0xYY 0x01 0x64 0x23 where 0xYY is the dimmer value
                uint32_t touchCompare = 0x24011E23;
        };

    } // namespace gosund
} // namespace esphome
