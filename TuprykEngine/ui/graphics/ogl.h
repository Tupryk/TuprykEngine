#ifndef UI_OGL
#define UI_OGL

/* Create the window and set up GL state. Call once, first. */
void ogl_init(void);

/* Movement speed (world units per second, default 6.0) and mouse
 * sensitivity (degrees per pixel, default 0.15). */
void ogl_set_speed(float speed, float sens);

/* Camera position in world coordinates. Z is up. (default 0, -8, 1.5) */
void ogl_set_camera_pos(float x, float y, float z);

/* Camera orientation in degrees. Z is up. Pitch is clamped to +-89.
 * pitch 0 = level, positive looks up (+Z).
 * yaw 0 looks down +X, 90 looks down +Y, 180 down -X, -90 down -Y
 * (default yaw 90, pitch -10). */
void ogl_set_camera_angles(float pitch, float yaw);

/* Start the main loop (never returns).
 * `draw` is called every frame AFTER the screen is cleared and the camera
 * is applied, so it should only issue drawing calls (glutSolidSphere etc.).
 * Do not clear, set the camera, or swap buffers inside it. */
void ogl_loop(void (*draw)(void));

void ogl_draw_sphere(
float x, float y, float z, float radius,
float r, float g, float b
);

void ogl_draw_line(float x1, float y1, float z1,
float x2, float y2, float z2,
float r, float g, float b, float a);


#endif
