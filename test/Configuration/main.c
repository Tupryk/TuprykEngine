#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/LinAlg/gtensor.h"
#include "../../TuprykEngine/Kinematics/frame.h"
#include "../../TuprykEngine/Kinematics/configuration.h"
#include "../../TuprykEngine/Kinematics/features.h"
#include "../../TuprykEngine/scenes/devastator/desert.h"
#include "../../TuprykEngine/ui/prints/linalg.h"
#include "../../TuprykEngine/ui/prints/config.h"


int test_frame()
{
    frame_t* f = (frame_t*) malloc(sizeof(frame_t));

    int pos_shape[] = {3, 1};
    float pos_values[] = {1, 2, 3};
    f->pos = new_tensor(pos_shape, 2, pos_values);

    int rot_shape[] = {4, 1};
    float rot_values[] = {1, 0, 0, 0};
    f->rot = new_tensor(rot_shape, 2, rot_values);

    tensor_t* pose = frame_get_pose_matrix_give(f);
    print_tensor_verbose(pose, 10);

    tensor_free(f->pos);
    tensor_free(f->rot);
    tensor_free(pose);

    return 0;
}

int test_config()
{
    config_t* C = init_devastator_config();
    print_config(C);
    config_free(C);
    return 0;
}


int test_feature()
{
    config_t* C = init_devastator_config();

    float rel_pos_v[] = {1.f, 0.f, 0.f};
    tensor_t* rel_pos = new_tensor_vector(3, rel_pos_v);

    float target_v[] = {1.f, 0.f, 0.f};
    tensor_t* target = new_tensor_vector(3, target_v);

    gtensor_t* out = new_gtensor_vector(3, NULL);

    p2p_vec(C, 6, rel_pos, target, out);

    print_tensor(out->t);

    tensor_free(rel_pos);
    tensor_free(target);
    gtensor_free(out);
    config_free(C);
    return 0;
}

int main()
{
    // test_frame();
    // test_config();
    test_feature();
    return 0;
}
