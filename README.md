Gosund SW2 Implementation for ESP Home
=======================

Features
--------
- Reading of the touch sensor
- Integration via both ESP Home Native API & MQTT
- Additional MQTT topic for setting the brightness without changing the state of the light
- White LED turns on when the light is on
- Green LED flashes when first powered until wifi connects


To use
------
- Update devices/secrets.yaml
- Update example_sw2.yaml with the id and friendly name for your switch.  The id will be used as the hostname and id in Home Assistant.
- Rename example_sw2.yaml
- Consider disabling the web interface in devices/gosund_sw2_light.yaml
- Compile with esphome
- Flash