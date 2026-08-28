CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/LinAlg/*.c \
	../../TuprykEngine/ui/prints/*.c \
	../../TuprykEngine/ui/graphics/*.c \
	../../TuprykEngine/Kinematics/*.c \
	../../TuprykEngine/Graphics/*.c \
	../../TuprykEngine/scenes/devastator/desert.c \
	../../TuprykEngine/Algos/*.c \
	../../TuprykEngine/Stochastic/*.c \
	../../TuprykEngine/Geom/*.c \
	../../TuprykEngine/Physics/*.c

OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
