#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>

struct point {
    GLfloat x, y, z;
};

/* Initialize OpenGL Graphics */
void initGL() {
    // Set "clearing" or background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black and opaque
    glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
}

// Global variables
struct point pos;   // position of the eye
struct point l;     // look/forward direction
struct point r;     // right direction
struct point u;     // up direction

float triangleVertex, triangleMax, triangleMin, sphereMax, sphereMin, triangleCur, sphereCur;
float angle_rotation = 0.0;

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,0,0);
        glVertex3f(0,1,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,0);
        glVertex3f(0,0,1);
    glEnd();
}

void drawCube()
{
    struct point points[101][101];
    for (int j = 0; j <= 100; j++) {
        for (int i = 0; i <= 100; i++) {
            points[j][i].x = -1.0+i*0.02;
            points[j][i].y = -1.0+j*0.02;
            points[j][i].z = 1.0;

            float len = points[j][i].x*points[j][i].x+points[j][i].y*points[j][i].y+points[j][i].z*points[j][i].z;
            len = sqrt(len);

            points[j][i].x /= len;
            points[j][i].y /= len;
            points[j][i].z /= len;

            points[j][i].x *= sphereCur;
            points[j][i].y *= sphereCur;
            points[j][i].z *= sphereCur;

        }
    }

    glBegin(GL_QUADS);
        for (int j = 0; j < 100; j++) {
            for (int i = 0; i < 100; i++) {
                glVertex3f(points[j][i].x, points[j][i].y, points[j][i].z);
                glVertex3f(points[j][i+1].x, points[j][i+1].y, points[j][i+1].z);

                glVertex3f(points[j+1][i+1].x, points[j+1][i+1].y, points[j+1][i+1].z);
                glVertex3f(points[j+1][i].x, points[j+1][i].y, points[j+1][i].z);
            }
        }
    glEnd();
}

void drawCubeSphere()
{
    // Front
    glPushMatrix();           // Begin drawing the pyramid with 4 triangles    
        glColor3f(0.0, 0.0, 1.0);
        glTranslatef(0.0, 0.0, triangleCur);
        drawCube();
    glPopMatrix();
    // Right
    glPushMatrix();
        glColor3f(1.0, 1.0, 0.0);
        glRotatef(90, 0, 1, 0);
        glTranslatef(0.0, 0.0, triangleCur);
        drawCube();
    glPopMatrix();

    // Back
    glPushMatrix();
        glColor3f(1.0, 0.0, 1.0);
        glRotatef(180, 0, 1, 0);
        glTranslatef(0.0, 0.0, triangleCur);        
        drawCube();
    glPopMatrix();

    // Left
    glPushMatrix();
        glColor3f(0.0, 1.0, 1.0);
        glRotatef(-90, 0, 1, 0);
        glTranslatef(0.0, 0.0, triangleCur);        
        drawCube();
    glPopMatrix();

    // lower front
    glPushMatrix();
        glColor3f(1.0, 0.0, 0.0);
        glRotatef(-90, 1, 0, 0);
        glTranslatef(0.0, 0.0, triangleCur);        
        drawCube();
    glPopMatrix();	

    // lower Right
    glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        glRotatef(90, 1, 0, 0);
        glTranslatef(0.0, 0.0, triangleCur);        
        drawCube();
    glPopMatrix();

}

void drawTriangle()
{
    glBegin(GL_TRIANGLES);
        glVertex3f( 0.0f, 1.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
    glEnd();
}

/* Draw an octahedron centered at the origin */
void drawOctahedron() {
    // Front
    glPushMatrix();           // Begin drawing the octahedron with 8 triangles
        glColor3f(1.0f, 0.0f, 0.0f);     // Red
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    // Right
    glPushMatrix();
        glColor3f(0.0f, 1.0f, 0.0f);     // Green
        glRotatef(90, 0, 0, 1);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    // Back
    glPushMatrix();
        glColor3f(0.0f, 0.0f, 1.0f);     // Blue
        glRotatef(180, 0, 0, 1);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    // Left
    glPushMatrix();
        glColor3f(0.0f, 1.0f, 1.0f);
        glRotatef(-90, 0, 0, 1);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    // lower front
    glPushMatrix();
        glColor3f(1.0f, 0.0f, 1.0f);
        glRotatef(180, 1, 1, 0);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();	

    // lower Right
    glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f);
        glRotatef(90, 0, 0, 1);
        glRotatef(180, 1, 1, 0);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    //lower back
    glPushMatrix();
        glColor3f(0.5f, 0.5f, 0.5f);    
        glRotatef(180, 0, 0, 1);
        glRotatef(180, 1, 1, 0);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();

    //lower left
    glPushMatrix();
        glColor3f(0.5f, 0.0f, 1.0f);
        glRotatef(-90, 0, 0, 1);
        glRotatef(180, 1, 1, 0);
        glTranslatef((1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0, (1.0-triangleCur)/3.0);
        glScaled(triangleCur, triangleCur, triangleCur);
        drawTriangle();
    glPopMatrix();
}

void drawCylinder(double height, double radius, int segments) {
    struct point points[segments+1];
    double phi = 1.231; //70.5287794 degrees to radians
    for (int i = 0; i < segments+1; i++) {
        double theta = i * 1.231 / segments -phi/2;
        points[i].x = radius * cos(theta);
        points[i].y = radius * sin(theta);
    }
    
    glBegin(GL_QUADS);
        for (int i = 0; i < segments; i++) {
            glVertex3f(points[i].x, points[i].y, height/2);
            glVertex3f(points[i].x, points[i].y, -height/2);
            glVertex3f(points[i+1].x, points[i+1].y, -height/2);
            glVertex3f(points[i+1].x, points[i+1].y, height/2);
        }
    glEnd();
}

void drawInternalCyl()
{
    //upper front
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //upper left
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(90, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //upper back
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //upper right
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(-90, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //middle front
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(90, 1, 0, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //middle left
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(-90, 1, 0, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //middle back
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(-90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //middle right
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(90, 1, 0, 0);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //lower front
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(180, 1, 0, 1);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //lower left
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(180, 1, 0, 1);
        glRotatef(90, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //lower back
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(180, 1, 0, 1);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

    //lower right
    glPushMatrix();
        glColor3f(1.0, 1.0, 1.0);
        glRotatef(180, 1, 0, 1);
        glRotatef(-90, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glRotatef(45, 0, 1, 0);
        glTranslatef(triangleCur/sqrt(2.0), 0.0, 0.0);
        drawCylinder(triangleCur*sqrt(2), sphereCur, 100);
    glPopMatrix();

}

/*  Handler for window-repaint event. Call back when the window first appears and
    whenever the window needs to be re-painted. */
void display() {
    // glClear(GL_COLOR_BUFFER_BIT);            // Clear the color buffer (background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);             // To operate on Model-View matrix
    glLoadIdentity();                       // Reset the model-view matrix

    // default arguments of gluLookAt
    // gluLookAt(0,0,0, 0,0,-100, 0,1,0);

    // control viewing (or camera)
    gluLookAt(pos.x,pos.y,pos.z,
              pos.x+l.x,pos.y+l.y,pos.z+l.z,
              u.x,u.y,u.z);
    // draw
    glRotatef(angle_rotation, 0, 1, 0);
    drawAxes();
    drawOctahedron();
    drawCubeSphere();
    drawInternalCyl();

    glutSwapBuffers();  // Render now
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshapeListener(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping area to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    /*if (width >= height) {
        // aspect >= 1, set the height from -1 to 1, with larger width
        gluOrtho2D(-1.0 * aspect, 1.0 * aspect, -1.0, 1.0);
    } else {
        // aspect < 1, set the width to -1 to 1, with larger height
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }*/
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/* Callback handler for normal-key event */
void keyboardListener(unsigned char key, int x, int y) {
    float rate = 0.01;
    double v = 0.25;
    double m = 0.1;
    double s;
    switch (key) {
    // Control eye (location of the eye)
    // control eyex
        case '1':
            r.x = r.x*cos(rate)+l.x*sin(rate);
            r.y = r.y*cos(rate)+l.y*sin(rate);
            r.z = r.z*cos(rate)+l.z*sin(rate);

            l.x = l.x*cos(rate)-r.x*sin(rate);
            l.y = l.y*cos(rate)-r.y*sin(rate);
            l.z = l.z*cos(rate)-r.z*sin(rate);
            break;

        case '2':
            r.x = r.x*cos(-rate)+l.x*sin(-rate);
            r.y = r.y*cos(-rate)+l.y*sin(-rate);
            r.z = r.z*cos(-rate)+l.z*sin(-rate);

            l.x = l.x*cos(-rate)-r.x*sin(-rate);
            l.y = l.y*cos(-rate)-r.y*sin(-rate);
            l.z = l.z*cos(-rate)-r.z*sin(-rate);
            break;

        case '3':
            l.x = l.x*cos(rate)+u.x*sin(rate);
            l.y = l.y*cos(rate)+u.y*sin(rate);
            l.z = l.z*cos(rate)+u.z*sin(rate);

            u.x = u.x*cos(rate)-l.x*sin(rate);
            u.y = u.y*cos(rate)-l.y*sin(rate);
            u.z = u.z*cos(rate)-l.z*sin(rate);
            break;

        case '4':
            l.x = l.x*cos(-rate)+u.x*sin(-rate);
            l.y = l.y*cos(-rate)+u.y*sin(-rate);
            l.z = l.z*cos(-rate)+u.z*sin(-rate);

            u.x = u.x*cos(-rate)-l.x*sin(-rate);
            u.y = u.y*cos(-rate)-l.y*sin(-rate);
            u.z = u.z*cos(-rate)-l.z*sin(-rate);
            break;

        case '5':
            u.x = u.x*cos(rate)+r.x*sin(rate);
            u.y = u.y*cos(rate)+r.y*sin(rate);
            u.z = u.z*cos(rate)+r.z*sin(rate);

            r.x = r.x*cos(rate)-u.x*sin(rate);
            r.y = r.y*cos(rate)-u.y*sin(rate);
            r.z = r.z*cos(rate)-u.z*sin(rate);
            break;

        case '6':
            u.x = u.x*cos(-rate)+r.x*sin(-rate);
            u.y = u.y*cos(-rate)+r.y*sin(-rate);
            u.z = u.z*cos(-rate)+r.z*sin(-rate);

            r.x = r.x*cos(-rate)-u.x*sin(-rate);
            r.y = r.y*cos(-rate)-u.y*sin(-rate);
            r.z = r.z*cos(-rate)-u.z*sin(-rate);
            break;

        case 'a':
            angle_rotation += 1.0f;
            break;
        case 'd':
            angle_rotation -= 1.0f;
            break;
        case 'w':
            pos.z += m;
            break;
        case 's':
            pos.z -= m;
            break;

        case ',':
            if(triangleCur >= 0.0625) triangleCur-=triangleMax/16.0;
            if(sphereCur <= sphereMax-0.036084)sphereCur +=sphereMax/(16.0);
            break;
        case '.':
            if(triangleCur <= triangleMax-0.0625) triangleCur+=triangleMax/16.0;
            if(sphereCur >= 0.036084)sphereCur -= sphereMax/16.0;
            break;
        // Control what is shown
        // Control exit
        case 27:    // ESC key
            exit(0);    // Exit window
            break;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

/* Callback handler for special-key event */
void specialKeyListener(int key, int x,int y) {
    switch (key) {
    case GLUT_KEY_RIGHT:
        pos.x+=r.x;
        pos.y+=r.y;
        pos.z+=r.z;
        break;
    case GLUT_KEY_LEFT :
        pos.x-=r.x;
        pos.y-=r.y;
        pos.z-=r.z;
        break;

    case GLUT_KEY_PAGE_UP:
        pos.x+=u.x;
        pos.y+=u.y;
        pos.z+=u.z;
        break;
    case GLUT_KEY_PAGE_DOWN:
        pos.x-=u.x;
        pos.y-=u.y;
        pos.z-=u.z;
        break;
    case GLUT_KEY_UP:
        pos.x+=l.x;
		pos.y+=l.y;
		pos.z+=l.z;
		break;
    case GLUT_KEY_DOWN:
        pos.x-=l.x;
        pos.y-=l.y;
        pos.z-=l.z;
        break;
    
    default:
        break;
    }
    glutPostRedisplay();    // Post a paint request to activate display()
}

void init()
{
    pos.x=0;pos.y=0;pos.z=5;

    l.x=0;l.y=0;l.z=-1;
    u.x=0;u.y=1;u.z=0;
    r.x=1;r.y=0;r.z=0;

    triangleMin = 0.0;
    triangleMax = 1.0;
    triangleCur = 1.0;
    sphereCur = 0.0;
    sphereMax = 1.0/sqrt(3);
    sphereMin = 0.0;
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
    glutInit(&argc, argv);                      // Initialize GLUT
    glutInitWindowSize(640, 640);               // Set the window's initial width & height
    glutInitWindowPosition(50, 50);             // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color
    glutCreateWindow("OpenGL 3D Drawing");      // Create a window with the given title
    init();
    glutDisplayFunc(display);                   // Register display callback handler for window re-paint
    glutReshapeFunc(reshapeListener);           // Register callback handler for window re-shape
    glutKeyboardFunc(keyboardListener);         // Register callback handler for normal-key event
    glutSpecialFunc(specialKeyListener);        // Register callback handler for special-key event
    initGL();                                   // Our own OpenGL initialization
    glutMainLoop();                             // Enter the event-processing loop
    return 0;
}