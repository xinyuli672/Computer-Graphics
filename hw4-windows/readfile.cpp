// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Transform.h" 

using namespace std;
#include "variables.h" 
#include "readfile.h"

// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, GLfloat* values) 
{
    mat4 transform = transfstack.top(); 
    vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
    vec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
    mat4 &T = transfstack.top(); 
    T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, GLfloat* values) 
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i]; 
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
    }
    return true; 
}

void readfile(const char* filename) 
{
    string str, cmd; 
    ifstream in;
    in.open(filename); 
    if (in.is_open()) {

        // I need to implement a matrix stack to store transforms.  
        // This is done using standard STL Templates 
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0));  // identity

        getline (in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines 

                stringstream s(str);
                s >> cmd; 
                int i; 
                GLfloat values[10]; // Position and color for light, colors for others
                                    // Up to 10 params for cameras.  
                bool validinput; // Validity of input 

				// General Command
				if (cmd == "size") {  // size width height
					validinput = readvals(s, 2, values);
					if (validinput) {
						w = (int)values[0]; 
						h = (int)values[1];
						std::cout << "size set to  " << w <<" "<< h << "\n";

					}
				}
				else if (cmd == "maxdepth") { // maxdepth depth
					validinput = readvals(s, 1, values);
					if (validinput) {
						depth = (int)values[0];
					}
				}
				else if (cmd == "output") { // output filename
					s >> outputfilename;
					if (s.fail()) {
						cerr << "Failed reading output file name " << outputfilename << " will skip\n";
					}
				}
				// Camera Command
				else if (cmd == "camera") { // camera (lookfromx lookfromy lookfromz lookatx lookaty lookatz upx upy upz fov)
					validinput = readvals(s, 10, values); 
					if (validinput) {
						eyeinit = vec3(values[0], values[1], values[2]);
						center = vec3(values[3], values[4], values[5]);
						upinit = Transform::upvector(vec3(values[6], values[7], values[8]), eyeinit - center);
						fovy = values[9];
						std::cout << "camera set to  " << values[0] << " " << values[1] << " ... \n";
					}
				}
				// Geometry Command
				else if (cmd == "sphere") {  // sphere x y z radius
					validinput = readvals(s, 4, values);
					if (validinput) {
						object * obj = new object;
						obj->type = sphere;
						obj->position = vec3(values[0], values[1], values[2]);
						obj->radius = values[3];

						// Set the object's light properties
						for (i = 0; i < 3; i++) {
							(obj->ambient)[i] = ambient[i];
							(obj->diffuse)[i] = diffuse[i];
							(obj->specular)[i] = specular[i];
							(obj->emission)[i] = emission[i];
						}
						obj->shininess = shininess;
						obj->transform = transfstack.top();
						objects.push_back(*obj);
						delete obj;
					}
					
				}

				else if (cmd == "tri") {  // tri v1 v2 v3
					validinput = readvals(s, 3, values);
					if (validinput) {
						object * obj = new object;
						obj->type = tri;
						std::cout << "tri set to  " << values[0] << " " << values[1] << " " << values[2] << "\n";

						// Set the object's light properties
						for (i = 0; i < 3; i++) {
							(obj->ambient)[i] = ambient[i];
							(obj->diffuse)[i] = diffuse[i];
							(obj->specular)[i] = specular[i];
							(obj->emission)[i] = emission[i];
						}
						obj->shininess = shininess;
						for (i = 0; i < 3; i++) {
							obj->triverts[i] = (int)values[i];
						}
						obj->transform = transfstack.top();
						objects.push_back(*obj);
						std::cout << "tri ambient set to  " << ambient[0] << " " << ambient[1] << " " << ambient[2] <<" \n";
						std::cout << "tri diffuse set to  " << diffuse[0] << " " << diffuse[1] << " " << diffuse[2] << " \n";
						delete obj;
					}

				}

				else if (cmd == "maxverts") {  // maxverts number
					validinput = readvals(s, 1, values);
					if (validinput) {
						maxvertsnumber = (int)values[0];
						std::cout << "maxverts set to  " << maxvertsnumber << " \n";
					}
				}

				//else if (cmd == "maxvertnorms") {}  // maxvertnorms number

				else if (cmd == "vertex") {  // vertex x y z
					if (maxvertsnumber <= vertices.size()) {
						cerr << "Exceed max vertices number " << maxvertsnumber << " will skip\n";
					}
					else {
						validinput = readvals(s, 3, values);
						if (validinput) {
							vertices.push_back(vec3(values[0], values[1], values[2]));
						}
					}
				}

				//else if (cmd == "vertexnormal") {}  // vertexnormal x y z nx ny nz
				//else if (cmd == "trinormal") {} // trinormal v1 v2 v3

				// Transformation Command
				else if (cmd == "translate") {  // translate x y z 
					validinput = readvals(s, 3, values);
					if (validinput) {
						mat4 temp = Transform::translate(values[0], values[1], values[2]);
						rightmultiply(temp, transfstack);
					}
				}

				else if (cmd == "scale") {  // scale x y z  
					validinput = readvals(s, 3, values);
					if (validinput) {
						mat4 temp = Transform::scale(values[0], values[1], values[2]);
						rightmultiply(temp, transfstack);
					}
				}

				else if (cmd == "rotate") {  // rotate x y z angle
					validinput = readvals(s, 4, values);
					if (validinput) {
						vec3 norm_axis = glm::normalize(vec3(values[0], values[1], values[2]));
						mat4 temp = mat4(Transform::rotate(values[3], norm_axis));
						rightmultiply(temp, transfstack);
					}
				}

				else if (cmd == "pushTransform") {  // pushTransform
					transfstack.push(transfstack.top());
				}
				
				else if (cmd == "popTransform") {  // popTransform
					if (transfstack.size() <= 1) {
						cerr << "Stack has no elements.  Cannot Pop\n";
					}
					else {
						transfstack.pop();
					}
				}


                // Lighting Command
                else if (cmd == "directional") {  // directional x y z r g b
                    
					validinput = readvals(s, 6, values); 
					if (validinput) {
						dirlightposn.push_back(vec3(values[0],values[1],values[2]));
						dirlightcolor.push_back(vec3(values[3], values[4], values[5]));
					}
                }

				else if (cmd == "point") {  // point x y z r g b
					validinput = readvals(s, 6, values); 
					if (validinput) {
						pointlightposn.push_back(vec3(values[0],values[1],values[2]));
						pointlightcolor.push_back(vec3(values[3], values[4], values[5]));
					}
				}
				
				else if (cmd == "attenuation ") {  // attenuation const linear quadratic
					validinput = readvals(s, 3, values);
					if (validinput) {
						constant = values[0];
						linear = values[1];
						quadratic = values[2];
					}
				}

				else if (cmd == "ambient") {  // ambient r g b
					validinput = readvals(s, 3, values); // colors 
					if (validinput) {
						for (i = 0; i < 3; i++) {
							ambient[i] = values[i];
						}
					}
				}

                // Material Commands 
                // Ambient, diffuse, specular, shininess properties for each object.
                // Filling this in is pretty straightforward, so I've left it in 
                // the skeleton, also as a hint of how to do the more complex ones.
                // Note that no transforms/stacks are applied to the colors. 

                else if (cmd == "diffuse") {  // diffuse r g b
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            diffuse[i] = values[i]; 
                        }
                    }
                }
				else if (cmd == "specular") {  // specular r g b
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            specular[i] = values[i]; 
                        }
                    }
                }
				
				else if (cmd == "emission") {  // emission r g b
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            emission[i] = values[i]; 
                        }
                    }
                }

				else if (cmd == "shininess") {  // shininess s
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0]; 
                    }
                }

                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                }
            }
            getline (in, str); 
        }

        // Set up initial position for eye, up and amount
        // As well as booleans 

        eye = eyeinit; 
        up = upinit; 
        amount = 5;
        sx = sy = 1.0;  // keyboard controlled scales in x and y 
        tx = ty = 0.0;  // keyboard controllled translation in x and y  
		

    } else {
        cerr << "Unable to Open Input Data File " << filename << "\n"; 
        throw 2; 
    }
}
