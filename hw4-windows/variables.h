/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  
#include <glm/glm.hpp>
#include <vector>
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 
EXTERN vec3 rayvec;

#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point 
int amountinit = 5;
int w = 500, h = 500 ; // width and height 
float fovy = 90.0 ; // For field of view
#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int amountinit;
EXTERN int w, h ; 
EXTERN float fovy ; 
#endif 

EXTERN string outputfilename;
EXTERN int depth;
EXTERN int maxvertsnumber;
EXTERN float constant, linear, quadratic;
EXTERN unsigned int vertexshader, fragmentshader, shaderprogram ; // shaders
EXTERN mat4 projection, modelview; // The mvp matrices
EXTERN unsigned int projectionPos, modelviewPos; // Uniform locations of the above matrices
static enum {view, translate, scale} transop ; // which operation to transform 
enum shape {sphere,tri} ;
EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

EXTERN vector<vec3> dirlightposn; // directional Light Positions
EXTERN vector<vec3> dirlightcolor; // directional Light Colors
EXTERN vector<vec3> dirlightransf; // directional Lights transformed by modelview
EXTERN vector<vec3> pointlightposn ; // point Light Positions
EXTERN vector<vec3> pointlightcolor ; // point Light Colors
EXTERN vector<vec3> pointlightransf ; // point Lights transformed by modelview
EXTERN vector<vec3> vertices;

// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN vec3 norm;
EXTERN vec3 ambient ; 
EXTERN vec3 diffuse ;
EXTERN vec3 specular ;
EXTERN vec3 emission ;
EXTERN float shininess ; 

EXTERN struct object {
	shape type;
	vec3 position; // sphere
	float radius;  // radius for sphere
	int triverts[3];  // vertices for tri
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
	mat4 transform;
};
EXTERN vector<object> objects;
// For multiple objects, read from a file.  
/*
EXTERN struct sphere {
  shape type ; 
  vec3 position; // sphere
  float radius;  // radius for sphere
  
  vec3 ambient ; 
  vec3 diffuse ; 
  vec3 specular ;
  vec3 emission ; 
  float shininess ;
  mat4 transform ; 
} ;
EXTERN vector<sphere> spheres;
EXTERN struct triangle {
	shape type;

	int triverts[3];  // vertices for tri
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
	mat4 transform;
};
EXTERN vector<triangle> triangles;
*/