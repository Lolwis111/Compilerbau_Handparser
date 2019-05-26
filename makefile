TARGET := minako
LFILES := minako-lexic.l
CFILES := minako-syntax.c

CC     := gcc
LEX    := flex
CFLAGS := -g -Wall -pedantic -Wunused-result
LFLAGS := -t

%.c: %.l
	$(LEX) $(LFLAGS) $< > $@
%.o: %.c %.h
	$(CC) $(CFLAGS) $< -c
%.o: %.c
	$(CC) $(CFLAGS) $< -c

all: $(TARGET)

$(TARGET): $(LFILES:%.l=%.o) $(CFILES:%.c=%.o)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(RMFLAGS) $(TARGET) $(TARGET:%=%.exe) $(LFILES:%.l=%.o) $(CFILES:%.c=%.o)
