default: compile

compile:
	codeblocks --rebuild avr_keypad_4x4_lib.cbp
clean:
	rm -rf ./obj ./bin
upload:
	avrdude -p m32 -c usbasp -U flash:w:./bin/Release/avr_keypad_4x4_lib.hex:i
communicate:
	picocom -b 76800 /dev/ttyUSB0
