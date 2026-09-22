#include "util.h"

#include <math.h>
#include <stddef.h>
#define EARTH_RADIUS 6400000.0

void lla_to_ecef(
    double lat,
    double lon,
    double alt,
    double *x,
    double *y,
    double *z)
{
    double r;
    double cos_lat;
    double sin_lat;
    double cos_lon;
    double sin_lon;

    if (x == NULL || y == NULL || z == NULL) {
        return;
    }

    /*
     * lat, lon : radian
     * alt      : meter
     */

    r = EARTH_RADIUS + alt;

    cos_lat = cos(lat);
    sin_lat = sin(lat);

    cos_lon = cos(lon);
    sin_lon = sin(lon);

    *x = r * cos_lat * cos_lon;
    *y = r * cos_lat * sin_lon;
    *z = r * sin_lat;
}

void ecef_to_lla(
    double x,
    double y,
    double z,
    double *lat,
    double *lon,
    double *alt)
{
    double r;

    if (lat == nullptr || lon == nullptr || alt == nullptr) {
        return;
    }

    r = sqrt(x * x + y * y + z * z);

    *lat = asin(z / r);
    *lon = atan2(y, x);
    *alt = r - EARTH_RADIUS;
}

void apply_command(Target *target, const Command *cmd)
{
    if (target == nullptr || cmd == nullptr) {
        return;
    }

    target->state.speed += cmd->delta_speed;

    target->state.yaw_rate += cmd->delta_yaw_rate;
    target->state.pitch_rate += cmd->delta_pitch_rate;
    target->state.roll_rate += cmd->delta_roll_rate;
}


void ned_velocity_to_ecef(
    double lat,
    double lon,
    double vn,
    double ve,
    double vd,
    double *vx,
    double *vy,
    double *vz)
{
    double sin_lat;
    double cos_lat;
    double sin_lon;
    double cos_lon;

    if (vx == nullptr || vy == nullptr || vz == nullptr) {
        return;
    }

    sin_lat = sin(lat);
    cos_lat = cos(lat);

    sin_lon = sin(lon);
    cos_lon = cos(lon);

    /*
     * NED -> ECEF
     *
     * [vx]
     * [vy] = R_ned_to_ecef * [vn ve vd]^T
     * [vz]
     */

    *vx =
        -sin_lat * cos_lon * vn
        -sin_lon * ve
        -cos_lat * cos_lon * vd;

    *vy =
        -sin_lat * sin_lon * vn
        +cos_lon * ve
        -cos_lat * sin_lon * vd;

    *vz =
         cos_lat * vn
        -sin_lat * vd;
}


void update_target_euler(Target *target, double dt)
{
    TargetState *state;

    double vn;
    double ve;
    double vd;

    double vx_ecef;
    double vy_ecef;
    double vz_ecef;

    if (target == nullptr) {
        return;
    }

    state = &target->state;

    vn =
        state->speed *
        cos(state->pitch) *
        cos(state->yaw);

    ve =
        state->speed *
        cos(state->pitch) *
        sin(state->yaw);

    vd =
        -state->speed *
        sin(state->pitch);


    ned_velocity_to_ecef(
        state->lat,
        state->lon,
        vn,
        ve,
        vd,
        &vx_ecef,
        &vy_ecef,
        &vz_ecef
    );


    state->x_ecef += vx_ecef * dt;
    state->y_ecef += vy_ecef * dt;
    state->z_ecef += vz_ecef * dt;


    state->yaw += state->yaw_rate * dt;
    state->pitch += state->pitch_rate * dt;
    state->roll += state->roll_rate * dt;


    
    ecef_to_lla(
        state->x_ecef,
        state->y_ecef,
        state->z_ecef,
        &state->lat,
        &state->lon,
        &state->alt
    );
}