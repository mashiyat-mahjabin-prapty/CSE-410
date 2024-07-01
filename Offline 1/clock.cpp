#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

double theta_sec=0;
double theta_min=0;
double theta_hr=0;
double theta_bob=0;
int sec = 0;
int min = 0;
int hr = 0;
int old_t = 0;

struct point
{
	double x,y,z;
};

void drawclock()
{
    float r=55.0;
    //second
    glLineWidth(2); // Make next lines wider
    glBegin(GL_LINES);  // Each set of 2 vertices form a line of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // White
        glVertex2d(0.0f, 0.0f);
        glVertex2d( r*sin(theta_sec), r*cos(theta_sec));
    glEnd();
    //minute
    glLineWidth(4); // Make next lines wider
    r = 40.0;
    glBegin(GL_LINES);  // Each set of 2 vertices form a line of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // White
        glVertex2d(0.0f, 0.0f);
        glVertex2d( r*sin(theta_min), r*cos(theta_min));
    glEnd();
    //hour
    glLineWidth(5); // Make next lines wider
    r = 30.0;
    glBegin(GL_LINES);  // Each set of 2 vertices form a line of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // White
        glVertex2d(0.0f, 0.0f);
        glVertex2d( r*sin(theta_hr), r*cos(theta_hr));
    glEnd();
    

}

void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

    float cx, cy, r;
    //outer circle
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        glColor3f(1.0f,1.0f,1.0f); 
        r = 70;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = r * cos(theta/180*pi);
            float y = r * sin(theta/180*pi);
            glVertex2f(x,y);
        }
    glEnd();

    //inner circle
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // Light-blue
        r = 65;
        int i = 0;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = r * sin(theta/180*pi);
            float y = r * cos(theta/180*pi);
            glVertex2f(x,y);
        }
    glEnd();

    //innermost circle
    glBegin(GL_LINE_LOOP);  // All vertices form a single loop of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // Light-blue
        r = 5;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = r * sin(theta/180*pi);
            float y = r * cos(theta/180*pi);
            glVertex2f(x,y);
        }
    glEnd();

    //clock ticks
    for(int i=0;i<60;i++){
		glBegin(GL_LINES);
		double x1,y1,x2,y2;
        x1 = 65.0f * cos((2*pi/60.0)*i);
        y1 = 65.0f * sin((2*pi/60.0)*i);

		if( i%5 == 0 ){
			x2 = 55.0f * cos((2*pi/60.0)*i);
        	y2 = 55.0f * sin((2*pi/60.0)*i);
		}else{
			x2 = 60.0f * cos((2*pi/60.0)*i);
       		y2 = 60.0f * sin((2*pi/60.0)*i);
		}
        glVertex2f(x1, y1); 
        glVertex2f(x2,y2); 
    glEnd();
	}
    
    double theta_max = pi/5.0;
    double theta = theta_max*cos(theta_bob*pi+theta_max);
    //pendulum stick
    double x3, x4, y3, y4;
    glBegin(GL_LINES);  // Each set of 2 vertices form a line of single pixel width
        glColor3f(1.0f,1.0f,1.0f);  // White
        glVertex2d(0.0f, -70.0f);
        x3 = -50.0f*sin(theta) + 0;
        y3 = -50.0*cos(theta) - 70;
        glVertex2d(x3, y3);
    glEnd();

    //bob
    glBegin(GL_POLYGON);    // All vertices form a single polygon
        glColor3f(1.0f,0.5f,0.0f);  // Orange
        cx = x3;
        cy = y3;
        r = 10;
        for (float theta = 0; theta < 360; theta += 10) {
            float x = cx + r * cos(theta/180*pi);
            float y = cy + r * sin(theta/180*pi);
            glVertex2f(x,y);
        }
    glEnd();
    drawclock();
	glutSwapBuffers();
}


void animate(){
	//codes for any changes in Models, Camera
    int t = glutGet(GLUT_ELAPSED_TIME);
    float elapsed_time = (t-old_t) / 1000.0;

    if(elapsed_time >= 1.0)
    {
        old_t = t;
        sec++;
        if(sec >= 60)
        {
            sec = abs(sec-60);
            min++;       
        }
        if(min >= 60)
        {
            min = abs(min-60);
            hr++;
        }
        theta_sec = (2*pi/60.0)*sec;
        theta_min = (2*pi/60.0)*min;
        theta_hr = (2*pi/12.0)*hr+(0.5*pi/180.0)*min;
    }
    theta_bob += 1.0f/(60.0);

	glutPostRedisplay();
}

void Initialize() 
{
    theta_sec = 0.0;
    theta_min = 0.0;
    theta_hr = 0.0;
    old_t = 0;

    time_t cur_time;
    time(&cur_time);
    struct tm *local = localtime(&cur_time);
    hr = local->tm_hour%12;
    min = local->tm_min%60;
    sec = local->tm_sec%60;

    glClearColor(0.0,0.0,0.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80,	1,	1,	1000.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);                  // Initialize GLUT
    glutInitWindowSize(640, 640);           // Set the window's initial width & height
    glutInitWindowPosition(0, 0);         // Position the window's initial top-left corner
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("2D Animated Clock");  // Create a window with the given title
    Initialize();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing
    glutDisplayFunc(display);               // Register display callback handler for window re-paint
    glutIdleFunc(animate);
    glutMainLoop();                         // Enter the infinitely event-processing loop
    return 0;
}

