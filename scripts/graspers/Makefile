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
    ../../TuprykEngine/LinAlg/tensor.c \
    ../../TuprykEngine/LinAlg/sparse.c \
    ../../TuprykEngine/ui/prints/linalg.c \
    ../../TuprykEngine/Stochastic/sample.c \
    ../../TuprykEngine/Algos/lists.c \
    ../../TuprykEngine/ui/graphics/ogl.c \
	../../TuprykEngine/ui/prints/algos.c \
    ../../TuprykEngine/NEAT/population.c \
	../../TuprykEngine/NEAT/agent.c \
	../../TuprykEngine/Funcs/funcs.c \
    ../../TuprykEngine/Geom/quaternions.c \
	../../TuprykEngine/Physics/voxels.c \
	../../TuprykEngine/Physics/voxel_utils.c \
	../../TuprykEngine/Physics/voxel_generators.c \
    ../../TuprykEngine/ui/graphics/voxels_vis.c \
	../../TuprykEngine/ui/prints/vis_neat.c \
    ../../TuprykEngine/MCP/mcp.c \
    utils.c
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
