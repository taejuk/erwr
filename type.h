#ifndef TYPE_H
#define TYPE_H

#define MAX_COMMAND 30

typedef struct {
    double lat;
    double lon;
    double alt;

    double x_ecef;
    double y_ecef;
    double z_ecef;

    double yaw;
    double pitch;
    double roll;

    double speed;

    double yaw_rate;
    double pitch_rate;
    double roll_rate;

} TargetState;


typedef struct {
    /* 이 Command를 적용할 시각 [s] */
    double apply_time;

    /* 현재 값에 더할 변화량 */
    double delta_speed;

    double delta_yaw_rate;
    double delta_pitch_rate;
    double delta_roll_rate;

} Command;


typedef struct {
    int target_id;

    TargetState state;

    int command_count;

    /*
     * 다음에 적용해야 할 command index
     */
    int current_command;

    Command commands[MAX_COMMAND];

} Target;

#endif