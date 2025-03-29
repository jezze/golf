struct camera
{

    float x;
    float y;
    float z;
    float angle;
    float vx;
    float vy;
    float vz;
    float vangle;
    float distance;
    float cosphi;
    float sinphi;

};

void camera_init(struct camera *camera, float x, float y, float z, float distance);
