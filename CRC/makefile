TARGET := crc

OBJDIRS := obj
SRCDIRS := src
SOURCES := $(wildcard $(SRCDIRS)/*.c)
OBJECTS := $(patsubst $(SRCDIRS)/%.c, $(OBJDIRS)/%.o, $(SOURCES))

CC       := gcc
CPPFLAGS := -I./$(SRCDIRS)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(CPPFLAGS)

$(OBJDIRS)/%.o: ./src/%.c
	@test -d $(OBJDIRS) || mkdir $(OBJDIRS)
	$(CC) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJDIRS)