CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/Algos/lists.c \
	../../TuprykEngine/Algos/spatial_data.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/ui/prints/linalg.c \
	../../TuprykEngine/ui/graphics/basic.c \
	../../TuprykEngine/ui/graphics/window.c \
	../../TuprykEngine/Stochastic/sample.c \
	../../TuprykEngine/ui/prints/algos.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
