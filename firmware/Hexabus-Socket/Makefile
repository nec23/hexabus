all: ipso
	 mv ipso.Hexabus-Socket Hexabus-Socket.elf
	 avr-objcopy -O ihex -R .signature -R .fuse -R .eeprom Hexabus-Socket.elf Hexabus-Socket.hex
	 avr-size -C --mcu=atmega1284p Hexabus-Socket.elf
#	 avr-size -C --mcu=atmega2561 Hexabus-Socket.elf
#	 avr-size -C --mcu=atxmega256d3 Hexabus-Socket.elf
APPS= hexabus-webserver udp_handler mdns_responder temperature datetime_service shutter relay state_machine button hexapush metering shutter relay provisioning value_broadcast hxb_broadcast_handler endpoint_access presence_detector hexonoff analogread i2c humidity pressure ir_receiver  memory_debugger
UIP_CONF_IPV6=1

CONTIKI=../contiki-2.x
#CONTIKI=../contiki-2.4
include $(CONTIKI)/Makefile.include

flash-jtag2: 
	avrdude -c jtag2 -P usb -p atmega1284p -U flash:w:Hexabus-Socket.hex -F 

flash-dragon: 
	avrdude -c dragon_jtag -P usb -p atmega1284p -U flash:w:Hexabus-Socket.hex -F 
