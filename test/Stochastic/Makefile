CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/Stochastic/markov.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/LinAlg/sparse.c \
	../../TuprykEngine/ui/prints/linalg.c \
	../../TuprykEngine/ui/prints/stoch.c \
	../../TuprykEngine/Stochastic/pascal.c \
	../../TuprykEngine/Stochastic/sample.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
