ONAME= dwm_status
INSTALL_DIR= /usr/bin

CC= c99
CPPFLAGS= -D_POSIX_C_SOURCE=199309L -D_DEFAULT_SOURCE
LDFLAGS= -lX11
CFLAGS= -g -O0 -Wall -Wextra -pedantic $(CPPFLAGS)

OBJ= dwm_status.o

.PHONY: all clean install uninstall

all: build

build: $(OBJ)
	$(CC) $(OBJ) -o $(ONAME) $(LDFLAGS)
%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
install:
	cp $(ONAME) $(INSTALL_DIR)/$(ONAME)
uninstall:
	rm $(INSTALL_DIR)/$(ONAME)
clean:
	rm -rf *.o $(ONAME)
