
# This is a simple wrapper around SCons for convenience

all: tools/scons.py
	python tools/scons.py

clean:
	rm -rf build

distclean: clean
	rm -rf .scon* config.log simulations.conf
	cd tools; rm -rf scons-LICENSE scons-README scons-local-0.98.4 scons*.py

tools/scons.py:
	cd tools; tar zxvf scons-local-0.98.4.tar.gz
