CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/Algos/lists.c \
	../../TuprykEngine/Algos/spatial_data.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/visual/prints/linalg.c \
	../../TuprykEngine/visual/graphics/basic.c \
	../../TuprykEngine/visual/graphics/window.c \
	../../TuprykEngine/Stochastic/sample.c \
	../../TuprykEngine/visual/prints/algos.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
