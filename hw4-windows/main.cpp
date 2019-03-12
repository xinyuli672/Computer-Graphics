#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h"
#include <FreeImage.h>
#include <glm/glm.hpp>
#include <algorithm> 

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
#include "main.h"
/*
void saveScreenshot(string fname) {
  FreeImage_Initialise();
  int pix = w * h;
  BYTE *pixels = new BYTE[3*pix];	
  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

  std::cout << "Saving screenshot: " << fname << "\n";

  FreeImage_Save(FIF_PNG, img, fname.c_str(), 0);

  delete pixels;
  FreeImage_DeInitialise();
}
*/



vec3 rayThruPixel(float i, float j) {
	float fovyrad = fovy * pi / 180.0;
	vec3 a = eyeinit - center;
	vec3 b = upinit;
	vec3 wvec = glm::normalize(a);
	vec3 u = glm::normalize(glm::cross(b, wvec));
	vec3 v = glm::cross(wvec, u);
	float alpha = (glm::tan(fovyrad / 2) * w / h) * ((j - w / 2 ) / (w / 2));
	float beta = glm::tan(fovyrad / 2) * ((i - h / 2) / (h / 2));
	vec3 p1 = glm::normalize(alpha * u + beta * v - wvec);
	return p1;
}

bool insidetri(vec3 a, vec3 b, vec3 c, vec3 p) {
	vec3 v0 = c - a;
	vec3 v1 = b - a;
	vec3 v2 = p - a;
	float d00 = glm::dot(v0, v0);
	float d01 = glm::dot(v0, v1);
	float d02 = glm::dot(v0, v2);
	float d11 = glm::dot(v1, v1);
	float d12 = glm::dot(v1, v2);

	float denom = d00 * d11 - d01 * d01;
	float beta = (d11 * d02 - d01 * d12) / denom;
	float gemma = (d00 * d12 - d01 * d02) / denom;
	//float alpha = 1.0f - beta - gemma;
	if (beta + gemma <= 1 && beta <= 1 && gemma <= 1 && beta >= 0 && gemma >= 0)
		return true;
	else
		return false;
}

float intersect(const vec3 ray, const object obj, const vec3 pos) {

	if (obj.type == sphere) {

		float a = glm::dot(ray, ray);  // P1 ray direction
		float b = 2 * glm::dot(ray, pos - obj.position);
		float c = glm::dot(pos - obj.position, pos - obj.position) - obj.radius * obj.radius;
		float delta = b * b - 4 * a * c;
		//when no intersect
		if (delta < 0)
			return -1;
		//if there's interestion delta>0, calculate roots, choose smaller positive root
		else if (delta > 0) {
			float small = (-b - sqrt(delta)) / (2 * a);
			float big = (-b + sqrt(delta)) / (2 * a);
			if (small < 0)
				return big;
			else
				return small;
		}
		else if (delta == 0) {
			float root = (-b - sqrt(delta)) / (2 * a);
			if (root > 0)
				return root;
			else
				return -1;
		}
	}
	else if (obj.type == tri) {
		vec3 a = vertices[obj.triverts[0]];
		vec3 b = vertices[obj.triverts[1]];
		vec3 c = vertices[obj.triverts[2]];
		vec3 n = glm::normalize(glm::cross(b - a, c - a));

		if (glm::dot(ray, n) == 0.0f)
			return -1;
		float t = (glm::dot(a, n) - glm::dot(pos, n)) / glm::dot(ray, n);
		vec3 p = pos + ray * t;

		//check if it's inside triangle 

		//inside tri
		if (insidetri(a, b, c, p)) {
			return t;
		}
		//not inside
		else {
			return -1;
		}
	}else{
		return -1;
	}
}

void findColor(vec3 ray, BYTE *pixels, int index, int min_primitive) {
	if (min_primitive == -1) {
		pixels[index] = 0;
		pixels[index + 1] = 0;
		pixels[index + 2] = 0;
		return;
	}
	object obj = objects[min_primitive];

	//// the following segment of code is used for scene 1
	//// constant color
	//// uncomment the code for scene 1
	//if (obj.type == tri) {
	//	pixels[index] = 0;
	//	pixels[index + 1] = 0;
	//	pixels[index + 2] = 255;
	//}
		
	// uncomment the code for scene 2
	vec3 finalcolor = obj.ambient + obj.emission;
	/*	
	//point lights
	for (int i = 0; i < pointlightposn.size(); i++) {
		int uselight = 1;
		vec3 posn = pointlightposn[i];
		vec3 color = pointlightcolor[i];
		float dist = glm::length(posn - ray);
		vec3 shadowdir = glm::normalize(posn - ray);
		vec3 shadowpos = ray + shadowdir / 1000.0F;
		for (int j = 0; j < objects.size(); j++) {
			float hit = intersect( shadowdir, objects[j], shadowpos);
			if (hit >= 0) {
				uselight = 0;
				break;
			}
		}
		//if shadow ray hits object, then skip this light
		if (uselight == 0)
			continue;
		//shadow ray doesn't hit object
		vec3 eyedir = glm::normalize(eye - ray);
		vec3 halfvec = glm::normalize(shadowdir + eyedir);
		float nDotL = glm::dot(norm, shadowdir);
		vec3 lambert = obj.diffuse * pointlightcolor[i] * max(nDotL, 0.0F);
		float nDotH = glm::dot(norm, halfvec);
		vec3 phong = obj.specular * pointlightcolor[i] * pow(max(nDotH, 0.0F), obj.shininess);
		vec3 dAnds = (lambert + phong) / (constant + linear * dist + quadratic * dist * dist);
		finalcolor += dAnds;
	}
	//directional lights
	for (int i = 0; i < dirlightposn.size(); i++) {
		int uselight = 1;
		vec3 posn = dirlightposn[i];
		vec3 color = dirlightcolor[i];
		vec3 shadowdir = glm::normalize(posn);
		vec3 shadowpos = ray + shadowdir / 1000.0F;
		for (int j = 0; j < objects.size(); j++) {
			float hit = intersect(shadowdir, objects[j], shadowpos);
			if (hit >= 0) {
				uselight = 0;
				break;
			}
		}
		//if shadow ray hits object, then skip this light
		if (uselight == 0)
			continue;
		//shadow ray doesn't hit object
		vec3 eyedir = glm::normalize(eye - ray);
		vec3 halfvec = glm::normalize(shadowdir + eyedir);
		float nDotL = glm::dot(norm, shadowdir);
		vec3 lambert = obj.diffuse * pointlightcolor[i] * max(nDotL, 0.0F);
		float nDotH = glm::dot(norm, halfvec);
		vec3 phong = obj.specular * pointlightcolor[i] * pow(max(nDotH, 0.0F), obj.shininess);
		vec3 dAnds = lambert + phong;
		finalcolor += dAnds;
	}
	*/

	pixels[index] = finalcolor[2] * 255;
	pixels[index + 1] = finalcolor[1] * 255;
	pixels[index + 2] = finalcolor[0] * 255;
	
	
	
}


int main(int argc, const char* argv[]) {
		if (argc < 2) {
			cerr << "Usage: transforms scenefile";
			exit(-1);
		}
		readfile(argv[1]);
		std::cout << "file " << argv[1] << " is read \n";
		
		int pix = w * h;
		BYTE *pixels = new BYTE[3 * pix];

	

	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int index = 3 * (i * w + j);
			
			//ray trace in the center of grid
			vec3 raydir = rayThruPixel(i, j);
			int k = 0;
			//target and targetpos have the final intersect object and pos
			int min_primitive = -1;
			float min_t = INFINITY;
			//iterate through all objects
			
			while (k < objects.size()) {
				//eye = eyeinit;

				if (objects[k].transform != mat4(1.0f)) {
					eye = vec3(glm::inverse(objects[k].transform) * vec4(eyeinit,1));
					rayvec = vec3(glm::inverse(objects[k].transform) * vec4(raydir, 0));
				}
				else {
					eye = eyeinit;
					rayvec = raydir;
				}
				float hit = intersect(rayvec, objects[k], eye);

				if (hit >= 0) {
					//cout << hit << "\n";
					if (min_t > hit) {
						min_t = hit;
						min_primitive = k;
					}
				}
				k++;
			}
			//find intersect in object coordinate
			
			vec3 ray = eye + rayvec * min_t;
			if (min_primitive != -1) {
				//find normal of object in object coordinate
				object obj = objects[min_primitive];
				if (obj.type == tri) {
					vec3 A = vertices[obj.triverts[0]];
					vec3 B = vertices[obj.triverts[1]];
					vec3 C = vertices[obj.triverts[2]];
					norm = glm::normalize(glm::cross(B - A, C - A));
				}
				else if (obj.type == sphere) {
					norm = glm::normalize(ray - obj.position);
					//update normal to world coordinate
					norm = vec3((glm::transpose(glm::inverse(obj.transform))) * vec4(norm, 0.0f));
					norm = glm::normalize(norm);
				}
				//update ray to world coordinate
				ray = vec3(obj.transform * vec4(ray, 1));
			}
			findColor(ray, pixels, index, min_primitive);
		}
	}
	
	FreeImage_Initialise();
	
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	std::cout << "Saving screenshot: " << outputfilename << "\n";

	FreeImage_Save(FIF_PNG, img, outputfilename.c_str(), 0);

	delete pixels;
	FreeImage_DeInitialise();
	
	return 0;
}