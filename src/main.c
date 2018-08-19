#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

/* Draw debug information. */
// #define DEBUG_VISUAL (1)
/* Print debug information. */
// #define DEBUG_CONSOLE (2)
#define ENTER (13)
#define ESC (27)
#define GAME_TIMER_ID (1)
#define GAME_TIMER_INTERVAL (20)

typedef struct _Line {
    GLfloat y;
} Line;

typedef struct _Car {
    GLfloat x;
    GLfloat y;
} Car;

/* START of GLOBAL variables. */
GLfloat carLength;
GLfloat carWidth;
GLfloat carScaleX;

int carsNum;
Car* cars;
GLfloat carsSeparation;
GLfloat carsSpawnY;

GLfloat playerCarX;
GLfloat playerCarY;

GLfloat lineLength;
GLfloat lineWidth;
GLfloat lineScaleX;

int linesNum;
Line* lines;

GLfloat linesSeparation;
GLfloat linesLX;
GLfloat linesRX;

int gameAnimation;
GLfloat speed;
GLfloat score;
/* END of GLOBAL variables.  */

/* START of functions declarations. */

/* Error check function. */
void assert(int expr, char* msg);
/* Initialize global variables. */
void initGlobalVars(int argc, char** argv);
/* Free/Remove .. variables. */
void deInitVars(void);
/*Initialize OpenGl*/
void initOpenGL(void);
/* Initialize light. */
void initLight(void);

/* Callback functions. */
void onKeyboard(unsigned char key, int x, int y);
void onReshape(int width, int height);
void onDisplay(void);
/* Timer function. */
void onTimer(int timerId);

/* Drawing functions. */
void drawAxis(void);
void drawEdges(void);
void drawRoadLines(void);
void drawCars(void);
void drawRoad(void);
/* END of functions declarations. */

int main(int argc, char** argv)
{
    /* Initialize global variables. */
    initGlobalVars(argc, argv);

    /* Initialize GLUT. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    /* Initialize/Create window. */
    glutInitWindowSize(300, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    /* Initialize OpenGL. */ 
    initOpenGL();

    /* Callback functions. */
    glutKeyboardFunc(onKeyboard);
    glutReshapeFunc(onReshape);
    glutDisplayFunc(onDisplay);

    /* Main loop. */
    glutMainLoop();

    /* Deinitialize global variables. */
    deInitVars();

    exit(EXIT_SUCCESS);
}

void assert(int expr, char* msg)
{
    if (!expr) {
        perror(msg);
        fprintf(stderr, "File: %s\nFunc: %s\nLine: %d\n", __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }    
}


void initGlobalVars(int argc, char** argv)
{
    carsNum = 3;

    carScaleX = 0.70f;
    carLength = 0.30f;
    carWidth = carLength * carScaleX;

    cars = (Car*)malloc(carsNum * sizeof(Car));
    assert(NULL != cars, "malloc()");

    /* Create spawn points for bot cars. */
    carsSeparation = 0.00f;
    GLfloat carX = 0.00f;
    int carRandomX = 0;
    for (int i = 0; i < carsNum; i++) {

        carRandomX = rand() % 3;
        switch (carRandomX) {
            case 0:
                carX = -0.50f;
                break;
            case 1:
                carX = 0.00f;
                break;
            case 2:
                carX = 0.50f;
                break;
        }

        cars[i].x = carX;
        cars[i].y = 1.00f + carsSeparation;
        carsSeparation += 2.5 * carLength; 
    }

    carsSpawnY = carsSeparation - 1.00f;

    playerCarX = 0.00f;
    playerCarY = -0.70f;

    #ifdef DEBUG_CONSOLE
        printf("carLength = %.2f, carWidth = %.2f, carsNum = %d\n", carLength, carWidth, carsNum);
    #endif

    linesNum = 8;
    lineScaleX = 0.30f;
    lineLength = carLength / 2;
    lineWidth = lineLength * lineScaleX;
    linesLX = -0.25f;
    linesRX = 0.25f;

    #ifdef DEBUG_CONSOLE
        printf("lineLength = %.2f, lineWidth = %.2f, linesLX = %.2f, linesRX = %.2f\n", lineLength, lineWidth, linesLX, linesRX);
    #endif

    lines = (Line*)malloc(linesNum * sizeof(Line));
    assert(NULL != lines, "malloc()");

    /* Create spawn points for road lines. */
    linesSeparation = 0.00f;
    for (int i = 0; i < linesNum; i++) {
        lines[i].y = 1.00f - linesSeparation;
        linesSeparation += 2 * lineLength; 
    }

    gameAnimation = 0;
    speed = carLength / (GLfloat) GAME_TIMER_INTERVAL / 2;
    score = 0.00f;
}

void onKeyboard(unsigned char key, int x, int y)
{
    switch (key) {

        /* Close game. */
        case ESC:
            exit(EXIT_SUCCESS);
            break;

        /* Start game. */
        case ENTER:

            if (!gameAnimation) {

                glutTimerFunc(GAME_TIMER_INTERVAL, onTimer, GAME_TIMER_ID);
                gameAnimation = 1;
            }
            break;
        
        /* Pause game. */
        case 'p':
        case 'P':
            gameAnimation = 0;
            break;

        /* Move one track to the left. */
        case 'a':
        case 'A':

            if (playerCarX != -0.50f) {

                playerCarX -= 0.50f;
            }
            break;

        /* Move player one track to the right. */
        case 'd':
        case 'D':

            if (playerCarX != 0.50f) {

                playerCarX += 0.50f;
            }
            break;
    }
}

void onReshape(int width, int height)
{
    /* Set viewport. */
    glViewport(0, 0, width, height);

    /* Set projection. */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho
    (
        -0.75d, 0.75d,
        -1.00d, 1.00d,
        -2.00d, 2.00d
    );
}

void onDisplay(void)
{
    /* Clear buffers. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Set point of view. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt
    (
        0.00d, 0.00d, 1.00d,
        0.00d, 0.00d, 0.00d, 
        0.00d, 1.00d, 0.00d
    );

    /* Initialize light. */
    initLight();

    #ifdef DEBUG_VISUAL
        /* Draw main axis lines. */
        drawAxis();

        /* Draw edges of view window. */
        drawEdges();
    #endif

    #ifdef DEBUG_CONSOLE
        printf("Road lines Y coordinates = ");
        for (int i = 0; i < linesNum; i++) {
            printf("%.2lf ", lines[i].y);
        }
        putchar('\n');
    #endif

    /* Draw road. */
    drawRoad();

    /* Draw road lines. */
    drawRoadLines();

    /* Draw cars. */
    drawCars();

    /* Swap buffers. */
    glutSwapBuffers();
}

void onTimer(int timerId)
{
    if (timerId == GAME_TIMER_ID) {

        /* Animate road lines. */
        for (int i = 0; i < linesNum; i++) {
            lines[i].y -= speed;
        }

        /* Score is based on speed. */
        score += speed;
        printf("Score: %.2lf\n", score);

        /* Animate cars. */
        for (int i = 0; i < carsNum; i++) {
            cars[i]. y -= speed;
        }

        /* Force draw on display. */
        glutPostRedisplay();

        /* Lines collision with bottom edge. */
        for (int i = 0; i < linesNum; i++) {

            if ((lines[i].y + lineLength/2) <= -1.00f) {

                lines[i].y = 1.00f;
            }
        }

        /* Cars colision with bottom edge. */
        for (int i = 0; i < carsNum; i++) {

            if (cars[i].y + carLength/2 <= -1.00f) {
                
                int carRandomX = rand() % 3;
                GLfloat carX;

                switch (carRandomX) {
                    case 0:
                        carX = -0.50f;
                        break;
                    case 1:
                        carX = 0.00f;
                        break;
                    case 2:
                        carX = 0.50f;
                        break;
                }

                /* New spawn point for bot car. */
                cars[i].x = carX;
                cars[i].y = carsSpawnY;
            }
        }

        /* Cars collision with player car. */
        for (int i = 0; i < carsNum; i++) {

            /* Possible collision. */
            if (cars[i].y <= -0.30f) {

                /* Collision detect. */
                if (fabs((cars[i].y - playerCarY)) <= carLength) {

                        if (cars[i].x == playerCarX) {

                            printf("GAMEOVER: playerCarY = %.2f, colisionCarY = %.2f\n", playerCarY, cars[i].y);
                            gameAnimation = 0;
                        }
                }
            }
        }

        if (gameAnimation) {
            glutTimerFunc(GAME_TIMER_INTERVAL, onTimer, GAME_TIMER_ID);
        }
    }
}

void initOpenGL(void)
{
    /* Define clear color. */
    glClearColor(0.40f, 0.40f, 0.40f, 0.00f);

    /* Enable Z buffer. */
    glEnable(GL_DEPTH_TEST);
}

void initLight(void)
{
    /* Light properties. */
    GLfloat lightPosition[] = {1.00f, -0.99f, 0.60f, 1.00f};
    GLfloat lightAmbient[] = {0.20f, 0.20f, 0.20f, 1.00f};
    GLfloat lightDiffuse[] = {0.80f, 0.80f, 0.80f, 1.00f};
    GLfloat lightSpecular[] = {1.00f, 1.00f, 1.00f, 1.00f};

    /* Material properties. */
    GLfloat ambient_coeffs[] = {0.20f, 0.20f, 0.20f, 1.00f};
    GLfloat diffuse_coeffs[] = {0.80f, 0.80f, 0.80f, 1.00f};
    GLfloat specular_coeffs[] = {0.00f, 0.00f, 0.00f, 1.00f};
    GLfloat shininess = 20.00f;

    /* Enable coloring. */
    glEnable(GL_COLOR_MATERIAL);

    /* Smooth shading. */
    glShadeModel(GL_SMOOTH);

    /* Enable lighting. */
    glEnable(GL_LIGHTING);

    /* Enable first light. */
    glEnable(GL_LIGHT0);

    /* Apply light properties. */
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    /* Apply material properties. */
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void drawAxis(void)
{
    glPushMatrix();

        glBegin(GL_LINES);
            /* X axis. */
            glColor3f(1.00f, 0.00f, 0.00f);
            glVertex3f(0.00f, 0.00f, 0.00f);
            glVertex3f(1.00f, 0.00f, 0.00f);

            /* Y axis. */
            glColor3f(0.00f, 1.00f, 0.00f);
            glVertex3f(0.00f, 0.00f, 0.00f);
            glVertex3f(0.00f, 1.00f, 0.00f);

            /* Z axis. */
            glColor3f(0.00f, 0.00f, 1.00f);
            glVertex3f(0.00f, 0.00f, 0.00f);
            glVertex3f(0.00f, 0.00f, 1.00f);
        glEnd();

    glPopMatrix();
}

void drawEdges()
{
    glPushMatrix();

        glLineWidth(5);

        glColor3f(1.00f, 0.00f, 0.00f);
        glBegin(GL_LINES);
            /* Left edge. */
            glVertex3f(-0.75f, -1.00f, 0.00f);
            glVertex3f(-0.75f, 1.00f, 0.00f);

            /* Right edge. */
            glVertex3f(0.75f, -1.00f, 0.00f);
            glVertex3f(0.75f, 1.00f, 0.00f);

            /* Bottom edge. */
            glVertex3f(-0.75f, -1.00f, 0.00f);
            glVertex3f(0.75f, -1.00f, 0.00f);

            /* Top edge. */
            glVertex3f(-0.75f, 1.00f, 0.00f);
            glVertex3f(0.75f, 1.00f, 0.00f);

            /* Left road line. */
            glVertex3f(-0.25f, -1.00f, 0.00f);
            glVertex3f(-0.25f, 1.00f, 0.00f);

            /* Right road line. */
            glVertex3f(0.25f, -1.00f, 0.00f);
            glVertex3f(0.25f, 1.00f, 0.00f);

        glEnd();

        glLineWidth(1);

    glPopMatrix();
}

void drawRoad(void)
{
    glColor3f(0.40f, 0.40f, 0.40f);
    glNormal3f(0.00f, 0.00f, 1.00f);
    glPushMatrix();
        glTranslatef(0.00f, 0.00f, -0.10f);
        glScalef(1.00f, 1.00f, 0.00f);
        glutSolidCube(2.00d);
    glPopMatrix();
}

void drawRoadLines(void)
{
    glColor3f(1.00f, 1.00f, 1.00f);

    for (int i = 0; i < linesNum; i++) {

        /* Draw left lines. */
        glPushMatrix();
            glTranslatef(linesLX, lines[i].y, 0.00f);
            glScalef(lineScaleX, 1.00f, 0.00f);
            glutSolidCube(lineLength);
        glPopMatrix();

        /* Draw right lines. */
        glPushMatrix();
            glTranslatef(linesRX, lines[i].y, 0.00f);
            glScalef(lineScaleX, 1.00f, 0.00f);
            glutSolidCube(lineLength);
        glPopMatrix();
    }
}

void deInitVars(void)
{
    free(lines);
    free(cars);
}

void drawCars(void)
{

    /* Bot cars. */
    for (int i = 0; i < carsNum; i++) {

        glColor3f(1.00f, 0.00f, 0.00f); 
        glPushMatrix(); 
            glTranslatef(cars[i].x, cars[i].y, 0.00f);
            glScalef(carScaleX, 1.00f, 1.00f);
            glutSolidCube((GLdouble)carLength);
        glPopMatrix();
    }

    /* Player car. */
    glColor3f(0.00f, 0.00f, 1.00f); 
    glPushMatrix(); 
        glTranslatef(playerCarX, playerCarY, 0.00f);
        glScalef(carScaleX, 1.00f, 1.00f);
        glutSolidCube((GLdouble)carLength);
    glPopMatrix();
}