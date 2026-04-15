#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include "../../TuprykEngine/Kinematics/frame.h"
#include "../../TuprykEngine/Kinematics/configuration.h"
#include "../../TuprykEngine/scenes/devastator/desert.h"
#include "../../TuprykEngine/visual/prints/linalg.h"
#include "../../TuprykEngine/visual/prints/config.h"


int test_frame()
{
    frame* f = (frame*) malloc(sizeof(frame));

    int pos_shape[] = {3, 1};
    float pos_values[] = {1, 2, 3};
    f->pos = new_tensor(pos_shape, 2, pos_values);

    int rot_shape[] = {4, 1};
    float rot_values[] = {1, 0, 0, 0};
    f->rot = new_tensor(rot_shape, 2, rot_values);

    tensor* pose = frame_get_pose_matrix_give(f);
    print_tensor_verbose(pose, 10);

    tensor_free(f->pos);
    tensor_free(f->rot);
    tensor_free(pose);

    return 0;
}

int test_config()
{
    config* C = init_devastator_config();
    print_config(C);
    config_free(C);
    return 0;
}

int main()
{
    // test_frame();
    test_config();
    return 0;
}
