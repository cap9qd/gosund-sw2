//Used reverse engineering info from: https://www.elektroda.com/rtvforum/topic4014774.html#20825305

#include "gosund_sw2_light.hpp"

namespace esphome
{
    namespace gosund
    {
        static const char *TAG = "gosund.light.sw2";
        static const byte ON_MASK = 0x80;

        static const uint8_t MIN_VALUE = 0;
        static const uint8_t MAX_VALUE = 100;
        static const uint8_t MIN_PERCENT = 1;
        
        void GosundLight::setup() {}

        void GosundLight::loop()
        {
            // pattern is 0x24 0xYY 0x01 0x1E 0x23, where YY is dimmer value
            if (available() >= 5)
            {
                uint8_t buff[5];

                //
                read_array(&buff[0], 5);
                
                if (available() > 0)
                {
                    ESP_LOGD(TAG, "Unexpected bytes on uart_. ");
                    ESP_LOGD(TAG, "  Expected: %02X %02X %02X %02X %02X", buff[0], buff[1], buff[2], buff[3], buff[4]);
                    ESP_LOGD(TAG, "  Expected: %02X %02X %02X %02X %02X", buff[0], buff[1], buff[2], buff[3], buff[4]);

                    while (available() >= 1)
                    {
                        read_byte(&buff[0]);
                        ESP_LOGD(TAG, "  Unexpected: %02X", buff[0]);
                    }
                }

                float dimmerVal = buff[1] / 150.0; //Brightness returned is 0x01 - 0x96

                auto call = state_->make_call();
                // Touch sensor only works when turned on
                call.set_state(true);
                call.set_brightness(dimmerVal);
                call.perform();

                ESP_LOGD(TAG, "Received dimmer value %02X %d", buff[1], buff[1]);
            }
        }
      
        light::LightTraits GosundLight::get_traits()
        {
            auto traits = light::LightTraits();
            traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
            return traits;
        }
  
        void GosundLight::write_state(light::LightState *state)
        {
            auto values = state->current_values;
            uint8_t output = std::min(MAX_VALUE, (uint8_t) (100.0 * values.get_brightness()));
            output = std::max(MIN_PERCENT, output);
            
            uint8_t ledOut = std::max(MIN_PERCENT, (uint8_t) (values.get_brightness() * 7.0));
            
            if (values.get_state() > 0 && values.get_brightness() > 0)
            {
                status_led_->turn_on();
                ESP_LOGD(TAG, "turning on status LED");
                
                ledOut |= 0x40; //Indicates LED message
                output |= 0x80; //Indicates output message
                
                write_byte(ledOut);
                write_byte(output);
            }
            else
            {
                status_led_->turn_off();
                ESP_LOGD(TAG, "turning off status LED");
                
                //write(0x41);
                write(0x01); //Tell it how many LED to keep on during off
            }

            ESP_LOGD(TAG, "write_state() called with state: %0.1f, brightness: %.02f => output: %02X and LED: %02X", values.get_state(), values.get_brightness(), output, ledOut);
            
            flush();
        }
    } // namespace gosund
} // namespace esphome
