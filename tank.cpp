#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <GL/glut.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>
using namespace std;
struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;
struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;
GLuint programID;
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	return ProgramID;
}
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors
	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = red;
		color_buffer_data [3*i + 2] = red;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}
void draw3DObject (struct VAO* vao)
{
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);
	glBindVertexArray (vao->VertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex		 0; 3 vertices total -> 1 triangle
}
char int2str(int x) {
	// converts int to strin
	char s[100];
	s[0] = x/10-'0';
	s[1] = x%10 - '0';
	//	return s;
}
/*void drawBitmapText(char *string,float x,float y,float z) 
{  
	char *c;
	glRasterPos3f(x, y,z);

	for (c=string; *c != '\n'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

void render(void)
{ 
	glClear(GL_COLOR_BUFFER_BIT); 
	glLoadIdentity();
	drawBitmapText("Osama Hosam's OpenGL Tutorials",200,200,0);
	//glutSwapBuffers(); 
}*/
void keyboardDown (unsigned char key, int x, int y)
{
	switch (key) {
		case 'Q':
		case 'q':
		case 27: //ESC
			exit (0);
		default:
			break;
	}
}
float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;
float trianglerotation = 0;
int flag = 0,dflag1=0,dflag2=0,dflag3=0,dflag4=0,dflag5=0;
int tflag1=0,tflag2=0,tflag3=0,tflag4=0,tflag5=0;
float x = -3.5 , y = -3.5;
double theta = 0;
double sine;
double cosine ;
int velocity = 11;
int targets = 5;
int lives = 5;
int score = 0;
int change = 1;
void keyboardUp (unsigned char key, int x, int y)
{
	switch (key) {
		case 'b':
			triangle_rotation = triangle_rotation - 2;
			trianglerotation = trianglerotation - 3;
			break;
		case 'a':
			triangle_rotation = triangle_rotation + 2;
			trianglerotation = trianglerotation + 3;
			break;
		case ' ':
			flag = 1;
			theta = 45+trianglerotation;
			sine = sin(theta*M_PI/180.0f);
			cosine = cos(theta*M_PI/180.0f);
			change = 1;
			score -= 10;
			//printf("theta = %lf sine = %lf cosine = %lf\n",theta,sine,cosine);
			break;
		case 'f':
			velocity++;
			break;
		case 's':
			velocity--;
			break;
		default:
			break;
	}
}
/*float camera_rotation_angle = 90;
  float rectangle_rotation = 0;
  float triangle_rotation = 0;
  float trianglerotation = 0;*/
void keyboardSpecialDown (int key, int x, int y)
{
	switch(key){
		case 'a':
			//			triangle_rotation = triangle_rotation + 2;
			//			trianglerotation = trianglerotation + 3;
		case 'b':
			//			triangle_rotation = triangle_rotation - 2;
			//			trianglerotation = trianglerotation - 3;
		default:
			break;
	}
}
void keyboardSpecialUp (int key, int x, int y)
{
}
VAO *triangle, *rectangle,*triangle1,*bomb,*target1,*target2,*target3,*target4,*target5,*danger1,*danger2,*danger3,*danger4,*danger5;
void mouseClick (int button, int state, int x, int y)
{
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN){
				triangle_rotation = triangle_rotation + 2;
				trianglerotation = trianglerotation + 3;
			}
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN) {
				triangle_rotation = triangle_rotation - 2;
				trianglerotation = trianglerotation - 3;
			}
			break;
		default:
			break;
	}
}
void mouseMotion (int x, int y)
{
}
void reshapeWindow (int width, int height)
{
	GLfloat fov = 90.0f;
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	Matrices.projection = glm::perspective (fov, (GLfloat) width / (GLfloat) height, 0.1f, 500.0f);
	Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}
//VAO *triangle, *rectangle;
void createTriangle ()
{
	static const GLfloat vertex_buffer_data [] = {
		0.2, 0,2, // vertex 0
		0,0.2,2, // vertex 1
		0.4,0.6,2,// vertex 2

		0.4,0.6,2,
		0.6,0.4,2,
		0.2,0,2
	};
	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		0,0,1, // color 2
		0,1,0, // color 3

		0,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};
	triangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createTriangle1 ()
{
	static const GLfloat vertex_buffer_data [] = {
		-0.1,-0.1,2, // vertex 0
		-0.14,-0.06,2, // vertex 1
		0.40,0.44,2,// vertex 2

		0.40,0.44,2,
		0.42,0.42,2,
		-0.1,-0.1,2
	};
	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		0,0,1, // color 2
		0,1,0, // color 3

		0,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};
	triangle1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createRectangle ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.5,1, // vertex 2
		0.5,0.5,1, // vertex 3

		0.5,0.5,1, // vertex 3
		0.5,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		0,0,1, // color 2
		0,1,0, // color 3

		0,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};
	rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createbomb ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 1
		1,0,1, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		0.3,0.3,0.3, // color 4
		1,0,0  // color 1
	};
	bomb = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createtarget1 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,1,0, // color 1
		1,1,1, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		1,1,0, // color 4
		1,1,0  // color 1
	};
	target1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createtarget2 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,1,0, // color 1
		1,1,0, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		1,1,0, // color 4
		1,1,0  // color 1
	};
	target2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createtarget3 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,1,0, // color 1
		1,1,0, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		1,1,0, // color 4
		1,1,0  // color 1
	};
	target3 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createtarget4 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,1,0, // color 1
		1,1,0, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		1,1,0, // color 4
		1,1,0  // color 1
	};
	target4 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createtarget5 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		1,1,0, // color 1
		1,1,0, // color 2
		1,1,0, // color 3

		1,1,0, // color 3
		1,1,0, // color 4
		1,1,0  // color 1
	};
	target5 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createdanger5 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		2,.1,.1, // color 1
		2,.1,.1, // color 2
		2,.1,.1, // color 3

		2,.1,.1, // color 3
		2,.1,.1, // color 4
		2,.1,.1  // color 1
	};
	danger5 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createdanger1 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		2,.1,.1, // color 1
		2,.1,.1, // color 2
		2,.1,.1, // color 3

		2,.1,.1, // color 3
		2,.1,.1, // color 4
		2,.1,.1  // color 1
	};
	danger1 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createdanger2 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		2,.1,.1, // color 1
		2,.1,.1, // color 2
		2,.1,.1, // color 3

		2,.1,.1, // color 3
		2,.1,.1, // color 4
		2,.1,.1  // color 1
	};
	danger2 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createdanger3 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		2,.1,.1, // color 1
		2,.1,.1, // color 2
		2,.1,.1, // color 3

		2,.1,.1, // color 3
		2,.1,.1, // color 4
		2,.1,.1  // color 1
	};
	danger3 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
void createdanger4 ()
{
	const GLfloat vertex_buffer_data [] = {
		0,0,1, // vertex 1
		0,0.16,1, // vertex 2
		0.16,0.16,1, // vertex 3

		0.16,0.16,1, // vertex 3
		0.16,0,1, // vertex 4
		0,0,1  // vertex 1
	};
	static const GLfloat color_buffer_data [] = {
		2,.1,.1, // color 1
		2,.1,.1, // color 2
		2,.1,.1, // color 3

		2,.1,.1, // color 3
		2,.1,.1, // color 4
		2,.1,.1  // color 1
	};
	danger4 = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}
float i ;
void ClearScreen()
{
	system("clear");
	/*char temp = lives + '0';
	  char live[] = "LIVES REMAINING" ;
	//printf("ok\n");
	RenderString(-0.5,1.8 , GLUT_BITMAP_TIMES_ROMAN_24, live, 0.2f, 0.3f, 0.4f);*/
	printf("LIVES REMAINING:- %d\n",lives);
	printf("SCORE:- %d\n",score);
	printf("TARGETS REMAINING:- %d\n",targets);
	if(lives == 0  || targets == 0){
		lives = 5;
		targets = 5;
		score = 0;
		printf("**********************NEW GAME**********************\n");
	}
	change = 0;
	//	cout << string( 100, '\n' );
}
void draw ()
{
	if(change)
		ClearScreen();
	//printf("OK\n");
	//Sprint(10, 1014, array ); // Print text to screen
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (programID);
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	glm::vec3 target (0, 0, 0);
	glm::vec3 up (0, 1, 0);
	Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
	glm::mat4 VP = Matrices.projection * Matrices.view;
	glm::mat4 MVP;	// MVP = Projection * View * Model
	//ClearScreen();
	Matrices.model = glm::mat4(1.0f);
	glm::mat4 translateTriangle = glm::translate (glm::vec3(-3.7f, -3.7f, 0.0f)); // glTranslatef
	glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0));  // rotate about vector (1,0,0)
	glm::mat4 triangleTransform = translateTriangle * rotateTriangle;
	Matrices.model *= triangleTransform; 
	MVP = VP * Matrices.model; // MVP = p * V * M*/
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(triangle);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translateRectangle = glm::translate (glm::vec3(-4, -4, 0));        // glTranslatef
	glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
	Matrices.model *= (translateRectangle /* rotateRectangle*/);
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(rectangle);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translateTriangle1 = glm::translate (glm::vec3(-3.5, -3.5, 0.0f));        // glTranslatef
	glm::mat4 rotateTriangle1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 triangle1Transform = translateTriangle1 * rotateTriangle1;
	Matrices.model *= triangle1Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(triangle1);
	if(i == 2){
		flag = 0;
		x = -3.5;
		y = -3.5;
		i = 0;
	}
	//ClearScreen();
	if(flag){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatebomb = glm::translate (glm::vec3(x,y, 0.0f));        // glTranslatef
		//glm::mat4 rotatebomb = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); // rotate about vector (-1,1,1)
		glm::mat4 trianglebomb = translatebomb;
		Matrices.model *= trianglebomb;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(bomb);
		y = velocity*sine*i - 5*i*i - 3.5;
		x = velocity*i*cosine - 3.5;
		if(2.84 <=x && x <= 3.16 && 2.84 <=y && y <= 3.16){
			if(!dflag1){
				change = 1;
				targets--;
				score += 100;
			}
			dflag1 = 1;
		}
		else if(2.94 <=x && x <= 3.26 && 2.04 <=y && y <= 2.36){
			if(!dflag2){
				change = 1;
				targets--;
				score += 80;
			}
			dflag2 = 1;
		}
		else if(3.04 <=x && x <= 3.36 && -3.46 <=y && y <= -3.14){
			if(!dflag3){
				change = 1;
				targets--;
				score += 60;
			}
			dflag3 = 1;
		}
		else if(2.34 <=x && x <= 2.66 && -1.46 <=y && y <= -1.14){
			if(!dflag4){
				change = 1;
				targets--;
				score += 40;
			}
			dflag4 = 1;
		}
		else if(1.94 <=x && x <= 2.26 && -1.86 <=y && y <= -1.54){
			if(!dflag5){
				change = 1;
				targets--;
				score += 20;
			}
			dflag5 = 1;
		}
		else if(-0.16 <=x && x <= 0.16 && -0.16 <=y && y <= 0.16){
			change = 1;
			lives--;
			dflag2 = 0;
			dflag1 = 0;
			dflag3 = 0;
			dflag4 = 0;
			dflag5 = 0;
			y = 5;
		}
		else if(0.84 <=x && x <= 1.16 && -0.16 <=y && y <= 0.16){
			change = 1;
			lives--;
			dflag2 = 0;
			dflag1 = 0;
			dflag3 = 0;
			dflag4 = 0;
			dflag5 = 0;
			y = 5;
		}
		else if(1.84 <=x && x <= 2.16 && -0.16 <=y && y <= 0.16){
			change = 1;
			lives--;
			dflag2 = 0;
			dflag1 = 0;
			dflag3 = 0;
			dflag4 = 0;
			dflag5 = 0;
			y = 5;
		}
		else if(-1.16 <=x && x <= -0.84 && -0.16 <=y && y <= 0.16){
			change = 1;
			lives--;
			dflag2 = 0;
			dflag1 = 0;
			dflag3 = 0;
			dflag4 = 0;
			dflag5 = 0;
			y = 5;
		}
		else if(-2.16 <=x && x <= -1.84 && -0.16 <=y && y <= 0.16){
			change = 1;
			lives--;
			dflag2 = 0;
			dflag1 = 0;
			dflag3 = 0;
			dflag4 = 0;
			dflag5 = 0;
			y = 5;
		}
		if( y < -4 || y > 4 || x < -4 || x > 4)
		{
			flag = 0;
			x = -3.5;
			y = -3.5;
			i = -0.01;
		}
		//printf("%lf %lf\n",x,y);
		i += 0.01;
	}
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translatetarget1 = glm::translate (glm::vec3(0.0, 0.0, 0.0f));        // glTranslatef
	//	glm::mat4 rotatetarget1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 target1Transform = translatetarget1;
	Matrices.model *= target1Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(target1);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translatetarget2 = glm::translate (glm::vec3(1.0, 0.0, 0.0f));        // glTranslatef
	//	glm::mat4 rotateTriangle1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 target2Transform = translatetarget2 ;
	Matrices.model *= target2Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(target2);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translatetarget3 = glm::translate (glm::vec3(2.0, 0.0, 0.0f));        // glTranslatef
	//glm::mat4 rotatetarget3 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 target3Transform = translatetarget3;
	Matrices.model *= target3Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(target3);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translatetarget4 = glm::translate (glm::vec3(-2.0, 0.0, 0.0f));        // glTranslatef
	//	glm::mat4 rotateTrian = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 target4Transform = translatetarget4;
	Matrices.model *= target4Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(target4);
	Matrices.model = glm::mat4(3.0f);
	glm::mat4 translatetarget5 = glm::translate (glm::vec3(-1.0, 0.0, 0.0f));        // glTranslatef
	//	glm::mat4 rotateTriangle1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
	glm::mat4 target5Transform = translatetarget5;
	Matrices.model *= target5Transform;
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
	draw3DObject(target5);
	if(!dflag1){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatedanger1 = glm::translate (glm::vec3(3.0, 3.0, 0.0f));        // glTranslatef
		//	glm::mat4 rotatedanger1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
		glm::mat4 danger1Transform = translatedanger1;
		Matrices.model *= danger1Transform;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(danger1);
	}
	if(!dflag2){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatedanger2 = glm::translate (glm::vec3(3.1, 2.2, 0.0f));        // glTranslatef
		//	glm::mat4 rotateTriangle1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
		glm::mat4 danger2Transform = translatedanger2 ;
		Matrices.model *= danger2Transform;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(danger2);
	}
	if(!dflag3){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatedanger3 = glm::translate (glm::vec3(3.2, -3.3, 0.0f));        // glTranslatef
		//glm::mat4 rotatedanger3 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
		glm::mat4 danger3Transform = translatedanger3;
		Matrices.model *= danger3Transform;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(danger3);
	}
	if(!dflag4){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatedanger4 = glm::translate (glm::vec3(2.5, -1.3, 0.0f));        // glTranslatef
		//	glm::mat4 rotateTrian = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
		glm::mat4 danger4Transform = translatedanger4;
		Matrices.model *= danger4Transform;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(danger4);
	}
	if(!dflag5){
		Matrices.model = glm::mat4(3.0f);
		glm::mat4 translatedanger5 = glm::translate (glm::vec3(2.1, -1.7, 0.0f));        // glTranslatef
		//	glm::mat4 rotateTriangle1 = glm::rotate((float)(trianglerotation*M_PI/180.0f), glm::vec3(0.0,0.0,1.0)); 
		glm::mat4 danger5Transform = translatedanger5;
		Matrices.model *= danger5Transform;
		MVP = VP * Matrices.model;
		glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
		draw3DObject(danger5);
	}
	//render();
	glutSwapBuffers ();
}
void idle () {
	draw (); // drawing same scene
}
void initGLUT (int& argc, char** argv, int width, int height)
{
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB |  GLUT_DEPTH );
	glutInitContextVersion (3, 3); // Init GL 3.3
	glutInitContextFlags (GLUT_CORE_PROFILE); // Use Core profile - older functions are deprecated
	glutInitWindowSize (width, height);
	glutCreateWindow ("TANK FIGHT");
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
		exit (1);
	}
	glutKeyboardFunc (keyboardDown);
	glutKeyboardUpFunc (keyboardUp);
	glutSpecialFunc (keyboardSpecialDown);
	glutSpecialUpFunc (keyboardSpecialUp);
	glutMouseFunc (mouseClick);
	glutMotionFunc (mouseMotion);
	glutReshapeFunc (reshapeWindow);
	glutDisplayFunc (draw); // function to draw when active
	glutIdleFunc (idle); // function to draw when idle (no I/O activity)
	//glutIgnoreKeyRepeat (false); // Ignore keys held down
}

void menu(int op)
{
	switch(op)
	{
		case 'Q':
		case 'q':
			exit(0);
	}
}
void addGLUTMenus ()
{
	int subMenu = glutCreateMenu (menu);
	glutAddMenuEntry ("Do Nothing", 0);
	glutAddMenuEntry ("Really Quit", 'q');
	glutCreateMenu (menu);
	glutAddSubMenu ("Sub Menu", subMenu);
	glutAddMenuEntry ("Quit", 'q');
	glutAttachMenu (GLUT_MIDDLE_BUTTON);
}
void initGL (int width, int height)
{
	createTriangle ();
	createTriangle1 ();
	createbomb ();
	createtarget1 ();
	createtarget2 ();
	createtarget3 ();
	createtarget4 ();
	createtarget5 ();
	createdanger1 ();
	createdanger2 ();
	createdanger3 ();
	createdanger4 ();
	createdanger5 ();
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");
	reshapeWindow (width, height);
	glClearColor (0.2f, 0.3f, 0.4f, 0.0f); // R, G, B, A
	glClearDepth (0.5f);
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	createRectangle ();
	//	createTriangle2 ();
	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}
int main (int argc, char** argv)
{
	i = 0;
	/*while(i<=2){
	  y = 14*(.707)*i - 5*i*i;
	  x = 14*i*(.707);
	  printf("%lf %lf\n",x,y);
	  i += .2;
	  }*/
	//printf("%lf\n",sin(M_PI/4))
	int width = 800;
	int height = 800;
	initGLUT (argc, argv, width, height);
	addGLUTMenus ();
	initGL (width, height);
	glutMainLoop ();
	return 0;
}
