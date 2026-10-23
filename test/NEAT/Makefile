CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/LinAlg/sparse.c \
	../../TuprykEngine/ui/prints/linalg.c \
	../../TuprykEngine/ui/prints/vis_neat.c \
	../../TuprykEngine/ui/prints/algos.c \
	../../TuprykEngine/NEAT/population.c \
	../../TuprykEngine/NEAT/agent.c \
	../../TuprykEngine/Funcs/funcs.c \
	../../TuprykEngine/Optim/meta.c \
	../../TuprykEngine/Algos/lists.c \
	../../TuprykEngine/Stochastic/sample.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
