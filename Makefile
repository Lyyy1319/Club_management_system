CC=gcc
CFLAGS=-Wall -Wextra -g -std=c11
SRCS=main.c auth.c club.c finance.c utils.c activit.c
OBJS=$(SRCS:.c=.o)
TARGET=club_system

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
