TARGET = invasion
OBJS = snowday.o firealarm.o covid.o
CFLAGS = -g
LDLIBS = -lm

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS)

clean:
	rm -f $(TARGET) $(OBJS) core*
