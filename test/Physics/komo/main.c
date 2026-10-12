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


int main(void)
{
    config_t* C = load_scene("../../../TuprykEngine/scenes/files/two_balls.sn");
    print_config(C);
    
    // tensor_t* im = new_image(WINDOW_W, WINDOW_H, 3);
    // raytrace(C, -1, im);

    config_free(C);

    return 0;
}
