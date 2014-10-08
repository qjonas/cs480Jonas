#include <GL/glew.h> // glew must be included before the main gl libs
#include <GL/glut.h> // doing otherwise causes compiler shouting


#ifdef ASSIMP_2
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#else
#include <assimp/Importer.hpp> // C++ importer Interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
//#include <assimp/color4.h> // allows for colors in models
#endif

#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string.h>
#include "../src/loader.h"
#include <vector>



#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //Makes passing matrices to shaders easier

using namespace std;


//--Data types
//This object will define the attributes of a vertex(position, color, etc...)
struct Vertex
{
    GLfloat position[3];
    GLfloat uv[2];
};
// moon

//--Evil Global variables
//Just for this example!
int w = 640, h = 480;// Window size
GLuint program;// The GLSL program handle
GLuint vbo_geometry;// VBO handle for our geometry
GLuint text; // this is for our textures I think

// More Global variable
char * objFname;
char * texFname;
unsigned int num_vertices;
bool rotate_flag = false;
bool rotation_dir = false; // true = clockwise 
bool rev_flag = false;
bool rev_dir = false; // true = clockewise
int rotation_spd = 20;

//uniform locations
GLint loc_mvpmat;// Location of the modelviewprojection matrix in the shader

Magick::Blob m_blob;

//attribute locationsTor
GLint loc_position;
//GLint loc_color;
//GLint text;
Glint texAttrib;


//transform matrices
glm::mat4 model;//obj->world each object should have its own model matrix
// moon model glm::mat4 moon_model
glm::mat4 view;//world->eye
glm::mat4 projection;//eye->clip
glm::mat4 mvp;//premultiplied modelviewprojection

// assimp model loader
Vertex* assimpLoader( /*const aiScene*& model*/ );

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

// model loader function
Vertex* objLoader( const char * obj_path ); 

//--Random time things
float getDT();
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;


//--Main
int main(int argc, char **argv)
{
    // file name
       objFname = argv[ 1 ];
	textFname = argv[2];
    // Initialize glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(w, h);

  //  const char * obj_name = argv[0];

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

    //set up the Vertex Buffer Object so it can be drawn
    glEnableVertexAttribArray(loc_position);
  //  glEnableVertexAttribArray(loc_color);

//set up the Texture so it can be bound
      glEnableVertexAttribArray(texAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
//activate and bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text);
	


    //set pointers into the vbo for each of the attributes(position and color)
    glVertexAttribPointer( loc_position,//location of attribute
                           3,//number of elements
                           GL_FLOAT,//type
                           GL_FALSE,//normalized?
                           sizeof(Vertex),//stride
                           0);//offset

    /*glVertexAttribPointer( loc_color,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(Vertex),
                           (void*)offsetof(Vertex,color)); */  

   glVertexAttribPointer( texAttrib,
			2,
			GL_FLOAT,
			GL_FLASE,
			sizeof(Vertex),
			(void*)offsetOf(Vertex,uv));
						


    glDrawArrays(GL_TRIANGLES, 0, num_vertices);//mode, starting index, count
		
    //clean up
    glDisableVertexAttribArray(loc_position);
    glDisableVertexAttribArray(textAttrib);
     
    //swap the buffers
    glutSwapBuffers();
}

void update()
{
    //total time
    static float angle = 0.0;
    static float rotate_angle = 0.0;
  //  static float moon_rotate_angle = 0.0;
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
    // moon_rotate_angle += dt * M_PI/6 * rotation_spd;

    model = glm::translate( glm::mat4(1.0f), glm::vec3(4.0 * sin(angle), 0.0 , 4.0 * cos(angle)));

   
    // rotate the model
    model = glm::rotate( model, rotate_angle  , glm::vec3(0.0, 1.0, 0.0));
    
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
    switch( key )
    {
    case 27://ESC
        exit(0);
	break;

    case 115:// S
	//Start the rotation
        rotate_flag = !rotate_flag;
	break;

    case 97:// A
        //Reverse spin Direction
        rotation_dir = !rotation_dir;
        break;

    case 116: // T
        // Speed up Spin
	rotation_spd++;
	break;

    case 98: //B
        // Slow Down spin
	rotation_spd--;
	break;

    case 103:  // G
        // start / stop revolution
	rev_flag = !rev_flag;
	break;

    case 114: // r
        // start / stop revolution
	rev_dir = !rev_dir;
	break;
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
    // where we load the model


   // assimp model loading

     
     // load details from scene
        Vertex* geometry = assimpLoader();
	Magick::Image * m_pImage = new Magick::Image(textFName);
	m_pImage->write(&m_blob, "RBGA" );



    // Create a Vertex Buffer object to store this vertex info on the GPU
    glGenBuffers(1, &vbo_geometry);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_geometry);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*num_vertices, geometry, GL_STATIC_DRAW);


    //--Geometry done

    // create A texture buffer object
	glGenTextures(1,&text);
//	GLenum TextureUnit;
	glActiveTexture(TextureUnit); // no clue here //////////////////////////////////////////////
	glBindTexture(GL_TEXTURE_2D, text);

glTexImage2D(m_textureTarget, 0, GL_RGBA, 
m_pImage->columns(), m_pImage->rows() GL_UNSIGNED_BYTE, m_blob.data());

glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);




    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);


    //Shader Sources
    // Put these into files and write a loader in the future
    // Note the added uniform!
       loader sLoader("../bin/newVsSader.txt" );
       loader fLoader("../bin/newFShader.txt"); 
       const char *vs = sLoader.getData();
       const char *fs = fLoader.getData(); 

    //compile the shaders
    GLint shader_status;

    // Vertex shader first
    glShaderSource(vertex_shader, 1, &vs, NULL);
    glCompileShader(vertex_shader);
    //check the compile status
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &shader_status);
  
    char buffer[512]; // buffer for error
    if(!shader_status)
    {
	glGetShaderInfoLog(vertex_shader,512,NULL,buffer);// inserts the error into the buffer
        std::cerr << buffer << std::endl; //prints out error
        return false;
    }

    // Now the Fragment shader
    glShaderSource(fragment_shader, 1, &fs, NULL);
    glCompileShader(fragment_shader);
    //check the compile status
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &shader_status);
    if(!shader_status)
    {
	glGetShaderInfoLog(fragment_shader,512,NULL,buffer);// inserts the error into the buffer
        std::cerr << buffer << std::endl; //prints out error
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

    texAttrib = glGetAttribLocation(program,
                    const_cast<const char*>("v_tex"));
    if(texAttrib == -1)
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

// code architecture taken from 
// www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
// provided opengl-tutorial.org
Vertex* objLoader( const char * path)
{ 
 // variables
    char lineHeader[200];
    int res;
    unsigned int  i = 0;
    vector<unsigned int> vertexIndices;
    vector<glm::vec3> temp_vertices;
//    int fCounter = 0;
 // open file
 FILE * fin = fopen( path, "r" );
// exit if the path is bad
 if( fin == NULL )
   {
    printf("IMPOSSIBLE TO OPEN FILE!!");
    // error message
    return NULL;
   }
// loop to get data either f or v 

 while( 1 )
 {
  res = fscanf( fin, "%s", lineHeader);
  if (res == EOF)
     {
      break;
     }
  else if( strcmp( lineHeader, "v") == 0 )
    {
	glm::vec3 vertex;
	fscanf( fin, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
	temp_vertices.push_back(vertex);
    } 
  else if( strcmp( lineHeader, "f" ) == 0 )
    {
     unsigned int vertList[3];
     fscanf( fin, "%d %d %d\n", &vertList[0], &vertList[1], &vertList[2] ); 
     vertexIndices.push_back(vertList[0]);
     vertexIndices.push_back(vertList[1]);
     vertexIndices.push_back(vertList[2]);
    }

//cout << "VList has" << vertList[0] << " "<< vertList[1] << " " << vertList[2] << endl;  
 }
//  cout << "VertIndices: " << fCounter*3 << endl;

   
// upload them 
	Vertex* model = new Vertex[vertexIndices.size()];

//     cout << "C0 CHeck" << endl;
i = 0;
 for( i = 0; i < vertexIndices.size(); i++ )
    {
//     cout << "Just the tip" << endl;
     unsigned int vertexIndex = vertexIndices[i];
//     cout << " i = " << i << endl;
//cout << "vertexIndex =" << vertexIndex << endl;
		glm::vec3 vert = temp_vertices[ vertexIndex-1];
//     cout << "vertices are: C1 " << vert.x << " " << vert.y << " " << vert.z << endl;
    // model.push_back(vert);

         model[i].position[0] = vert.x;
         model[i].color[0] = 0.5;

       
         model[i].position[1] = vert.y;
         model[i].color[1] = 0.4;

         model[i].position[2] = vert.z;
         model[i].color[2] = 0.8;

//     cout << "vertices are: " << vert.x << " " << vert.y << " " << vert.z << endl;
  }
num_vertices = vertexIndices.size();
 return model;
} // end of function

Vertex* assimpLoader( /*const aiScene*& scene*/ )
{
 // variables for triple loop
// cout << "in loader" << endl;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objFname, aiProcess_Triangulate);
    unsigned int i, j, k;
    num_vertices = 0;
for( i = 0; i < scene->mNumMeshes; i++ )
{
 num_vertices += scene->mMeshes[i]->mNumVertices;
}

//cout << "num Vertices: " << num_vertices << endl;
Vertex* ret_model = new Vertex[num_vertices];
for( i = 0; i < scene->mNumMeshes; i++ )
{
	for( j = 0; j < mesh->mNumFaces; j++ ) // loop through faces in mesh
	{
 //cout << "numfaces: " << mesh->mNumFaces << endl;
		const aiFace & face = mesh->mFaces[j];
 //cout << "face created "  << endl;
//cout << "num Indices: " << face.mNumIndices << endl;
		for( k = 0; k < 2; k++ ) // get the three indices from the face
		{
	//cout << "in k loop" << endl;
                 aiVector3D pos = mesh->mVertices[face.mIndices[k]];
		aiVector3d textureCoord = mesh->mTextureCoords[0][face.mIndices[k]];
 //cout << "pos created "  << endl;
		 ret_model[j].position[0] = pos.x;
		 ret_model[j].position[1] = pos.y;		 
		 ret_model[j].position[2] = pos.z;

		 ret_model[j].uv[0] = textureCoord.x;
		 ret_model[j].uv[1] = textureCoord.y;
		}

/*
            cout << "x pos: "<< mesh->mVertices[face.mIndices[0]].x<< endl;
		 ret_model[j].position[0] = scene->mMeshes[i]->
		 mVertices[scene->mMeshes[i]->mFaces[j].mIndices[0]].x; 
		 ret_model[j].position[1] = scene->mMeshes[i]->
		 mVertices[scene->mMeshes[i]->mFaces[j].mIndices[1]].y; 
		 ret_model[j].position[2] = scene->mMeshes[i]->
		 mVertices[scene->mMeshes[i]->mFaces[j].mIndices[2]].z; 
		// at face[j] get indice of the vertex and add it to the return model.
                 ret_model[j].position[0] = mesh->mVertices[face.mIndices[0]].x;
*/
		// ret_model++;
		}	 	
	}    
}
 //cout << "before the end" << endl;
//ret_model -= mesh->mNumFaces*3;
return ret_model;
}












