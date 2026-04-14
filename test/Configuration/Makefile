CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRCS = main.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/Configuration/frame.c \
	../../TuprykEngine/visual/prints/linalg.c \
	../../TuprykEngine/Configuration/configuration.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
