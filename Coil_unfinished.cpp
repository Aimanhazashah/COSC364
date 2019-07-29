/*========================================================================
* COSC 363  Computer Graphics (2018)
* Coil.cpp 
* See Lab06.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glut.h>
using namespace std;


vector<glm::vec4> vertices;     //Array of model vertices
vector<glm::vec4> centres;      //Array of slice centres

float angle = 0;                        //Camera angle
float degToRad = 3.14159265 / 180.0;    //Degrees to radians conversion factor

//
//  This function draws a grid of lines on the floor plane
//
void drawFloor()
{
    glColor3f(0., 0.5, 0.);         //Floor colour

    glBegin(GL_LINES);              //A set of grid lines on the xz-plane
    for (int i = -100; i <= 100; i+=4)
    {
        glVertex3f(-100, 0, i);
        glVertex3f(100, 0, i);
        glVertex3f(i, 0, -100);
        glVertex3f(i, 0, 100);
    }
    glEnd();
}

//
//  This function generates a base curve (an ellipse) containing
//  36 points. The curve is first created at the origin and then
//  translated to (xshift, yshift). 
//  The points are added to the array "vertices" and the
//  centre point to the array "centres"
//
void generateCurve()
{
    float a = 5, b = 2;     //semi major and minor axes lengths
    float xshift = 10;      //shift distance along x-axis
    float yshift = 4;       //shift distance along y-axis
    float x, y;

    vertices.clear();       //Clear vertex array
    for (int i = 0; i < 36; i++)
    {
        x = a*cos(i * 10 * degToRad) + xshift;
        y = b*sin(i * 10 * degToRad) + yshift;
        vertices.push_back(glm::vec4(x, y, 0, 1));
    }
    centres.push_back(glm::vec4(xshift, yshift, 0, 1));
}

//
//  This function generates the data for a helix model by
//  revolving the base curve about the y-axis, at the same time
//  shifting it along the y-axis.
//
void generateModel()
{
    float theta = 10.0*degToRad;    //Rotation angle = 10 degs
    float yshift = 0.2;
    int numPoints;    //Number of points in vertex array before update
    
    glm::mat4 idenMat = glm::mat4(1.0f);  //Identity matrix
//  glm::mat4 rotnMat =   //please complete
//  glm::mat4 prodMat =   //please complete
 
//------
        numPoints = vertices.size();
        for (int i = numPoints - 36; i < numPoints; i++)  //Iterate over the points on the last slice
        {
            //This is the transformed point of vertices[i]
            //glm::vec4 pt = 
            //Add the transformed point to the array using "vertices.push_back(pt)"
        }
        //This is the transformed version of the last centre point in the array "centres"
        //The last point in the array can be referenced using "centres.back()"
        //glm::vec4 ct = 
        //Add the transformed point to the array "centres".
//------
    
}

//
//  Sets the projection matrix and generates model data  
//
void initialize()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    //Background colour
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 1.0, 20., 1000.);

    generateCurve();
    generateModel();
}

//
//  This function assigns colour values at each vertex of the 
//  helix using a simple illumination model
//
void color(glm::vec4 point, glm::vec4 centre)
{
    glm::vec3 matCol(1.0, 0.5, 0.25);   //Material colour for the model
    glm::vec3 ambCol(0.2, 0.2, 0.2);    //Ambient colour
    glm::vec3 finalCol;
    glm::vec3 light(0, 50, 100);        //Lights position
    //Complete the function



    glColor3fv(glm::value_ptr(finalCol));
}

//
//  This function draws the coil model by connecting consecutive pairs of slices
//  using quad strips.
//
void drawCoil()
{
    glColor3f(0.0, 0.0, 1.0);
    for (int c = 0; c < centres.size()-1; c++)
    {
        glBegin(GL_QUAD_STRIP);
        for (int i = c*36;  i < (c+1)*36; i++)
        {
//          color(vertices[i], centres[c]);
            glVertex3fv(glm::value_ptr(vertices[i]));
            glVertex3fv(glm::value_ptr(vertices[i + 36]));
        }
        glVertex3fv(glm::value_ptr(vertices[c*36]));        //Extra quad to close the gap at the end.
        glVertex3fv(glm::value_ptr(vertices[(c+1)*36]));
        glEnd();
    }
}

//
// The main display module
// 
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(80 * sin(angle*degToRad), 30, 80 * cos(angle*degToRad), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    drawFloor();
    drawCoil();

    glFlush();
}


//
// Special key event callback to rotate the camera around the scene
// 
void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT) angle--;
    else if (key == GLUT_KEY_RIGHT) angle++;
    glutPostRedisplay();
}

//
// Main.  
//
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("3D Coil");

    glutDisplayFunc(display);
    glutSpecialFunc(special);
    initialize();

    glutMainLoop();
    return 0;
}
