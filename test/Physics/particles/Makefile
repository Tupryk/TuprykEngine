CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../../TuprykEngine/LinAlg/tensor.c \
	../../../TuprykEngine/ui/prints/linalg.c \
	../../../TuprykEngine/Physics/particle.c \
	../../../TuprykEngine/ui/graphics/window.c \
	../../../TuprykEngine/ui/graphics/basic.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
