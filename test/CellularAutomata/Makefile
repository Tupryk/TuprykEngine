CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

SRCS = main.c \
	../../TuprykEngine/LinAlg/tensor.c \
	../../TuprykEngine/LinAlg/sparse.c \
	../../TuprykEngine/ui/prints/linalg.c \
	../../TuprykEngine/ui/graphics/window.c \
	../../TuprykEngine/ui/graphics/basic.c \
	../../TuprykEngine/Kinematics/configuration.c \
	../../TuprykEngine/Kinematics/frame.c \
	../../TuprykEngine/Graphics/raytracer.c \
	../../TuprykEngine/ui/prints/config.c \
	../../TuprykEngine/scenes/devastator/desert.c \
	../../TuprykEngine/Graphics/image.c \
	../../TuprykEngine/Algos/lists.c \
	../../TuprykEngine/Stochastic/sample.c \
	../../TuprykEngine/Geom/quaternions.c \
	../../TuprykEngine/Physics/simulation.c \
	../../TuprykEngine/Physics/forces.c \
	../../TuprykEngine/Algos/utils.c
OBJS = $(SRCS:.c=.o)
TARGET = p

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
