#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct Volumetrics{
    double x_min;
    double y_min;
    double z_min;

    double x_max;
    double y_max;
    double z_max;

    double dx;
    double dy;
    double dz;

    Volumetrics(){
        x_min = 0.0;
        y_min = 0.0;
        z_min = 0.0;

        x_max = 0.0;
        y_max = 0.0;
        z_max = 0.0;

        dx = 0.0;
        dy = 0.0;
        dz = 0.0;
    }

    Volumetrics& operator= (const Volumetrics& vol)
    {
     if(this == &vol) return *this;

     this->x_min = vol.x_min;
     this->y_min = vol.y_min;
     this->z_min = vol.z_min;

     this->x_max = vol.x_max;
     this->y_max = vol.y_max;
     this->z_max = vol.z_max;

     this->dx = vol.dx;
     this->dy = vol.dy;
     this->dz = vol.dz;

     return *this;
    }

} Volumetrics;

#endif // STRUCTS_H
