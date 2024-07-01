#include <windows.h>  // for MS Windows
#include <GL/glut.h>  // GLUT, include glu.h and gl.h
#include <cmath>
#include "header.hpp"


using namespace std;

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
int drawgrid;
double angle_rotation = 0;
double nearplane = 1, farplane = 1000, fovY = 80, aspectRatio = 1;
bitmap_image image;
double windowWidth, windowHeight;
int pixels;

void rotate3D(point &vec,point &axis,double ang){
	vec = vec*cos(ang)+(axis*vec)*sin(ang);
}
void input()
{
    ifstream in("F:\\4-1\\CSE 410\\Offline 3\\description.txt");
    cout << "Taking input"<<endl;
    in >> nearplane >> farplane >> fovY >> aspectRatio;
    in >> recursionLevel;
    
    
    in >> pixels;
    cout<<pixels<<endl;
    Shape* sh = new Floor;
    in >> *((Floor*)sh);
    shapes.push_back(sh);

    int numObject;
    in >> numObject;
    cout << numObject << endl;
    for(int i = 0; i < numObject; i++)
    {
        string name;
        in >> name;
        Shape *shape; 
        if(name == "sphere")
        {
            shape = new Sphere;
            in >> *((Sphere*)shape);
            shapes.push_back(shape);
            cout << *((Sphere*)shape) << endl;
        }
        else if(name == "pyramid")
        {
            shape = new Pyramid;
            in >> *((Pyramid*)shape);
            shapes.push_back(shape);
            cout << *((Pyramid*)shape) << endl;
        }
        else if(name == "cube")
        {
            Shape* shape = new Cube;
            in >> *((Cube*)shape);
            shapes.push_back(shape);
            cout << *((Cube*)shape) << endl;
        }
    
    }
    int numPointLight;
    in >> numPointLight;
    cout << numPointLight << endl;
    for(int i = 0; i < numPointLight; i++)
    {
        PointLight *pointLight = new PointLight;
        in >> *pointLight;
        pointLights.push_back(pointLight);
        cout << *pointLight << endl;
    }
    int numSpotLight;
    in >> numSpotLight;
    cout << numSpotLight << endl;
    for(int i = 0; i < numSpotLight; i++)
    {
        SpotLight *spotLight = new SpotLight;
        in >> *spotLight;
        spotLights.push_back(spotLight);
        cout << *spotLight << endl;
    }
    cout << "Finished taking input" << endl;
}

void capture()
{
    cout << "In capture" << endl;
    image.setwidth_height(pixels, pixels);
    double planeDistance = (windowHeight/2)/tan(fovY*pi/360);

    point topLeft;
    topLeft = pos + l*planeDistance - r*(windowWidth/2) + u*(windowHeight/2);

    double du = windowWidth/pixels;
    double dv = windowHeight/pixels;

    point middle;
    middle = topLeft + r*(du/2) - u*(dv/2);

    double t, tMin;
    int index;

    for(int i = 0; i < pixels; i++)
    {
        for(int j = 0; j < pixels; j++)
        {
            point current;
            current = topLeft + r*(j*du) - u*(i*dv);

            Ray ray;
            ray.start = pos;
            ray.dir = current - pos;
            ray.dir.normalize();

            tMin = farplane;
            index = -1;
            Color colorMin(0, 0, 0);
            for(int k = 0; k < shapes.size(); k++)
            {
                Color color;
                t = shapes[k]->intersect(ray, color, recursionLevel);
                if(t < tMin && t > 0)
                {
                    tMin = t;
                    index = k;
                    colorMin = color;
                }
            }
          
            colorMin.fixRange();
            image.set_pixel(j, i, 255*colorMin.r, 255*colorMin.g, 255*colorMin.b);
           
        }
    }
    image.save_image("F:\\4-1\\CSE 410\\Offline 3\\out.bmp");
    cout << "Saving Image" << endl;
}

/* Draw axes: X in Red, Y in Green and Z in Blue */
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1,0,0);   // Red
        // X axis
        glVertex3f(-100,0,0);
        glVertex3f(100,0,0);

        glColor3f(0,1,0);   // Green
        // Y axis
        glVertex3f(0,-100,0);
        glVertex3f(0,100,0);

        glColor3f(0,0,1);   // Blue
        // Z axis
        glVertex3f(0,0,-100);
        glVertex3f(0,0,100);
    glEnd();
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

    for(int i = 0; i < shapes.size(); i++)
    {
        shapes[i]->draw();
    }

    for(int i = 0; i < pointLights.size(); i++)
    {
        pointLights[i]->draw();
    }

    for(int i = 0; i < spotLights.size(); i++)
    {
        spotLights[i]->draw();
    }

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
    gluPerspective(fovY, aspect, nearplane, farplane);
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

        case '0':
            capture();
            break;

        case 'a':
            rotate3D(r,u,pi/180);
            rotate3D(l,u,pi/180);
            break;
        case 'd':
            rotate3D(r,u,-pi/180);
            rotate3D(l,u,-pi/180);
            break;
        case 'w':
            pos.z += m;
            break;
        case 's':
            pos.z -= m;
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
    pos.x=0;pos.y=-160;pos.z=60;

    l.x=0;l.y=1;l.z=0;
    u.x=0;u.y=0;u.z=1;
    r.x=1;r.y=0;r.z=0;

    input();

    windowHeight = 2 * nearplane * tan(fovY * pi / 360);
    windowWidth = windowHeight * aspectRatio;
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