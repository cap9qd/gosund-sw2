Gosund SW2 (new hardware with BK7231T) Implementation for ESP Home
=======================

NOTE: Doubt this is backward compatible with the older ESP8266-based SW2 switches.

Features
--------
- Reading of the touch sensor
- Integration via both ESP Home Native API
- White LED turns on when the light is on
- Green LED flashes when first powered until wifi connects
- Works in libre-tiny with new BK7231T based SW2 switches.

To use
------
- Update devices/secrets.yaml
- Update example_sw2.yaml with the id and friendly name for your switch.  The id will be used as the hostname and id in Home Assistant.
- Rename example_sw2.yaml
- Consider disabling the web interface in devices/gosund_sw2_light.yaml
- Compile with esphome
- Flash