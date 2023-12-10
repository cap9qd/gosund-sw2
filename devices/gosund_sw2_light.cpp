//For the BK7231T version (mcu v2?) I used reverse engineering info from: 
//   https://www.elektroda.com/rtvforum/topic4014774.html#20825305

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
            static uint8_t pos = 0;
            static uint8_t setupSync = 0;
            static uint8_t mPos = 16;
            static uint8_t tBuffer[16];
            static float   dimmerVal = 0.0;
            
            if(available() > 0)
            {
                if(pos < mPos - 2)
                {    
                    read_byte(&tBuffer[pos++]);
                    if(debugPrint)
                        ESP_LOGD(TAG, "READ BYTE! 0x%02X", tBuffer[pos-1]);
                }
                else
                {
                    pos = mPos - 2;
                    for(int i = 0;i<mPos-2;i++)
                        tBuffer[i] = tBuffer[i+1];
                    read_byte(&tBuffer[pos]);
                    
                    if(debugPrint && (tBuffer[0] == 0x24))
                        ESP_LOGD(TAG, "READ BYTE! 0x%02X %02X %02X %02X %02X", tBuffer[0], tBuffer[1], tBuffer[2], tBuffer[3], tBuffer[4]);
                }
                tBuffer[pos+1] = '\0';
                
                //MCU v1 = 0x24 0xYY 0x01 0x1E 0x23 where 0xYY is the dimmer value
                //MCU v2 = 0x24 0xYY 0x01 0x64 0x23 where 0xYY is the dimmer value
                if(tBuffer[0] == 0x24)
                {
                    uint32_t tCompare = (tBuffer[0] << 24 ) \
                                      + (tBuffer[2] << 16 ) \
                                      + (tBuffer[3] << 8 ) \
                                      +  tBuffer[4];
                    
                    if(printDebug)
                        ESP_LOGD(TAG, "Dimmer Compare 0x%08X", tCompare);
                    
                    if(tCompare == touchCompare)
                    {
                        dimmerVal = tBuffer[1] / 150.0; //Brightness returned is 0x01 - 0x96
                        setupSync = 1;
                        memset(&tBuffer[0], 0, 5);
                        if(debugPrint)
                            ESP_LOGD(TAG, "Received dimmer value: %01.1f", dimmerVal*100);
                    
                        auto call = state_->make_call();
                        // Touch sensor only works when turned on
                        call.set_state(true);
                        call.set_brightness(dimmerVal);
                        call.perform();
                    }
                }
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
                if(debugPrint)
                    ESP_LOGD(TAG, "turning on status LED");
                
                output |= 0x80; //Indicates output message
                
                if(mcuVer == 2)
                {
                    ledOut |= 0x40; //Indicates LED message
                    write_byte(ledOut);
                }
                write_byte(output);
            }
            else
            {
                status_led_->turn_off();
                if(debugPrint)
                    ESP_LOGD(TAG, "turning off status LED");
                
                //write(0x41);
                write(0x01); //Tell it how many LED to keep on during off
            }
            if(debugPrint)
                if(mcuVer == 2)
                    ESP_LOGD(TAG, "write_state() called with state: %0.1f, brightness: %.02f => output: %02X and LED: %02X", values.get_state(), values.get_brightness(), output, ledOut);
                else
                    ESP_LOGD(TAG, "write_state() called with state: %0.1f, brightness: %.02f => output: %02X", values.get_state(), values.get_brightness(), output);
        }
    } // namespace gosund
} // namespace esphome
