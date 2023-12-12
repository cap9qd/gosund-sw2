#pragma once
#include "esphome.h"

namespace esphome
{
    namespace gosund
    {
        class GosundLight : public Component, public light::LightOutput, public uart::UARTDevice {

            public:
                GosundLight(output::BinaryOutput *status_led, uart::UARTComponent *uart, uint8_t _mcuVer = 2, bool _debugPrint = false) : UARTDevice(uart)
                { 
                    status_led_ = status_led;
                    mcuVer = _mcuVer;
                    debugPrint = _debugPrint;
                    
                    // Set compare; only check last 3 elements later and use first as "msg ready" indication...
                    switch(mcuVer)
                    {
                        case 0x01:
                            MAX_VALUE = 100.0;
                            break;
                        case 0x02:
                            MAX_VALUE = 150.0;
                            break;
                        default:
                            setupError = true;
                            break;
                    }
                }
                
                void setup() override;
                void loop() override;
                light::LightTraits get_traits() override;
                void setup_state(light::LightState *state) override { state_ = state; }
                void write_state(light::LightState *state) override;
                void dump_config();

            protected:
                light::LightState *state_{nullptr};
                output::BinaryOutput *status_led_;
                uint8_t mcuVer = 0;
                bool debugPrint = false;
                
                const char *TAG = "gosund.light.sw2";
                const byte ON_MASK = 0x80;

                const uint8_t MAX_PERCENT = 100;
                const uint8_t MIN_PERCENT = 1;

                float MAX_VALUE = 100.0;

                bool setupError = 0;
        };

    } // namespace gosund
} // namespace esphome
