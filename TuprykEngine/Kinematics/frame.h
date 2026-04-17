#ifndef CONFIG_FRAME
#define CONFIG_FRAME

#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"


typedef struct
{
    float color[3];
    float ambient;
    float diffuse;
    float specular;
    float shininess;
} texture;

typedef struct
{
    int type;  // 0: Sphere
    void* mesh;
    texture* tex;
} geom;

typedef struct
{
    float mass;
    float radius;
} ball_geom;

typedef struct
{
    float fx;
    float fy;
} camera_t;

typedef struct
{
    float intensity;
} light_t;

typedef struct
{
    tensor* pos;
    tensor* rot;
    
    int parent;
    int children_count;
    int* children;

    int type;  // 0: Marker, 1: Ball, 2: Camera, 3: Light
    void* data;

    stack* forces;
} frame;

frame* frame_init(float* pos, float* rot);
void frame_free(frame* f);
void frame_get_pose_matrix(frame* f, tensor* pose);
tensor* frame_get_pose_matrix_give(frame* f);

#endif
