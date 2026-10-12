#include "ogl.h"

#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define DEG2RAD     0.01745329f

/* All state lives here (static in the .c file, NOT in the header), so there
 * is exactly one copy of the camera. */
/* Z is up. X is right and Y is forward at yaw = 90. */
static float cam_x = 0.0f, cam_y = -8.0f, cam_z = 1.5f;
static float yaw   = 90.0f;  /* degrees; 0 looks down +X, 90 looks down +Y */
static float pitch = -10.0f; /* degrees; positive looks up (+Z) */

static float move_speed = 6.0f;  /* world units per second */
static float mouse_sens = 0.15f; /* degrees per pixel */

static unsigned char keys[256] = {0};
static int last_time = 0;
static int last_x, last_y, dragging = 0;
static void (*user_draw)(void) = NULL;

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float fx = cosf(pitch * DEG2RAD) * cosf(yaw * DEG2RAD);
    float fy = cosf(pitch * DEG2RAD) * sinf(yaw * DEG2RAD);
    float fz = sinf(pitch * DEG2RAD);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam_x, cam_y, cam_z,
              cam_x + fx, cam_y + fy, cam_z + fz,
              0.0f, 0.0f, 1.0f); /* up = +Z */

    /* Set the light after the camera so it stays fixed in the world */
    GLfloat light_pos[] = {5.0f, 5.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    if (user_draw) user_draw();

    glutSwapBuffers();
}

static void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / h, 0.1, 300.0);
    glMatrixMode(GL_MODELVIEW);
}

static void key_down(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    if (key == 27) exit(0);
    keys[tolower(key)] = 1;
}

static void key_up(unsigned char key, int x, int y)
{
    (void)x; (void)y;
    keys[tolower(key)] = 0;
}

static void idle(void)
{
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - last_time) / 1000.0f;
    last_time = now;
    if (dt > 0.1f) dt = 0.1f;

    /* Ground plane is XY. Forward and right are flat (ignore pitch). */
    float fx = cosf(yaw * DEG2RAD), fy = sinf(yaw * DEG2RAD);
    float rx = fy, ry = -fx;

    float mx = 0.0f, my = 0.0f, mz = 0.0f;
    if (keys['w']) { mx += fx; my += fy; }
    if (keys['s']) { mx -= fx; my -= fy; }
    if (keys['d']) { mx += rx; my += ry; }
    if (keys['a']) { mx -= rx; my -= ry; }
    if (keys['e']) mz += 1.0f;
    if (keys['q']) mz -= 1.0f;

    float len = sqrtf(mx * mx + my * my);
    if (len > 0.0f) { mx /= len; my /= len; }

    cam_x += mx * move_speed * dt;
    cam_y += my * move_speed * dt;
    cam_z += mz * move_speed * dt;

    glutPostRedisplay();
}

/* Hold the left mouse button and drag to look around */
static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        dragging = (state == GLUT_DOWN);
        last_x = x;
        last_y = y;
    }
}

static void motion(int x, int y)
{
    if (!dragging) return;

    yaw   -= (x - last_x) * mouse_sens; /* yaw grows counter-clockwise seen from above */
    pitch -= (y - last_y) * mouse_sens;
    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    last_x = x;
    last_y = y;
}

void ogl_set_speed(float speed, float sens)
{
    move_speed = speed;
    mouse_sens = sens;
}

void ogl_set_camera_pos(float x, float y, float z)
{
    cam_x = x;
    cam_y = y;
    cam_z = z;
}

void ogl_set_camera_angles(float pitch_deg, float yaw_deg)
{
    if (pitch_deg >  89.0f) pitch_deg =  89.0f;
    if (pitch_deg < -89.0f) pitch_deg = -89.0f;
    pitch = pitch_deg;
    yaw = yaw_deg;
}

void ogl_init(void)
{
    int argc = 1;
    char *argv[] = {"engine", NULL};

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Engine");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    /* Ambient light lights every surface equally, regardless of direction.
     * The OpenGL default is 0.2; raise these values for a brighter scene. */
    GLfloat ambient[] = {0.75f, 0.75f, 0.75f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
}

void ogl_loop(void (*draw)(void))
{
    user_draw = draw;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(key_down);
    glutKeyboardUpFunc(key_up);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(idle);

    last_time = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
}

void ogl_draw_sphere(
    float x, float y, float z, float radius,
    float r, float g, float b
) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(r, g, b);
    glutSolidSphere(radius, 32, 32);
    glPopMatrix();
}

void ogl_draw_line(float x1, float y1, float z1,
                   float x2, float y2, float z2,
                   float r, float g, float b, float a)
{
    glDisable(GL_LIGHTING); /* lines have no normals, so draw them unlit */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
    glColor4f(r, g, b, a);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
 
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}
