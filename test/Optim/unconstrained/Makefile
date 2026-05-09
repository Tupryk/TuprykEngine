CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRCS = main.c \
	../../../TuprykEngine/LinAlg/tensor.c \
	../../../TuprykEngine/visual/prints/linalg.c \
	../../../TuprykEngine/Optim/unconstrained/gradient_descent.c \
	../../../TuprykEngine/Optim/unconstrained/newton.c \
	../../../TuprykEngine/Optim/meta.c \
	../../../TuprykEngine/visual/graphics/basic.c \
	../../../TuprykEngine/visual/graphics/window.c \
	../../../TuprykEngine/Optim/constrained/augmented_lagrangian.c \
	../../../TuprykEngine/Optim/constrained/nlp.c \
	../../../TuprykEngine/visual/graphics/basic.c \
	../../../TuprykEngine/Optim/constrained/sampling/nhr.c \
	../../../TuprykEngine/Optim/constrained/sampling/mcmc.c \
	../../../TuprykEngine/Optim/constrained/sampling/masem.c \
	../../../TuprykEngine/Stochastic/sample.c \
	../../../TuprykEngine/Algos/spatial_data.c \
	../../../TuprykEngine/Algos/utils.c \
	../functions.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
