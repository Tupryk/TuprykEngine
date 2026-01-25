CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRCS = main.c ../../TuprykEngine/LinAlg/tensor.c ../../TuprykEngine/visual/prints/linalg.c ../../TuprykEngine/Optim/unconstrained/gradient_descent.c ../../TuprykEngine/Optim/unconstrained/newton.c ../../TuprykEngine/Optim/functions.c ../../TuprykEngine/Optim/meta.c ../../TuprykEngine/visual/graphics/basic.c ../../TuprykEngine/visual/graphics/window.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
