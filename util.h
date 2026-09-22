#ifndef UTIL_H
#define UTIL_H

#include "type.h"

#define PI 3.14159265358979323846

#define DEG_TO_RAD(x) ((x) * PI / 180.0)
#define RAD_TO_DEG(x) ((x) * 180.0 / PI)

void lla_to_ecef(
    double lat,
    double lon,
    double alt,
    double *x,
    double *y,
    double *z
);

void ecef_to_lla(
    double x,
    double y,
    double z,
    double *lat,
    double *lon,
    double *alt
);

void ned_velocity_to_ecef(
    double lat,
    double lon,
    double vn,
    double ve,
    double vd,
    double *vx,
    double *vy,
    double *vz
);

void update_target_euler(
    Target *target,
    double dt
);

void apply_command(
    Target *target,
    const Command *cmd
);

#endif