CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = x3d-toggle-c
SRC = x3d-toggle.c
PREFIX = /usr
BINDIR = $(DESTDIR)$(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -d $(BINDIR)
	install -m 755 $(TARGET) $(BINDIR)/$(TARGET)
	@echo "Installed to $(BINDIR)/$(TARGET)"

uninstall:
	rm -f $(BINDIR)/$(TARGET)
	@echo "Uninstalled from $(BINDIR)/$(TARGET)"

.PHONY: all clean install uninstall
