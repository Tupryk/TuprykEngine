#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../TuprykEngine/Graphics/image.h"
#include "../../TuprykEngine/Graphics/raytracer.h"
#include "../../TuprykEngine/Kinematics/configuration.h"
#include "../../TuprykEngine/Physics/simulation.h"

#include "../../TuprykEngine/ui/graphics/window.h"
#include "../../TuprykEngine/ui/graphics/basic.h"
#include "../../TuprykEngine/ui/prints/config.h"


int main(void)
{
    srand( time( NULL ) );
    init_window();

    tensor_t* im = new_image(WINDOW_W, WINDOW_H, 3);

    view_image(im);
    window_wait();

    tensor_free(im);

    free_window();
    return 0;
}
