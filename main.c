#include "type.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_TARGET 10
#define DT 0.1

int main(void)
{
    Target targets[MAX_TARGET];
    FILE *files[MAX_TARGET] = { NULL };

    int target_count = 0;
    double simulation_time = 0.0;


    /*
     * Target 개수 입력
     */
    printf("Target count: ");

    if (scanf("%d", &target_count) != 1) {
        fprintf(stderr, "failed to read target_count\n");
        return -1;
    }

    if (target_count > MAX_TARGET || target_count <= 0) {
        fprintf(stderr, "target_count invalid\n");
        return -1;
    }


    /*
     * Target별 초기 상태 + Command 입력
     */
    for (int i = 0; i < target_count; i++)
    {
        double lat;
        double lon;
        double alt;

        double yaw;
        double pitch;
        double roll;

        double speed;

        double yaw_rate;
        double pitch_rate;
        double roll_rate;


        printf("\n");
        printf("===== Target %d =====\n", i);


        /*
         * 초기 위치
         */
        printf("lat [deg]: ");
        scanf("%lf", &lat);

        printf("lon [deg]: ");
        scanf("%lf", &lon);

        printf("alt [m]: ");
        scanf("%lf", &alt);


        /*
         * 초기 자세
         */
        printf("yaw [deg]: ");
        scanf("%lf", &yaw);

        printf("pitch [deg]: ");
        scanf("%lf", &pitch);

        printf("roll [deg]: ");
        scanf("%lf", &roll);


        /*
         * 초기 속력
         */
        printf("speed [m/s]: ");
        scanf("%lf", &speed);


        /*
         * 초기 각속도
         */
        printf("yaw_rate [deg/s]: ");
        scanf("%lf", &yaw_rate);

        printf("pitch_rate [deg/s]: ");
        scanf("%lf", &pitch_rate);

        printf("roll_rate [deg/s]: ");
        scanf("%lf", &roll_rate);


        /*
         * Target 초기화
         */
        targets[i].target_id = i;

        targets[i].current_command = 0;


        targets[i].state.lat = DEG_TO_RAD(lat);
        targets[i].state.lon = DEG_TO_RAD(lon);
        targets[i].state.alt = alt;

        targets[i].state.yaw = DEG_TO_RAD(yaw);
        targets[i].state.pitch = DEG_TO_RAD(pitch);
        targets[i].state.roll = DEG_TO_RAD(roll);

        targets[i].state.speed = speed;

        targets[i].state.yaw_rate =
            DEG_TO_RAD(yaw_rate);

        targets[i].state.pitch_rate =
            DEG_TO_RAD(pitch_rate);

        targets[i].state.roll_rate =
            DEG_TO_RAD(roll_rate);


        /*
         * 초기 LLA -> ECEF
         */
        lla_to_ecef(
            targets[i].state.lat,
            targets[i].state.lon,
            targets[i].state.alt,

            &targets[i].state.x_ecef,
            &targets[i].state.y_ecef,
            &targets[i].state.z_ecef
        );


        /*
         * Command 개수 입력
         */
        printf("command count: ");

        scanf(
            "%d",
            &targets[i].command_count
        );


        if (targets[i].command_count < 0 ||
            targets[i].command_count > MAX_COMMAND) {

            fprintf(
                stderr,
                "invalid command_count\n"
            );

            return -1;
        }


        /*
         * Command 입력
         */
        for (int j = 0;
             j < targets[i].command_count;
             j++)
        {
            double apply_time;

            double delta_speed;

            double delta_yaw_rate;
            double delta_pitch_rate;
            double delta_roll_rate;


            printf(
                "\n--- Target %d Command %d ---\n",
                i,
                j
            );


            printf("apply time [s]: ");
            scanf("%lf", &apply_time);


            printf("delta speed [m/s]: ");
            scanf("%lf", &delta_speed);


            printf("delta yaw rate [deg/s]: ");
            scanf("%lf", &delta_yaw_rate);


            printf("delta pitch rate [deg/s]: ");
            scanf("%lf", &delta_pitch_rate);


            printf("delta roll rate [deg/s]: ");
            scanf("%lf", &delta_roll_rate);


            targets[i].commands[j].apply_time =
                apply_time;

            targets[i].commands[j].delta_speed =
                delta_speed;


            /*
             * 각속도 변화량도 내부에서는 rad/s
             */
            targets[i].commands[j].delta_yaw_rate =
                DEG_TO_RAD(delta_yaw_rate);

            targets[i].commands[j].delta_pitch_rate =
                DEG_TO_RAD(delta_pitch_rate);

            targets[i].commands[j].delta_roll_rate =
                DEG_TO_RAD(delta_roll_rate);
        }


        /*
         * 출력 파일 생성
         */
        char filename[64];

        snprintf(
            filename,
            sizeof(filename),
            "target_%d.csv",
            targets[i].target_id
        );


        files[i] = fopen(
            filename,
            "w"
        );


        if (files[i] == NULL) {

            fprintf(
                stderr,
                "failed to open %s\n",
                filename
            );

            for (int j = 0; j < i; j++) {
                if (files[j] != NULL) {
                    fclose(files[j]);
                }
            }

            return -1;
        }


        /*
         * CSV Header
         */
        fprintf(
            files[i],
            "time,latitude,longitude,altitude,"
            "speed,yaw,pitch,roll,"
            "yaw_rate,pitch_rate,roll_rate\n"
        );


        /*
         * 초기 상태 기록
         */
        fprintf(
            files[i],

            "%.1f,"
            "%.10f,%.10f,%.3f,"
            "%.3f,"
            "%.6f,%.6f,%.6f,"
            "%.6f,%.6f,%.6f\n",

            0.0,

            RAD_TO_DEG(targets[i].state.lat),
            RAD_TO_DEG(targets[i].state.lon),
            targets[i].state.alt,

            targets[i].state.speed,

            RAD_TO_DEG(targets[i].state.yaw),
            RAD_TO_DEG(targets[i].state.pitch),
            RAD_TO_DEG(targets[i].state.roll),

            RAD_TO_DEG(targets[i].state.yaw_rate),
            RAD_TO_DEG(targets[i].state.pitch_rate),
            RAD_TO_DEG(targets[i].state.roll_rate)
        );
    }


    /*
     * 총 Simulation 시간
     */
    printf("\nSimulation time [s]: ");

    if (scanf("%lf", &simulation_time) != 1) {

        fprintf(
            stderr,
            "failed to read simulation time\n"
        );

        for (int i = 0; i < target_count; i++) {
            fclose(files[i]);
        }

        return -1;
    }


    if (simulation_time <= 0.0) {

        fprintf(
            stderr,
            "simulation_time invalid\n"
        );

        for (int i = 0; i < target_count; i++) {
            fclose(files[i]);
        }

        return -1;
    }


    /*
     * Simulation
     */
    double time = 0.0;


    while (time + DT <= simulation_time + 1e-9)
    {
        /*
         * 이번 step의 시작 시각
         *
         * time = 0.0, 0.1, 0.2 ...
         */

        for (int i = 0; i < target_count; i++)
        {
            Target *target = &targets[i];


            /*
             * 현재 시각에 적용해야 하는 Command가 있는지 확인
             */
            while (
                target->current_command <
                target->command_count
            )
            {
                Command *cmd =
                    &target->commands[
                        target->current_command
                    ];


                /*
                 * Command의 apply_time에 도달했다면 적용
                 */
                if (cmd->apply_time <= time + 1e-9)
                {
                    apply_command(
                        target,
                        cmd
                    );


                    printf(
                        "[%.1f sec] "
                        "Target %d: Command %d applied\n",
                        time,
                        target->target_id,
                        target->current_command
                    );


                    target->current_command++;
                }
                else
                {
                    /*
                     * 아직 다음 command 시간이 안 됨
                     */
                    break;
                }
            }


            /*
             * 현재 상태로 DT = 0.1초 이동
             */
            update_target_euler(
                target,
                DT
            );
        }


        /*
         * 이제 t + 0.1 상태
         */
        time += DT;


        /*
         * 새 상태 CSV 기록
         */
        for (int i = 0; i < target_count; i++)
        {
            TargetState *state =
                &targets[i].state;


            fprintf(
                files[i],

                "%.1f,"
                "%.10f,%.10f,%.3f,"
                "%.3f,"
                "%.6f,%.6f,%.6f,"
                "%.6f,%.6f,%.6f\n",

                time,

                RAD_TO_DEG(state->lat),
                RAD_TO_DEG(state->lon),
                state->alt,

                state->speed,

                RAD_TO_DEG(state->yaw),
                RAD_TO_DEG(state->pitch),
                RAD_TO_DEG(state->roll),

                RAD_TO_DEG(state->yaw_rate),
                RAD_TO_DEG(state->pitch_rate),
                RAD_TO_DEG(state->roll_rate)
            );
        }
    }


    /*
     * 파일 닫기
     */
    for (int i = 0; i < target_count; i++) {
        fclose(files[i]);
    }


    printf("\nSimulation finished.\n");

    return 0;
}