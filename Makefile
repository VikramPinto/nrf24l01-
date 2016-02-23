all: nrf

nrf: nrffn.o nrfmain.o
	arm-cortexa8-linux-gnueabihf-gcc nrfmain.o nrffn.o -o nrf
nrfmain.o: nrfmain.c
	arm-cortexa8-linux-gnueabihf-gcc -c nrfmain.c
nrffn.o: nrffn.c
	arm-cortexa8-linux-gnueabihf-gcc -c nrffn.c
clean:
	rm -rf *.o nrf
