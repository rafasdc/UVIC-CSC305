#include "Canvas.h"
#include <math.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


using namespace glm;
using namespace std;

vec3 camPos(0.0f,0.0f,3.0f);
vec3 up(0.0f,1.0f,0.0f);
vec3 target(0.0f,0.0f,0.0f);


mat4 model;
mat4 view;
mat4 pr;
mat4 Mv;
mat4 MvL;


float lastX = 256;
float lastY = 256;
float yaws = -90.0f;
float pitchs = 0.0f;

float zoom = -85;
float rotateCube = 25;

float vppos_x = 0;
float vppos_y = 0;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float linelength = 0;
int timercount = 0;
bool mousemoved = false;

unsigned int width = 1200;
unsigned int height = 1200;

float Rotation = 0;
float RotatingSpeed = 0.02;
Canvas canvas;

const GLfloat vpoint[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
   };

const char * vshader_light = " \
        #version 330 core \n\
        in vec3 vpoint; \
        uniform mat4 MvL; \
        void main() { \
            gl_Position =  MvL*vec4(vpoint,1.0f);\
        } \
        ";

const char * fshader_light = " \
        #version 330 core \n\
        out vec3 color; \
        void main() { color = vec3(1, 1, 1);} \
        ";

const char * vshader_square = " \
        #version 330 core \n\
        layout (location = 0) in vec3 position; \
        layout (location = 1) in vec3 normal;\
        in vec3 vpoint; \
        uniform mat4 model; \
        uniform mat4 view;\
        uniform mat4 pr;\
        uniform mat4 Mv;\
        out vec3 Normal;\
        out vec3 FragPos; \
        void main() { \
            gl_Position =  pr*view*model*vec4(position,1.0f);\
            Normal = normal;\
            FragPos = vec3(model*vec4(position,1.0f));\
        } \
        ";

const char * fshader_square = " \
        #version 330 core \n\
        out vec4 color; \
        in vec3 Normal; \
        in vec3 FragPos;\
         \
        uniform vec3 objectColor;\
        uniform vec3 lightColor; \
        uniform vec3 lightSource;\
        \
        void main() {\
            float ambientConstant = 0.1f;\
            vec3 ambient = ambientConstant * lightColor;\
            \
            vec3 norm = normalize(Normal);\
            vec3 lightDirection = normalize(lightSource - FragPos);\
            float diff = max(dot(norm,lightDirection),0.0);\
            vec3 diffuse = diff * lightColor;\
            \
            vec3 toApply = (ambient+diffuse) * objectColor;\
            color = vec4(toApply,1.0f);\
        }\
        ";






//OpenGL context variables
GLuint programID = 0;
GLuint VertexArrayID = 0;
GLuint modelGL = 0;
GLuint viewGL = 0;
GLuint prGL = 0;
GLuint MvGL = 0;
GLuint objectColorGL = 0;
GLuint lightColorGL = 0;
GLuint lightSourceGL = 0;

GLuint lightID = 0;
GLuint VertexArrayLight = 0;
GLuint MvLGL;



void InitializeCam(){


    model = rotate(model, radians(25.0f), vec3(1.0f,0.0f,0.5f));
    view = lookAt(camPos,target,up);
    pr = perspective(/* zoom */ radians(-85.0f),(float)width/(float)height,0.1f,100.0f);
    Mv = pr * view * model;
    mat4 modelL;
    modelL = translate(modelL, vec3(-0.75f,-1.95f,0.75f));
    modelL = scale(modelL,vec3(0.35f));
    MvL = pr* view * modelL;

}
void InitializeGL()
{

    glEnable(GL_DEPTH_TEST); //The cube loses 2 faces if this is not enabled.

    //Compile the shaders
    programID = compile_shaders(vshader_square, fshader_square);


    //Generate Vertex Array and bind the vertex buffer data

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    //glGenVertexArrays(1, &programID);
    //glBindVertexArray(programID);



    ///--- Generate memory for VBO
    GLuint VBO;
    glGenBuffers(1, &VBO);
    /// The subsequent commands will affect the specified buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /// Pass the vertex positions to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

    ///--- find the binding point in the shader:
    /// "vpoint" in the vertex shader
    glUseProgram(programID);
    //GLuint vpoint_id = glGetAttribLocation(programID, "vpoint");
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3, //size per vertex (3 floats for cord)
                          GL_FLOAT,
                          false, //don't normalize
                          6 * sizeof(GLfloat), //stride = 0
                          0); //offset = 0
    glVertexAttribPointer(1,
                          3, //size per vertex (3 floats for cord)
                          GL_FLOAT,
                          false, //don't normalize
                          6 * sizeof(GLfloat), //stride = 0
                          (GLvoid*)(3*sizeof(GLfloat))); //offset = 0
    glEnableVertexAttribArray(1);
    //Find the binding point for the uniform variable
    //glEnableVertexAttribArray(vpoint_id);
    MvGL = glGetUniformLocation(programID,"Mv");
    objectColorGL = glGetUniformLocation(programID,"objectColor");
    lightColorGL = glGetUniformLocation(programID,"lightColor");
    lightSourceGL = glGetUniformLocation(programID, "lightSource");
    modelGL = glGetUniformLocation(programID, "model");
    viewGL = glGetUniformLocation(programID,"view");
    prGL = glGetUniformLocation(programID,"pr");


    //glEnableVertexAttribArray(0);
    //glBindVertexArray(0);

// ------------- light ---------------

    lightID = compile_shaders(vshader_light, fshader_light);
    glGenVertexArrays(1, &VertexArrayLight);
    glBindVertexArray(VertexArrayLight);

    glUseProgram(lightID);
    //glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(0,
                          3, //size per vertex (3 floats for cord)
                          GL_FLOAT,
                          false, //don't normalize
                          6* sizeof(GLfloat), //stride = 0
                          0); //offset = 0
    glEnableVertexAttribArray(0);

    MvLGL = glGetUniformLocation(lightID,"MvL");




}

void MouseMove(double x, double y)
{
    lastX = vppos_x;
    lastY = vppos_y;
   //the pointer has moved
   vppos_x = (float)(x) / 256 - 1;
   vppos_y = 1 - (float)(y) / 256;
   mousemoved= true;
}

void MouseButton(MouseButtons mouseButton, bool press)
{
    if (mouseButton == LeftButton)
    {
        if (press == true) leftButtonPressed = true;
        else leftButtonPressed = false;
    }
    if (mouseButton == RightButton)
    {
        if (press == true) rightButtonPressed = true;
        else rightButtonPressed = false;
    }
}

void KeyPress(char keychar)
{

}

void OnPaint()
{


    //Binding the openGL context
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);
    glUniformMatrix4fv(viewGL,1,GL_FALSE,value_ptr(view));
    glUniformMatrix4fv(prGL,1,GL_FALSE,value_ptr(pr));
    glUniformMatrix4fv(MvGL,1,GL_FALSE,value_ptr(Mv));
    glUniform3f(objectColorGL, 1.0f,0.5f,0.31f);
    glUniform3f(lightColorGL, 1.0f,1.0f,1.0f);
    glUniform3f(lightSourceGL, -0.75f,-1.95f,0.75f);
    glUniformMatrix4fv(modelGL,1,GL_FALSE,value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0 , 36);
    //Clean up the openGL context for other drawings


    glUseProgram(0);
    glBindVertexArray(0);


    glUseProgram(lightID);
    glBindVertexArray(VertexArrayID);
    glUniformMatrix4fv(MvLGL,1,GL_FALSE,value_ptr(MvL));
    glDrawArrays(GL_TRIANGLES,0,36);
    glUseProgram(0);
    glBindVertexArray(0);



}

void HandleLeftClick(){
    if (leftButtonPressed){
        float xoffset = vppos_x - lastX;
        float yoffset = lastY - vppos_y;

        xoffset *= 45;
        yoffset *= 45;

        yaws += xoffset;
        pitchs += yoffset;

        vec3 F;
        F.x = cos(radians(yaws) * cos(radians(pitchs)));
        F.y = sin(radians(pitchs));
        F.z = sin(radians(yaws)) * cos(radians(pitchs));
        //normalize(F);
        camPos = F;
        //model = rotate(model, radians(25.0f), vec3(1.0f,0.0f,0.5f));
        view = lookAt(camPos,target,up);
        //pr = perspective(/* zoom */ radians(-85.0f),(float)width/(float)height,0.1f,100.0f);
        //Mv = pr * view * model;

    }

}

void HandleRightClick(){
    if (rightButtonPressed){
        if (vppos_y > lastY){
            zoom += 5;
            pr = perspective(/* zoom */ radians(zoom),(float)width/(float)height,0.1f,100.0f);
            //Mv = pr * view * model;
        } else {
            zoom -= 5;
            pr = perspective(/* zoom */ radians(zoom),(float)width/(float)height,0.1f,100.0f);
            //Mv = pr * view * model;
        }
    }
}

void OnTimer()
{
    HandleLeftClick();
    HandleRightClick();
}

int main(int, char **){

    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.05, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "Rotating Square Demo");
    //Do our initialization
    InitializeCam();
    InitializeGL();
    canvas.Show();

    return 0;
}

