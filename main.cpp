#include <iostream>
#include <GL/glut.h>
#include "common.h"
#include "System.h"
#include "window.h"
#include <pthread.h>

using namespace std;

Window window;

void init()
{
    glClearColor(0,0,0,1);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    gluOrtho2D(0,window.width,0,window.height);

    System::getInstance()->setMode(2);
    System::getInstance()->load("rest.obj");
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    System* system = System::getInstance();

    glPointSize(5);
    glBegin(GL_POINTS);
    glColor4f(1,1,1,1);
    for(int i = 0; i < system->mass.size(); i++){
        glVertex2f(system->mass[i].x[0] + system->mass[i].r[0],
                system->mass[i].x[1] + system->mass[i].r[1]);
    }
    glEnd();

    glBegin(GL_LINES);
    for(int i = 0; i < system->mass.size(); i++){
        for(int j = 0; j < system->mass[i].strings.size(); j++){
            int strId = system->mass[i].strings[j];
            int a = system->str[strId].mass1;
            int b = system->str[strId].mass2;


            glVertex2f(system->mass[a].x[0] + system->mass[a].r[0],
                    system->mass[a].x[1] + system->mass[a].r[1]);
            glVertex2f(system->mass[b].x[0] + system->mass[b].r[0],
                    system->mass[b].x[1] + system->mass[b].r[1]);
        }
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
    int i = pthread_getconcurrency();
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

