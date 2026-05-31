#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <string.h>

#define WIDTH 900
#define HEIGHT 600
#define MASS_RADIUS 20
#define LINE_THICKNESS 4
#define TRACE_LEN 100
#define TRACE_THICKNESS 2
#define L1 200
#define L2 150
#define G 1

Vector2 start_pos = (Vector2) { WIDTH / 2, 0};
Vector2 trace[TRACE_LEN] = {};
int current_trace_len = 0;
float l1, l2, phi1, phi2, phi1_d, phi2_d, phi1_dd, phi2_dd, m1, m2;


Vector2 get_end(Vector2 start, float l, float phi)
{
    return (Vector2) {start.x + l * sinf(phi), start.y + l * cosf(phi)};
}

void draw_pendulum(float l, Vector2 start, float phi)
{
    Vector2 end = get_end(start, l, phi);
    DrawLineEx(start, end, LINE_THICKNESS, WHITE);
    DrawCircleV(end, MASS_RADIUS, RED);
}

void draw_double_pendulum(Vector2 start, float phi1, float phi2, float l1, float l2)
{
    Vector2 end1 = get_end(start, l1, phi1);
    draw_pendulum(l2, end1, phi2);

    Vector2 end2 = get_end(end1, l2, phi2);
    draw_pendulum(l1, start, phi1);

    if (current_trace_len < TRACE_LEN) {
        current_trace_len++;
    }

    Vector2 trace_copy[TRACE_LEN];
    memcpy(trace_copy, trace, sizeof(Vector2)*TRACE_LEN);
    // shift trace back
    for (int i = 0; i < current_trace_len; i++) {
        trace[i] = trace_copy[i - 1];
    }
    trace[0] = end2;

    // visualize trace
    Color color_red = RED;
    for (int i = 0; i < TRACE_LEN; ++i) {
        DrawRectangleV(trace[i], (Vector2){TRACE_THICKNESS, TRACE_THICKNESS}, RED);
    }
}

void step(float dt)
{
    phi1_dd = (-G * (2 * m1 + m2) * sinf(phi1) - m2 * G * sinf(phi1 - 2 * phi2) - 2 * sinf(phi1 - phi2)
            * m2 * (phi2_d * phi2_d * l2 + phi1_d * phi1_d * l1 * cosf(phi1 - phi2)))
            / (l1 * (2 * m1 + m2 - m2 * cosf(2 * phi1 - 2 * phi2)));

    phi2_dd = 2*sinf(phi1-phi2)*(phi1_d*phi1_d*l1*(m1+m2) + G*(m1+m2)*cosf(phi1)+phi2_d*phi2_d*l1*m2*cosf(phi1-phi2))
        / (l2 * (2*m1+m2-m2*cosf(2*phi1-2*phi2)));

    phi1_dd += phi1_d * dt;
    phi2_dd += phi2_d * dt;

    phi1 += phi1_d * dt;
    phi2 += phi2_d * dt;
}

void init_solver()
{
    l1 = L1;
    l2 = L2;
    phi1 = GetRandomValue(-90, 90) * DEG2RAD;
    phi2 = GetRandomValue(-90, 90) * DEG2RAD;
    phi1_d = 0;
    phi2_d = 0;
    m1 = 1;
    m2 = 1;
}

int main(int argc, char *argv[])
{
    InitWindow(WIDTH, HEIGHT, "Double pendulum");

    SetTargetFPS(60);

    init_solver();

    while (!WindowShouldClose()) {
        step(GetFrameTime());
        BeginDrawing();
        ClearBackground(BLACK);
        draw_double_pendulum(start_pos, phi1, phi2, l1, l2);
        EndDrawing();
    }
    return 0;
}
