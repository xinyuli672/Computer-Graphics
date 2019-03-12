#include "WorkingScene.h"
#include "math.h"

// This file includes the basic functions that your program must fill in.  
// Note that there are several helper functions from Curve.cpp that can be used!
// In particular, take a look at moveActivePoint, connectTheDots, drawLine, etc.

// What happens when you drag the mouse to x and y?  
// In essence, you are dragging control points on the curve.
void WorkingScene::drag(int x, int y) {
	/* YOUR CODE HERE */
	//you must figure out how to transform x and y so they make sense
	//update oldx, and oldy
	//make sure scene gets redrawn
	theOnlyCurve->moveActivePoint((float)(x -oldx) / width, (float)(oldy - y) / height);
	oldx = x;
	oldy = y;
	glutPostRedisplay();
}

// Mouse motion.  You need to respond to left clicks (to add points on curve) 
// and right clicks (to delete points on curve) 
void WorkingScene::mouse(int button, int state, int x, int y) {
	// theOnlyCurve is the current type of curve being drawn. It is created in Scene.cpp.
	if (theOnlyCurve && state == GLUT_DOWN) {
		if (button == GLUT_LEFT_BUTTON) {
			/* YOUR CODE HERE */
			theOnlyCurve->addPoint((float)x / width, (float)(height - y) / height);
		}
		if (button == GLUT_RIGHT_BUTTON) {	
			/* YOUR CODE HERE */
			theOnlyCurve->updateActivePoint((float)x / width, (float)(height - y) / height);
			theOnlyCurve->deleteActivePoint();
		}
	}

	/* YOUR CODE HERE */
	//update oldx, and oldy
	//make sure scene gets redrawn
	oldx = x;
	oldy = y;
	glutPostRedisplay();
}



#include "Bezier.h"


//helper fcn to calculate the (n choose k) part in the bernstein-bezier polynimials
int factorial(int a) {
	if (a <= 1) return 1;
	return a * factorial(a - 1);
}
int bernstein_coefficient(int n, int k) {
	return factorial(n) / (factorial(k) * factorial(n - k));
}
// Bezier drawing function.  This is by deCasteljau or equivalent algorithm. 
// It should support Bezier curves of arbitrary degree/order.
// F(u) = summation k of P_k * B_k^n(u)
void Bezier::draw(int levelOfDetail) {

	connectTheDots();
	int i, j, k;
	Pvector::iterator p;
	//Pvector allpoints;

	/* YOUR CODE HERE */
	float Px, Py, Nx, Ny, u;
	if ((int)points.size() >= 2) {
		Px = (points.begin())->x;
		Py = (points.begin())->y;
		for (i = 0; i < levelOfDetail + 1; i++) {
			u = (float)1 / levelOfDetail;
			Nx = 0;
			Ny = 0;
			int n = points.size() - 1;
			for (p = points.begin(), j = points.size() - 1, k = 0; p != points.end(); p++, j--, k++) {
				Nx += bernstein_coefficient(n, k)*pow(u*i, k)*pow((1 - u * i), j)*(p->x);
				Ny += bernstein_coefficient(n, k)*pow(u*i, k)*pow((1 - u * i), j)*(p->y);
			}
			drawLine(Px, Py, Nx, Ny);
			Px = Nx;
			Py = Ny;
		}
	}

}



#include "Bspline.h"

// The B-Spline drawing routine.  
// Remember to call drawSegment (auxiliary function) for each set of 4 points.
void Bspline::draw(int levelOfDetail) {
	connectTheDots();
	/* YOUR CODE HERE */
	if (points.size() >= 4) {
		for (Pvector::iterator p = points.begin(); p != (points.end() - 3); p++) {
			drawSegment(p, p + 1, p + 2, p + 3, levelOfDetail);
		}
	}
}

void Bspline::drawSegment(Pvector::iterator p1, Pvector::iterator p2, Pvector::iterator p3, Pvector::iterator p4, int levelOfDetail) {
	float x, y, Px, Py;

	/* YOUR CODE HERE */
	//draw segment
	float m[4][4] = { {-1.0f, 3.0f, -3.0f, 1.0f},
					  {3.0f, -6.0f, 3.0f, 0.0f},
					  {-3.0f, 0.0f, 3.0f, 0.0f},
					  {1.0f, 4.0f, 1.0f, 0.0f} };
	Px = (1 / 6.0f)*((float)((p1->x) + 4 * (p2->x) + (p3->x)));
	Py = (1 / 6.0f)*((float)((p1->y) + 4 * (p2->y) + (p3->y)));
	for (int i = 0; i < levelOfDetail + 1; i++) {
		float u = (float)i / levelOfDetail;
		float cons = (float) 1.0f / 6.0f;
		float vec[4] = { pow(u,3), pow(u,2), u, 1.0f };
		x = 0;
		y = 0;
		for (int j = 0; j < 4; j++) {
			x += vec[j] * cons * ((p1->x) * m[j][0] + (p2->x) * m[j][1] + (p3->x) * m[j][2] + (p4->x) * m[j][3]);
			y += vec[j] * cons * ((p1->y) * m[j][0] + (p2->y) * m[j][1] + (p3->y) * m[j][2] + (p4->y) * m[j][3]);
		}
		drawLine(Px, Py, x, y);
		Px = x;
		Py = y;
	}
	//then create a Point to be drawn where the knot should be

	Point p(x, y);
	p.draw();
}

#include "Bezier2.h"

//This function is provided to aid you.
//It should be used in the spirit of recursion, though you may choose not to.
//This function takes an empty vector of points, accum
//It also takes a set of control points, pts, and fills accum with
//the control points that correspond to the next level of detail.
void accumulateNextLevel(Pvector* accum, Pvector pts) {
	if (pts.empty()) return; 
	accum->push_back(*(pts.begin()));
	if (pts.size() == 1) return;
	for (Pvector::iterator it = pts.begin(); it != pts.end() - 1; it++) {
		/* YOUR CODE HERE  (only one to three lines)*/
		it->x = ((it->x + (it + 1)->x) / 2);
		it->y = ((it->y + (it + 1)->y) / 2);
	}
	//save the last point
	Point last = *(pts.end()-1);
	pts.pop_back();
	//recursive call
	accumulateNextLevel(accum, pts);
	accum->push_back(last);
}


// The basic draw function for Bezier2.  Note that as opposed to Bezier, 
// this draws the curve by recursive subdivision.  So, levelofdetail 
// corresponds to how many times to recurse.  
void Bezier2::draw(int levelOfDetail) {
	//This is just a trick to find out if this is the top level call
	//All recursive calls will be given a negative integer, to be flipped here
	if (levelOfDetail > 0) {
		connectTheDots();
	} else {
		levelOfDetail = -levelOfDetail;
	}

	//Base case.  No more recursive calls.
	if (levelOfDetail <= 1) {
		if (points.size() >= 2) {
			for (Pvector::iterator it = points.begin(); it != points.end() - 1; it++) {

				/* YOUR CODE HERE */
				drawLine(it->x, it->y, (it + 1)->x, (it + 1)->y);
				
			}
		}
	} else {
		Pvector* accum = new Pvector();
		Bezier2 left, right;

		//add the correct points to 'left' and 'right'.
		//You may or may not use accum as you see fit.
		/* YOUR CODE HERE */
		accumulateNextLevel(accum, points);
		Pvector::iterator it = (*accum).begin();
		int size = (*accum).size();
		for (int i = 1; i <= size; i++) {
			int half = size / 2;
			if ((i <= half) || (((size % 2) == 1) && (i == (half + 1)))) {
				left.points.push_back(*it);
			}
			if (i > half) {
				right.points.push_back(*it);
			}
			it++;
		}
		left.draw(1-levelOfDetail);
		right.draw(1-levelOfDetail);
		delete accum;
	}
}
