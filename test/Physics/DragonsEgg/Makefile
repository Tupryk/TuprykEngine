CC = gcc
UNAME_S := $(shell uname -s)

CFLAGS  = -Wall -Wextra -g `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm

ifeq ($(UNAME_S),Darwin)
    # macOS
    CFLAGS  += -Wno-deprecated
    LDFLAGS += -framework GLUT -framework OpenGL
else
    # Linux
    LDFLAGS += -lglut -lGLU -lGL
endif

SRCS = main.c \
    ../../../TuprykEngine/LinAlg/tensor.c \
    ../../../TuprykEngine/LinAlg/sparse.c \
    ../../../TuprykEngine/ui/prints/linalg.c \
    ../../../TuprykEngine/Physics/particle.c \
    ../../../TuprykEngine/ui/graphics/window.c \
    ../../../TuprykEngine/ui/graphics/particle_sim.c \
    ../../../TuprykEngine/Stochastic/sample.c \
    ../../../TuprykEngine/Algos/lists.c \
    ../../../TuprykEngine/Algos/sds.c \
    ../../../TuprykEngine/ui/graphics/basic.c \
    ../../../TuprykEngine/ui/graphics/ogl.c
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
