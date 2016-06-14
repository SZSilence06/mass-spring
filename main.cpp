#include <iostream>
#include <GL/glut.h>
#include "common.h"
#include "System.h"
#include "window.h"

using namespace std;

Window window;

void init()
{
    glClearColor(0,0,0,1);
    glEnable(GL_POINT_SMOOTH);
    gluOrtho2D(0,window.width,0,window.height);

    System::getInstance()->setMode(2);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPointSize(5);
    glBegin(GL_POINTS);
    glColor4f(1,1,1,1);
    for(int i = 0; i < 5; i++){
        glVertex2f(System::getInstance()->mass[i].x * 50, window.height / 2);
    }
    glEnd();

    glBegin(GL_LINES);
    for(int i = 0; i < 4; i++){
        glVertex2f(System::getInstance()->mass[i].x * 50, window.height / 2);
        glVertex2f(System::getInstance()->mass[i+1].x * 50, window.height / 2);
    }
    glEnd();

    glFlush();

    glutSwapBuffers();
}

void reshape(int w,int h)
{
    glViewport(0,0,w,h);
}

void onTimer(int timerId)
{
    System::getInstance()->simulate();

    glutPostRedisplay();

    glutTimerFunc(33,onTimer,1);
}

int main(int argc,char **argv)
{
    window.width = window.height = 500;

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window.width,window.height);
    glutInitWindowPosition(400,400);
    glutCreateWindow("GL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(33,onTimer,1);
    glutMainLoop();
    return 0;
}

