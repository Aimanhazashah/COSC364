//assignment

#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <fstream>
#include "loadTGA.h"
using namespace std;
//###########
#define MAX_DROPS 10000000
#define GRAVITY -0.0005
int NumDrops = 0;
//###########


float x_axis_door = 20;
float z_axis_door = -120;
        
//--------------------
//camera attributes
GLUquadric *q; 
float lookAngle = 0.0;      //Camera rotation
float look_x, look_z=-1., eye_x, eye_z;  //Camera parameters
float cdr=3.14159265/180.0; //Conversion from degrees to radians
//--------------------
//====================
float tank_x = 0;
float tank_y = 0;
float tank_z = 0;
float tank_theta = 0;
float tank_stop_time = 0;
bool tank_moving = true;
bool tank_reversing = true;
bool tank_south = true;
bool tank_east = false;
bool tank_north = false;
bool tank_west = false;
//====================
float train_z = 0;
float barrier_y  = 0;
float train_timer = 0;
bool train_moving = true;
//====================
bool open_door_bool = true;
#define GL_CLAMP_TO_EDGE 0x812F   //To get rid of seams between textures
GLuint texId[9];

void tank_movement() 
{
    
    if (tank_moving) {
        if (tank_south) {
            if (tank_x <= -100.0) {
                tank_theta += 1.5;
                if (tank_theta == 90.0) {
                    tank_south = false;
                    tank_east = true;
                }
            } else {
                tank_x -= 1.0;
            }
        } else if (tank_east) {
            if (tank_z >= 100.0) {
                tank_theta += 2.5;
                if (tank_theta == 180.0) {
                    tank_east = false;
                    tank_north = true;
                }
            } else {
                tank_z += 1.0;
            }
        } else if (tank_north) {
            if (tank_x >= 100.0) {
                tank_theta += 1.5;
                if (tank_theta == 270.0) {
                    tank_north = false;
                    tank_west = true;
                }
            } else {
                tank_x += 1.0;
            }
        } else if (tank_west) {
            if (tank_z <= -100.0) {
                tank_theta += 1.5;
                if (tank_theta == 360.0) {
                    tank_west = false;
                    tank_south = true;
                    tank_theta = 0;
                }
            } else {
                tank_z -= 1.0;
            }
        }
    } 
}

void wagon_movement()
{
    if(train_moving){
        if(train_z <= 300){
            train_z += 1;
            if(train_z == 40)
            {
                train_moving = false;
            }
            if(train_z == 300)
            {
                train_z = -300;
            }
            if(train_z >= 100 && train_z <= 170 )
            {
                barrier_y -= 0.2;
            }
        }
    } 
    else {
        train_timer += 1;
        if(train_timer >= 300)
        {
            train_moving = true;
            train_timer = 0;
        } 
        else {
            if(train_timer >= 100 && train_timer <= 170)
            {
                barrier_y += 0.2;
            }
        }
        
    }
}

//------Waterfall System（Partical system *)------------------------------------------------------
typedef struct {
    int alive;
    float xpos, ypos;
    float xdir, ydir;
    float mass;
} Particle;

Particle water[MAX_DROPS];
void draw_waterfall(void)
{
    int i;
    glColor3f(0.0, 0.5, 1.0);
    glBegin(GL_POINTS);
    for ( i=0 ; i<NumDrops ; i++ ){
        if (water[i].alive) {
            glVertex2f(water[i].xpos, water[i].ypos);
        }
    }
    glEnd();
    glFlush();
}

void time_step(void)
{
    int i;
    for ( i=0 ; i<NumDrops ; i++ ) {
        if (water[i].alive) {           // set up an object to hit
            if (water[i].ypos + GRAVITY*water[i].mass < -0.75){ // bounce it off of the "floor"
                water[i].ydir = -water[i].ydir;
            } 
            else {            // let gravity do its thing
            water[i].ydir += GRAVITY * water[i].mass;
            }
        water[i].xpos += water[i].xdir;
        water[i].ypos += water[i].ydir;
        if (water[i].ypos < -1.0 || water[i].xpos > 1.0){
            water[i].alive = 0;}
        }
    }
}

void drop_generator(void)
{
    int i;
    float newdrops = drand48()*60;

    if (NumDrops + newdrops > MAX_DROPS){
        newdrops = MAX_DROPS - NumDrops;
    }
    
    for ( i=NumDrops ; i<NumDrops+newdrops ; i++ ) {
        water[i].alive = 1; 
        water[i].xpos = -0.8 + 0.01*drand48();
        water[i].ypos = 0.8 + 0.01*drand48();
        water[i].xdir = 0.0075 + 0.0025*drand48();
        water[i].ydir = 0.0;
        water[i].mass = 0.5 + 0.5*drand48();
    }
    NumDrops += newdrops;
}

void waterfall_base(void)
{
    glPushMatrix();
    glTranslatef(25,6,0);
    glutSolidTeapot(1);
    glPopMatrix(); 
    
    glPushMatrix();     //waterfall
        glTranslatef(25,6,0);
        glScalef(20,20,15);
        drop_generator();
        draw_waterfall();
        time_step();
    glPopMatrix();
}

//====


                    
void myTimer(int value)
{ 
    tank_movement();
    wagon_movement();
    glutPostRedisplay();
    glutTimerFunc(20, myTimer, 0);
}

void loadGLTextures()               // Load bitmaps And Convert To Textures
{
    glGenTextures(9, texId);        // Create texture ids
    // * left *
    glBindTexture(GL_TEXTURE_2D, texId[0]);
    loadTGA("./Texture/left.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // * front *
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    loadTGA("./Texture/front.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // * right *
    glBindTexture(GL_TEXTURE_2D, texId[2]);
    loadTGA("./Texture/right.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // * back***
    glBindTexture(GL_TEXTURE_2D, texId[3]);
    loadTGA("./Texture/back.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // * top *
    glBindTexture(GL_TEXTURE_2D, texId[4]);
    loadTGA("./Texture/up.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    // * down *
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    loadTGA("./Texture/down.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    // * road *
    glBindTexture(GL_TEXTURE_2D, texId[6]);
    loadTGA("./Texture/roads.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    // G R E A T .  K I M . P O S T E R
    glBindTexture(GL_TEXTURE_2D, texId[7]);
    loadTGA("./Texture/kim.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    // Front side of building
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    loadTGA("./Texture/front_building.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    // Other side of bui
    glBindTexture(GL_TEXTURE_2D, texId[9]);
    loadTGA("./Texture/kim.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void skybox(){
  glEnable(GL_TEXTURE_2D);
  
  ////////////////////// LEFT WALL ///////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[0]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f(-200,  0, 200);
  glTexCoord2f(1. ,0.);
  glVertex3f(-200, 0., -200);
  glTexCoord2f(1. ,1.);
  glVertex3f(-200, 200., -200);
  glTexCoord2f(0. ,1.);
  glVertex3f(-200, 200, 200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);

  ////////////////////// FRONT WALL ///////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[1]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f(-200,  0, -200);
  glTexCoord2f(1. ,0.);
  glVertex3f(200, 0., -200);
  glTexCoord2f(1. ,1.);
  glVertex3f(200, 200, -200);
  glTexCoord2f(0. ,1.);
  glVertex3f(-200,  200, -200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);
  
 ////////////////////// RIGHT WALL ///////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[2]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f(200,  0, -200);
  glTexCoord2f(1. ,0.);
  glVertex3f(200, 0, 200);
  glTexCoord2f(1. ,1.);
  glVertex3f(200, 200,  200);
  glTexCoord2f(0. ,1.);
  glVertex3f(200,  200,  -200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);

  ////////////////////// REAR WALL ////////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f( 200, 0, 200);
  glTexCoord2f(1. ,0.);
  glVertex3f(-200, 0,  200);
  glTexCoord2f(1. ,1.);
  glVertex3f(-200, 200,  200);
  glTexCoord2f(0. ,1.);
  glVertex3f( 200, 200, 200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);

  /////////////////////// TOP //////////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f(-200, 200, -200);
  glTexCoord2f(1. ,0.);
  glVertex3f(200, 200,  -200);
  glTexCoord2f(1. ,1.);
  glVertex3f(200, 200,  200);
  glTexCoord2f(0. ,1.);
  glVertex3f(-200, 200, 200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);

  /////////////////////// FLOOR //////////////////////////
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texId[5]);
  glBegin(GL_QUADS);
  glTexCoord2f(0. ,0.);
  glVertex3f(-200, 0., 200);
  glTexCoord2f(1. ,0.);
  glVertex3f(200, 0.,  200);
  glTexCoord2f(1. ,1.);
  glVertex3f(200, 0., -200);
  glTexCoord2f(0. ,1.);
  glVertex3f(-200, 0., -200);
  
  glEnd();
  glDisable(GL_TEXTURE_2D);
  
  glDisable(GL_TEXTURE_2D);
}

//---------------------------------------------------------------------
void initialise(void)
{
    loadGLTextures();

    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_NORMALIZE);
    
    // --------------   T   E   S   T
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    //  Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);

    //  Define light's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);
    // --------------   T   E   S   T
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   //Background colour
    //----------------
    //uncertain traits
    glEnable(GL_LIGHTING);                  //Enable OpenGL states
    //glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    //glEnable(GL_COLOR_MATERIAL);
    //glEnable(GL_NORMALIZE);
    //

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 5000.0);
}

//---------------------------------------------------------------------
//  T R A I N S
void base()
{
    glColor4f(0.2, 0.2, 0.2, 1.0);   //The base is nothing but a scaled cube
    glPushMatrix();
      glTranslatef(0.0, 4.0, 0.0);
      glScalef(20.0, 2.0, 10.0);     //Size 20x10 units, thickness 2 units.
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();                 //Connector between wagons
      glTranslatef(11.0, 4.0, 0.0);
      glutSolidCube(2.0);
    glPopMatrix();

    //Wheels
    glColor4f(0.5, 0., 0., 1.0);
    glPushMatrix();
      glTranslatef(-8.0, 2.0, 5.1);
      gluDisk(q, 0.0, 2.0, 20, 2);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(8.0, 2.0, 5.1);
      gluDisk(q, 0.0, 2.0, 20, 2);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(-8.0, 2.0, -5.1);
      glRotatef(180.0, 0., 1., 0.);
      gluDisk(q, 0.0, 2.0, 20, 2);
    glPopMatrix();
    glPushMatrix();
      glTranslatef(8.0, 2.0, -5.1);
      glRotatef(180.0, 0., 1., 0.);
      gluDisk(q, 0.0, 2.0, 20, 2);
    glPopMatrix();
}

void engine()
{
    base();

    //Cab
    glColor4f(0.8, 0.8, 0.0, 1.0);
    glPushMatrix();
      glTranslatef(7.0, 8.5, 0.0);
      glScalef(6.0, 7.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();

    glPushMatrix();
      glTranslatef(6.0, 14.0, 0.0);
      glScalef(4.0, 4.0, 8.0);
      glutSolidCube(1.0);
    glPopMatrix();

    //Boiler
    glPushMatrix();
      glColor4f(0.5, 0., 0., 1.0);
      glTranslatef(4.0, 10.0, 0.0);
      glRotatef(-90.0, 0., 1., 0.);
      gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
      glTranslatef(0.0, 0.0, 14.0);
      gluDisk(q, 0.0, 5.0, 20, 3);
      glColor4f(1.0, 1.0, 0.0, 1.0);
      glTranslatef(0.0, 4.0, 0.2);
      gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
    glPopMatrix();

}

void texture_mapping()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[7]);
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(134.5,20,0);
      glTexCoord2f(0,0);
      glVertex3f(134.5,3,0);
      glTexCoord2f(1,0);
      glVertex3f(134.5,3,40);
      glTexCoord2f(1,1);
      glVertex3f(134.5,20,40);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void wagon()
{
    base();

    glColor4f(0.0, 1.0, 1.0, 1.0);
    glPushMatrix();
      glTranslatef(0.0, 10.0, 0.0);
      glScalef(18.0, 10.0, 10.0);
      glutSolidCube(1.0);
    glPopMatrix();
    glPushMatrix();
      glColor4f(0.5, 0., 0., 1.0);
      glTranslatef(4.0, 10.0, 0.0);
      glRotatef(-90.0, 0., 1., 0.);
      gluCylinder(q, 5.0, 5.0, 14.0, 20, 5);
      glTranslatef(0.0, 0.0, 14.0);
      gluDisk(q, 0.0, 5.0, 20, 3);
      glColor4f(1.0, 1.0, 0.0, 1.0);
      glTranslatef(0.0, 4.0, 0.2);
      gluDisk(q, 0.0, 1.0, 20,2);  //headlight!
    glPopMatrix();
}

//---------------------------------------------------------------------

void long_ass_wagon()
{
    glPushMatrix();
    glTranslatef(140,1,0);
    glRotatef(90,0,1,0);
    wagon();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(140,1,20);
    glRotatef(90,0,1,0);
    wagon();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(140,1,40);
    glRotatef(90,0,1,0);
    wagon();
    glPopMatrix();
}

void roads()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[6]);
    glBegin(GL_QUADS);
    glTexCoord2f(100.0, 0.0);
    glVertex3f(150, 1, 350);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(150, 1, -350);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(130, 1, -350);
    glTexCoord2f(100.0, 1.0);
    glVertex3f(130, 1, 350);
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
}

void tunnel()
{
    //GLUquadric *q;
    int baseRadius = 30;
    int topRadius = 30;
    int height = 100; //length of the tunnel
    int slices = 20;
    int stacks = 50;
    //glPushMatrix(); 
      //glTranslatef(140.0, 5.0, -155.0); //tunnel
      //glRotatef(180,0,1,0);
    gluCylinder(q,baseRadius, topRadius, height, slices, stacks);
    //glPopMatrix();
}

void stump(){
    glColor4f(1.0, 1.0, 1.0, 1.0);   //The base
    glPushMatrix();
      glTranslatef(0.0, 0.0, 0.0);
      glScalef(2.5, 70.0, 2.5);  
      glutSolidCube(1.0);
    glPopMatrix();
}

void barriers_arms()
{
    GLUquadric *q;
    int baseRadius = 1;
    int topRadius = 1;
    int height = 25; //length of the turret
    int slices = 20;
    int stacks = 10;
    q = gluNewQuadric();
    glPushMatrix(); 
      glColor4f(1.0, 1.0, 1.0, 1.0);   //The base
      glRotatef(90,0,1,0);
      glTranslatef(0.0, 10.0, 0.0); //turret
      gluCylinder(q,baseRadius, topRadius, height, slices, stacks);
    glPopMatrix();
}

void street_post()
{
    GLUquadric *q;
    int baseRadius = 1;
    int topRadius = 1;
    int height = 25; //length of the turret
    int slices = 20;
    int stacks = 10;
    q = gluNewQuadric();
    glColor4f(0.2, 0.2, 0.2, 1.0);   //The base
    glPushMatrix(); 
      glRotatef(270,1,0,0);
      glTranslatef(155.0, 130.0, 0.0); //turret
      gluCylinder(q,baseRadius, topRadius, height, slices, stacks);
    glPopMatrix();

    glPushMatrix();
      glColor3f(1, 1, 1);  
      glTranslatef(153.0, 23.0, -130.0); //turret base
      glScalef(10.0, 5.0, 10.0);  
      glutSolidCube(1.0);
    glPopMatrix();
}

void open_door(){
        //open door
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texId[7]);
        glBegin(GL_QUADS);
          glTexCoord2f(0,1);
          glVertex3f(-10,30,-120);
          glTexCoord2f(0,0);
          glVertex3f(-10,1,-120);
          glTexCoord2f(1,0);
          glVertex3f(x_axis_door,1,z_axis_door);
          glTexCoord2f(1,1);
          glVertex3f(x_axis_door,30,z_axis_door);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
        

void street_post_set(){
    
    glTranslatef(0,0,20);
    street_post();
    glTranslatef(0,0,70);
    street_post();
    glTranslatef(0,0,120);
    street_post();
}

void tank_display(){
    glPushMatrix();
    glTranslatef(tank_x, tank_y, tank_z);
    glRotatef(tank_theta,0,1,0);
    engine(); 
    glPopMatrix();
}

void train_display(){
    glPushMatrix();
    glTranslatef(0, 1, train_z);
    long_ass_wagon(); 
    glPopMatrix();
}
void barrier_display(){
    glTranslatef(120.0,barrier_y,93);
    barriers_arms();
} 

void building()
{
    //Front Right side
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0.666,1);
      glVertex3f(20,50,-120);
      glTexCoord2f(0.666,0.666);
      glVertex3f(20,1,-120);
      glTexCoord2f(1,0.666);
      glVertex3f(50,1,-120);
      glTexCoord2f(1,1);
      glVertex3f(50,50,-120);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //Front Center side
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0.333,1);
      glVertex3f(-10,50,-120);
      glTexCoord2f(0.333,0.6);
      glVertex3f(-10,30,-120);
      glTexCoord2f(0.666,0.6);
      glVertex3f(20,30,-120);
      glTexCoord2f(0.666,1);
      glVertex3f(20,50,-120);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //Front Left side
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0,0.333);
      glVertex3f(-40,50,-120);
      glTexCoord2f(0,0);
      glVertex3f(-40,1,-120);
      glTexCoord2f(0.333,0);
      glVertex3f(-10,1,-120);
      glTexCoord2f(0.333,0.333);
      glVertex3f(-10,50,-120);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //Right side 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(50,50,-120);
      glTexCoord2f(0,0);
      glVertex3f(50,1,-120);
      glTexCoord2f(1,0);
      glVertex3f(50,1,-170);
      glTexCoord2f(1,1);
      glVertex3f(50,50,-170);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //Left side 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(-40,50,-120);
      glTexCoord2f(0,0);
      glVertex3f(-40,1,-120);
      glTexCoord2f(1,0);
      glVertex3f(-40,1,-170);
      glTexCoord2f(1,1);
      glVertex3f(-40,50,-170);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    //Back side 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[8]);
    glBegin(GL_QUADS);
      glTexCoord2f(0,1);
      glVertex3f(-40,50,-170);
      glTexCoord2f(0,0);
      glVertex3f(-40,1,-170);
      glTexCoord2f(1,0);
      glVertex3f(50,1,-170);
      glTexCoord2f(1,1);
      glVertex3f(50,50,-170);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//---------------------------------------------------------------------
void display(void)
{
    q = gluNewQuadric();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye_x, 10, eye_z,  look_x, 10, look_z,   0, 1, 0);
    
    glPushMatrix();
    skybox();
    glPopMatrix();
    
    glPushMatrix();
    tank_display(); //======== Handles the tank :D
    glPopMatrix();

    glPushMatrix();
    roads();
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(-10,-10,-160);  // Waterfall
        waterfall_base();
    glPopMatrix();

    glPushMatrix();
    barrier_display();
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(120.0,0,93);
    stump();
    glPopMatrix();
    
    glPushMatrix();
    building();
    glPopMatrix();
    
    glPushMatrix();
    street_post_set();
    glPopMatrix();
    
    glPushMatrix();
    train_display(); //======== Handles the train :D
    glPopMatrix();
    
    open_door();
    
    glPushMatrix(); //South Tunnel
    //glColor3f(151,105,79);  display Brown color Tunnel
    glTranslatef(140.0, 5.0, -155.0); //tunnel
    glRotatef(180,0,1,0);
    tunnel();
    glPopMatrix();
    
    glPushMatrix();  //North Tunnel
    //glColor3f(151,105,79);  display Brown color Tunnel
    glTranslatef(140.0, 5.0, 235.0); //tunnel
    glRotatef(180,0,1,0);
    tunnel();
    glPopMatrix();
    
    glFlush();
}

//--------------------------------------------------------------
void special(int key, int x, int y)
 {
    if(key==GLUT_KEY_LEFT) lookAngle-=5;         //Turn left
    else if(key==GLUT_KEY_RIGHT) lookAngle+=5;   //Turn right
    else if(key == GLUT_KEY_DOWN)
    {  //Move backward
        eye_x -= 5*sin(lookAngle * cdr);
        eye_z += 5*cos(lookAngle * cdr);

    }
    else if(key == GLUT_KEY_UP)
    { //Move forward
        eye_x += 5*sin(lookAngle * cdr);
        eye_z -= 5*cos(lookAngle * cdr);
    }
    
    if (key==GLUT_KEY_F2)
    {
        if(open_door_bool){
        x_axis_door = -10;
        z_axis_door = -90;
        open_door_bool = false;
         }
         else 
         {
            x_axis_door = 20;
            z_axis_door = -120;
            open_door_bool = true;
         }
    }
    

    look_x = eye_x + 100*sin(lookAngle * cdr);
    look_z = eye_z - 100*cos(lookAngle * cdr);

    glutPostRedisplay();
}
//-------------------------------------------------------------------

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_DEPTH );
   glutInitWindowSize (1000, 1000); 
   glutInitWindowPosition (10, 10);

   glutCreateWindow ("assignment");
   initialise();
   glutDisplayFunc(display);
   glutSpecialFunc(special);
   glutTimerFunc(100, myTimer, 0);

   glutMainLoop();
   return 0;
}

