#ifndef CONFIG_FRAME
#define CONFIG_FRAME

#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"


typedef struct 
{
    int type;  // 0: X-Axis, 1: Y-Axis, 2: Z-Axis, 3: Free
    int has_limits;
    int q_id;
} joint_t;

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
    
    float mass;
    tensor* inertia;
    
    texture* tex;
    
    stack* forces;  // force_t
} geom;

typedef struct
{
    tensor* force;
    tensor* torque;
    tensor* poa;
} force_t;  // TODO: Should maybe be in a different filem ie. Physics/forces.h

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
    char* name;
    tensor* pos;
    tensor* rot;

    tensor* pos_rel;
    tensor* rot_rel;
    
    int parent;
    int children_count;
    int* children;

    int type;  // 0: Marker, 1: Geometry, 2: Camera, 3: Light, 4: Joint
    void* data;
} frame;

frame* frame_init(const char *name, float* pos, float* rot);
void frame_free(frame* f);
void frame_get_pose_matrix(frame* f, tensor* pose);
tensor* frame_get_pose_matrix_give(frame* f);

void force_free(force_t* f);
void forces_add(force_t* a, force_t* b, force_t* out);

#endif
