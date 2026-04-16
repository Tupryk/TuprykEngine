CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRCS = main.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/visual/prints/linalg.c \
	../../TuprykEngine/visual/graphics/window.c \
	../../TuprykEngine/visual/graphics/basic.c \
	../../TuprykEngine/Kinematics/configuration.c \
	../../TuprykEngine/Kinematics/frame.c \
	../../TuprykEngine/Graphics/raytracer.c \
	../../TuprykEngine/visual/prints/config.c \
	../../TuprykEngine/scenes/devastator/desert.c \
	../../TuprykEngine/Graphics/image.c \
	../../TuprykEngine/Algos/lists.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
