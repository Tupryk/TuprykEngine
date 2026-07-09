#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/Graphics/image.h"
#include "../../../TuprykEngine/Graphics/raytracer.h"
#include "../../../TuprykEngine/Kinematics/configuration.h"

#include "../../../TuprykEngine/scenes/loader.h"
#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/basic.h"
#include "../../../TuprykEngine/ui/prints/config.h"


// Returns the dt at collision and -1.f if no collision.
float collision_in_time(config* C, float time)
{
    return -1.f;
}

int main(void)
{
    config* C = load_scene("../../../TuprykEngine/scenes/files/two_balls.sn");
    print_config(C);
    
    // tensor* im = new_image(WINDOW_W, WINDOW_H, 3);
    // raytrace(C, -1, im);

    // collision_in_time(config* C, float time);

    config_free(C);

    return 0;
}
