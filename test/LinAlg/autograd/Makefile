CC = gcc
CFLAGS = -Wall -Wextra -g

SRCS = main.c \
	../../../TuprykEngine/LinAlg/tensor.c \
	../../../TuprykEngine/LinAlg/sparse.c \
	../../../TuprykEngine/LinAlg/gtensor.c \
	../../../TuprykEngine/Stochastic/sample.c \
	../../../TuprykEngine/Algos/lists.c \
	../../../TuprykEngine/ui/prints/linalg.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
