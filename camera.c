#include <stdlib.h>
#include "camera.h"

void camera_init(struct camera *camera, float x, float y, float z, float distance)
{

    camera->x = x;
    camera->y = y;
    camera->z = z;
    camera->distance = distance;

}

