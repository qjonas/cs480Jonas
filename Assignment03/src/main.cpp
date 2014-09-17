#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting
#include <iostream>
#include <fstream>
#include <chrono>
#include <string.h>
#include "../src/loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

using namespace std;


//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};
// moon

//--Evil Global variables
//Just for this example!
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry


// More Global variables
bool rotate_flag = false;
bool rotation_dir = false; // true = clockwise 
bool rev_flag = false;
bool rev_dir = false; // true = clockewise
int rotation_spd = 20;

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader
// moon


//attribute locationsTor
GLint loc_position;
GLint loc_color;
// moon 


//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
// moon
glm::mat4 moon_model;//obj->world each object should have its own model matrix
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection
glm::mat4 mvp1;//premultiplied modelviewprojection

//--GLUT Callbacks
void render();
void update();
void reshape(int n_w, int n_h);
void keyboard(unsigned char key, int x_pos, int y_pos);
void arrowKey( int key, int x_pos, int y_pos );
void rotation_menu( int id );
void top_menu( int id );
void myMouse( int button, int state, int x, int y);
void idle();

//--Resource management
bool initialize();
void cleanUp();

// shader loader function
//char* loadShader( const char* fileName );

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;


//--Main
int main(int argc, char **argv)
{
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);

    // Name and create the Window
    glutCreateWindow("Matrix Example");

    // create menu
         int sub_menu;
        sub_menu = glutCreateMenu(rotation_menu);
       glutAddMenuEntry("Start Spinning", 2 );
       glutAddMenuEntry("Stop Spinning", 3 );
       glutAddMenuEntry("Reverse Spin Direction", 4 );
       glutCreateMenu(top_menu);
       glutAddMenuEntry("Quit", 1 );
       glutAddSubMenu("Rotation Control", sub_menu);
       glutAttachMenu(GLUT_RIGHT_BUTTON);

    //Glut  
       


    // Now that the window is created the GL context is fully set up
    // Because of that we can now initialize GLEW to prepare work with shaders
    GLenum status = glewInit();
    if( status != GLEW_OK)
    {
        std::cerr << "[F] GLEW NOT INITIALIZED: ";
        std::cerr << glewGetErrorString(status) << std::endl;
        return -1;
    }

    // Set all of the callbacks to GLUT that we need 
    // mouse call back added
    glutDisplayFunc(render);// Called when its time to display
    glutReshapeFunc(reshape);// Called if the window is resized
    glutIdleFunc(update);// Called if there is nothing else to do
    glutKeyboardFunc(keyboard);// Called if there is keyboard input
    glutSpecialFunc(arrowKey); // Called if there is arrow input
    glutMouseFunc(myMouse); // Called if there is mouse input

    // Initialize all of our resources(shaders, geometry)
    bool init = initialize();
    if(init)
    {
        t1 = std::chrono::high_resolution_clock::now();
        glutMainLoop();
    }

    // Clean up after ourselves
    cleanUp();
    return 0;
}

//--Implementations
void render()
{
    //--Render the scene

    //clear the screen
    glClearColor(0.0, 0.0, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //premultiply the matrix for this example
    mvp = projection * view * model;
    // second mvp matrix do below this
      

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));
	// second 

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);


    //set pointers into the vbo for each of the attributes(position and color)
// Second
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

// moon //////////////////////////


// add second

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

// add second 		
    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);

  //premultiply the matrix for this example
    mvp = projection * view * moon_model;
    // second mvp matrix do below this
      

    //enable the shader program
    glUseProgram(program);

    //upload the matrix to the shader
    glUniformMatrix4fv(loc_mvpmat, 1, GL_FALSE, glm::value_ptr(mvp));

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
    glEnableVertexAttribArray(loc_color);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);


    //set pointers into the vbo for each of the attributes(position and color)
// Second
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color));

// moon //////////////////////////


// add second

    glDrawArrays(GL_TRIANGLES, 0, 36);//mode, starting index, count

// add second 		
    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(loc_color);

                           
    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float angle = 0.0;
    static float rotate_angle = 0.0;
    static float moon_rotate_angle = 0.0;
    float dt = getDT();// if you have anything moving, use dt.

    if( rev_flag )
      {

        if( rev_dir )
          {
           angle += dt * M_PI/2; //move through 90 degrees a second
          }
        else
           {       
           angle += dt * -M_PI/2; //move through 90 degrees a second
           }
      }
 // rotate fast
  if( rotate_flag )
  {
   if( rotation_dir )
     {
      rotate_angle -= dt * M_PI * rotation_spd;
     }
 else
    {
     rotate_angle += dt * M_PI * rotation_spd;
    }
  }

    // moon rotate angle update
     moon_rotate_angle += dt * M_PI/6 * rotation_spd;

   model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0 , 4.0 * cos(angle)));
   // move the moon
   moon_model = glm::translate( glm::mat4(1.0f), 
       glm::vec3(4.0 * sin(moon_rotate_angle), 0.0 , 4.0 * cos(moon_rotate_angle)));
   moon_model = moon_model * model;
   
   // rotate the model
   model = glm::rotate( model, rotate_angle  , glm::vec3(0.0, 1.0, 0.0));
    
    // rotate the moon
    moon_model = glm::rotate( moon_model, moon_rotate_angle  , glm::vec3(0.0, 1.0, 0.0));

 
    // Update the state of the scene
    glutPostRedisplay();//call the display callback
}


void reshape(int n_w, int n_h)
{
    w = n_w;
    h = n_h;
    //Change the viewport to be correct
    glViewport( 0, 0, w, h);
    //Update the projection matrix as well
    //See the init function for an explaination
    projection = glm::perspective(45.0f, float(w)/float(h), 0.01f, 100.0f);

}

void keyboard(unsigned char key, int x_pos, int y_pos )
{
    // Handle keyboard input
    if(key == 27)//ESC
    {
        exit(0);
    }
    else if( key == 115 )// S
       {
        rotate_flag = !rotate_flag;
       }
    else if( key == 97 )// A
       {
        //Reverse spin Direction
        rotation_dir = !rotation_dir;
       }

    else if( key == 116 ) // T
      {
        // Speed up Spin
           rotation_spd++;
      }
    else if( key == 98 ) //B
      {
       // Slow Down spin
           rotation_spd--;
      }
    else if( key == 103 ) // G
      {
       // start / stop revolution
          rev_flag = !rev_flag;
      }
    else if( key == 114 ) // r
      {
       // start / stop revolution
          rev_dir = !rev_dir;
      }
 }

void arrowKey( int key, int x_pos, int y_pos  )
{
    if( key == GLUT_KEY_RIGHT) // ->
      {
       // start / stop revolution
          rev_dir = false;
      }
     else if( key == GLUT_KEY_LEFT ) // <-
      {
       // start / stop revolution
          rev_dir = true;
      }

}

bool initialize()
{
    // Initialize basic geometry and shaders for this example

    //this defines a cube, this is why a model loader is nice
    //you can also do this with a draw elements and indices, try to get that working
    Vertex geometry[] = { {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{-1.0, -1.0, -1.0}, {0.0, 0.0, 0.0}},

                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},
                          
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},

                          {{1.0, -1.0, -1.0}, {1.0, 0.0, 0.0}},
                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{1.0, 1.0, -1.0}, {1.0, 1.0, 0.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, -1.0}, {0.0, 1.0, 0.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},

                          {{1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}},
                          {{-1.0, 1.0, 1.0}, {0.0, 1.0, 1.0}},
                          {{1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}}
                        };
    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(geometry), geometry, GL_STATIC_DRAW);


    //--Geometry done

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);



    //--Geometry done

  //  GLuint vertex_shader1 = glCreateShader(GL_VERTEX_SHADER);
  //  GLuint fragment_shader1 = glCreateShader(GL_FRAGMENT_SHADER);


    //Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!
       loader sLoader("../bin/shader.txt" );
       loader fLoader("../bin/fragment.txt"); 
     const char *vs = sLoader.getData();

     const char *fs = fLoader.getData(); 

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE VERTEX SHADER!" << std::endl;
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] FAILED TO COMPILE FRAGMENT SHADER!" << std::endl;
        return false;
    }

    //Now we link the 2 shader objects into a program
    //This program is what is run on the GPU
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    //check if everything linked ok
    glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
    if(!shader_status)
    {
        std::cerr << "[F] THE SHADER PROGRAM FAILED TO LINK" << std::endl;
        return false;
    }

    //Now we set the locations of the attributes and uniforms
    //this allows us to access them easily while rendering
    loc_position = glGetAttribLocation(program,
                    const_cast<const char*>("v_position"));
    if(loc_position == -1)
    {
        std::cerr << "[F] POSITION NOT FOUND" << std::endl;
        return false;
    }

    loc_color = glGetAttribLocation(program,
                    const_cast<const char*>("v_color"));
    if(loc_color == -1)
    {
        std::cerr << "[F] V_COLOR NOT FOUND" << std::endl;
        return false;
    }

    loc_mvpmat = glGetUniformLocation(program,
                    const_cast<const char*>("mvpMatrix"));
    if(loc_mvpmat == -1)
    {
        std::cerr << "[F] MVPMATRIX NOT FOUND" << std::endl;
        return false;
    }
    
    //--Init the view and projection matrices
    //  if you will be having a moving camera the view matrix will need to more dynamic
    //  ...Like you should update it before you render more dynamic 
    //  for this project having them static will be fine
    view = glm::lookAt( glm::vec3(0.0, 8.0, -16.0), //Eye Position
                        glm::vec3(0.0, 0.0, 0.0), //Focus point
                        glm::vec3(0.0, 1.0, 0.0)); //Positive Y is up

    projection = glm::perspective( 45.0f, //the FoV typically 90 degrees is good which is what this is set to
                                   float(w)/float(h), //Aspect Ratio, so Circles stay Circular
                                   0.01f, //Distance to the near plane, normally a small value like this
                                   100.0f); //Distance to the far plane, 

    //enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //and its done
    return true;
}

void cleanUp()
{
    // Clean up, Clean up
    glDeleteProgram(program);
    glDeleteBuffers(1, &vbo_geometry);
// add second
}


//returns the time delta
float getDT()
{
    float ret;
    t2 = std::chrono::high_resolution_clock::now();
    ret = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
    t1 = std::chrono::high_resolution_clock::now();
    return ret;
}

// sub menu
void rotation_menu( int id )
{
 switch(id)
 { 
  case 2: // start rotation
	rotate_flag = true;
	break;
  case 3: // stop rotation
	rotate_flag = false;
	break;
  case 4: // switch rotation direction
	rotation_dir = !rotation_dir;
	break;
 }
 glutPostRedisplay();
} 

void top_menu( int id )
{
 switch(id)
 { 
  case 1:
	exit(0);
	break;
 }
 glutPostRedisplay();
} 

// left click to change directions
void myMouse( int button, int state, int x, int y)
{
 // switches rotation directions
   if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
     {
      rotation_dir = !rotation_dir;
     }
}










