CFLAGS = -std=c17 -Wpedantic -Wall -O2 -pipe -fno-plt -fPIC
LDFLAGS = -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

.PHONY: all
all: execve-example fork-example orphan-example zombie-example wait-example

execve-example: execve-example.o

fork-example: fork-example.o

orphan-example: orphan-example.o

zombie-example: zombie-example.o

wait-example: wait-example.o

.PHONY: clean
clean:
	rm -f execve-example.o execve-example
	rm -f fork-example.o fork-example
	rm -f orphan-example.o orphan-example
	rm -f zombie-example.o zombie-example
	rm -f wait-example.o wait-example
