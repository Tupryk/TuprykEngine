CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRCS = main.c ../../TuprykEngine/Stochastic/markov.c ../../TuprykEngine/LinAlg/tensor.c ../../TuprykEngine/LinAlg/sparse.c ../../TuprykEngine/visual/prints/linalg.c ../../TuprykEngine/visual/prints/stoch.c ../../TuprykEngine/Stochastic/pascal.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
