SHELL = /bin/sh

all:
	make -f logging/Makefile
	make -f network/Makefile
	make -f object_pool/Makefile
	make -f prop/Makefile
	make -f thread/Makefile
	make -f util/Makefile

thread:
	make -f thread/Makefile

clean:
	make -f logging/Makefile clean
	make -f network/Makefile clean
	make -f object_pool/Makefile clean
	make -f prop/Makefile clean
	make -f thread/Makefile clean
	make -f util/Makefile clean