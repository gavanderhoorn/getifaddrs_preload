
.PHONY: clean all

all: _getifaddrs.so

_getifaddrs.so: _getifaddrs.c
	$(CC) -shared -fPIC -g _getifaddrs.c -o _getifaddrs.so

clean:
	@rm -f _getifaddrs.so
