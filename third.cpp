/* third.cpp
 * Programmer: Bintao Wang
 * Purpose: To draw a blueprint(kind of) using openGL. It allows you to try different trail with different data
 *			in the given file, or with the same data. It allows you to put a car on the trail to simulate its 
 * 			movement which could speed up or speed down, plus it allows you to make the graph bigger or smaller.
 *
 * Input:	will be seen in the menu (right click)   Uh, don't wanna repeat again & again. You are not gonna read
 *			the comments will you? 
 * Output:	will be seen on your screen. Well, try something more to see if there's any surprise.
 */
#include <time.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <GL/glut.h>
using namespace std;
const int windowHeight = 1500;							// The X-size of the window
const int windowWidth = 1200;							// The Y-size of the window
string filename = "g3.in";						   	   	// Name of the input file
long  NUM_TICKS = 15000;								// time waiting for next frame
float z = 2.0; 								 			// distance of eye (center of projection) from frustum
float x_value[10];										// array for the x input
float y_value[10];										// array for the y input
float x_position;										// x coordicate of the car's top
float y_position;										// x coordicate of the car's top
int num_point;											// number of points in the trail
int choice = 0;											// choice to choose which spline to display (0 for linear, 1 for quadra, 2 for cubic)
float lb[10];											// slope in linear spline
float qa[10], qb[10], qc[10];							// a & b & c using quadratic spline
float tempo;											// tempo of dot forming up a line
int section = 0;										// sections between points on the trail
float freefall = 0;										// freefall velocity when it first release
int stillRun = 0;										// check if the car is still running
int lineSkip = 0;										// the number of line in that file that will be skipped
void mymenu (int);										// cool way showing the menu
class roller
{
public:
	void readfile ();									// function to read the file
	void drawbase ();									// function to draw the basement
	void drawSpline ();									// function to draw the linear or quadratic or cubic? spline
	void drawCar ();									// function to draw the moving car
	void Idle ();										// function to determine where should the car be drawn
};
roller mydrawing;
void display (void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity ();
    gluLookAt (0.0, 0.0, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);		// the observe point
    glScalef (1.0, 1.0, 1.0);
  	mydrawing.drawbase();
  	mydrawing.drawSpline();
  	mydrawing.Idle();
  	mydrawing.drawCar();
  	glFlush ();
}
void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glFrustum (0, 1000, 0, 1000, 1.5, 20.0);				// viewport
   glMatrixMode (GL_MODELVIEW);
}
void roller::readfile (void)
{
	int i, j;											// counters to figure out which number to read/delete next
	int lineJump = lineSkip;							// non-global variable of lineSkip
	ifstream inFp;										
	inFp.open(filename);								// opening file
	if (inFp.is_open())
	{
		cout << "File called: '" << filename << "' founded." << endl;		
		while(lineJump >= 0){							// read line
			inFp >> num_point;

			for(i = 0; i < num_point; i++){				// read numbers
				inFp >> x_value[i];
				inFp >> y_value[i];				
			}
			if(lineJump > 0 && num_point != '\0'){		// delete numbers if needed
				for(j = 0; j < num_point; j++){
					x_value[j] = '\0';
					y_value[j] = '\0';
					num_point = '\0';
				}
			}
			if (i > num_point){							// check if there is more points than needed
				for(j = 0; j < num_point; j++){			// if so delete all
					x_value[j] = '\0';
					y_value[j] = '\0';
					num_point = '\0';
				}
				// return;
			}
			else if (i < num_point){					// check if there is less points than needed
				for(j = 0; j < num_point; j++){			// if so delete all
					x_value[j] = '\0';
					y_value[j] = '\0';
					num_point = '\0';
				}
				// return;
			}
			else if(num_point == 0){
				cout << "No more data, press 'n' again to restart from the first line." << endl;
				lineSkip = -1;
			}
			else
				cout << "The number of points are valid, good to go." << endl;
			lineJump--;
		}
	}
	else{
		cout << "There's no file called " << filename << " in this folder, please check it." << endl;
	}
	inFp.close();										// closing file
}

void roller::drawbase (void)
{
	for (int Pillar = 0; Pillar < num_point; Pillar++){
		glBegin(GL_LINE_LOOP);
			glColor3f(0.0, 0.0, 0.0);
			glVertex3f(100 + x_value[Pillar] - 25, 100, 0);
			glVertex3f(100 + x_value[Pillar] + 25, 100, 0);
			glVertex3f(100 + x_value[Pillar] + 25, 150, 0);
			glVertex3f(100 + x_value[Pillar] - 25, 150, 0);
		glEnd();
		glBegin(GL_LINES);			
			glVertex3f(100 + x_value[Pillar], 150, 0);
			glVertex3f(100 + x_value[Pillar], 150 + 500 + y_value[Pillar], 0);
		glEnd();
	}
}
void roller::drawSpline (void)
{
	float q_temp;										// quick way to write tempo - x_value[vertice]								
	if(choice == 0){
			glColor3f(0.0, 0.0, 0.0);
			for (int vertice = 0; vertice < num_point; vertice++){
				lb[vertice] = (y_value[vertice + 1] - y_value[vertice]) / (x_value[vertice + 1] - x_value[vertice]);
				for(tempo = x_value[vertice]; tempo <= x_value[vertice + 1]; tempo+= 0.05){
					glBegin(GL_POINTS);
						glVertex3f(100 + x_value[0] + tempo, 650 + y_value[vertice] + lb[vertice] * (tempo- x_value[vertice]), 0);
					glEnd();
			}
		}
	}
	else if(choice == 1){
		qa[0] = 0;
		qc[0] = 0;
		qb[0] = (y_value[1] - y_value[0]) / (x_value[1] - x_value[0]);
		if(num_point != 0){
			for(tempo = x_value[0]; tempo <= x_value[1]; tempo+= 0.05){
				glBegin(GL_POINTS);
					glVertex3f(100 + x_value[0] + tempo, 150 + 500 +  y_value[0] + tempo * qb[0], 0);
				glEnd();
			}
		}
		for(int vertice = 1; vertice < num_point; vertice++){
			qc[vertice] = y_value[vertice];
			qb[vertice] = qa[vertice-1] * x_value[vertice] + qb[vertice-1];
			qa[vertice] = (y_value[vertice + 1] - qb[vertice] * (x_value[vertice + 1] - x_value[vertice]) - qc[vertice]) / pow (x_value[vertice + 1] - x_value[vertice], 2);
			for(tempo = x_value[vertice]; tempo <= x_value[vertice + 1]; tempo+= 0.05){
				q_temp = tempo - x_value[vertice];
				glBegin(GL_POINTS);
					glVertex3f(100 + x_value[0] + tempo, 650 +  qc[vertice] + qa[vertice] * q_temp * q_temp + q_temp * qb[vertice], 0);
				glEnd();
			}
		}		
	}
}
void roller::Idle (void)
{
	clock_t endWait;  /* end of wait interval */
	if(stillRun == 1 || stillRun == 2){
		if(choice == 0){
			if(x_value[section] <= x_position && x_position < x_value[section + 1]){
				x_position += 0.1;
				y_position =  y_value[section] + lb[section] * (x_position- x_value[section]);
			}
			else if (x_position < 1000)
				section++;
		}
		else if(choice == 1){
			if(x_value[section] <= x_position && x_position < x_value[section + 1]){
				x_position += 0.1;
				y_position =  y_value[section] + qa[section] * pow ((x_position- x_value[section]), 2) + (x_position- x_value[section]) * qb[section];
			}
			else if (x_position < 1000)
				section++;
		}
		else{
			if(y_position >= -550){
				x_position += 0.1;
				y_position -= freefall;
				freefall += 0.003;
			}
			else{
				cout << "Accident, cubic is not built yet!" << endl;
				cout << "Press 'r' to reset." << endl;
				section = 0;
				freefall = 0;
				stillRun = 0;
			}
		}
	}
	endWait = clock () + NUM_TICKS;
	while (clock () < endWait);
glutPostRedisplay();   
}
void roller::drawCar (void)
{
	if(num_point != 0){
		glBegin(GL_LINES);
			glVertex3f(x_position + 100, y_position + 650, 0);
			glVertex3f(x_position + 100, y_position + 575, 0);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(x_position + 115, y_position + 575, 0);
			glVertex3f(x_position + 95, y_position + 575, 0);
			glVertex3f(x_position + 95, y_position + 600, 0);
			glVertex3f(x_position + 85, y_position + 600, 0);
			glVertex3f(x_position + 85, y_position + 550, 0);
			glVertex3f(x_position + 115, y_position + 550, 0);
		glEnd();
	}
}
void init ()
{
   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
   glClearColor (1.0, 1.0, 1.0, 0.0);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0.0, windowWidth, 0.0, windowHeight);
}

void Key( unsigned char key, int x, int y )
{
	switch (key) {
		case '0':	choice = 0;
					cout << "Start drawing linear." << endl;
			display ();
			break;
		case '1':	choice = 1;
					cout << "Start drawing quadratic." << endl;
			display ();
			break;
		case '2':	choice = 2;
					cout << "Start drawing cubic." << endl;
					cout << "not prepared yet." << endl;
			display ();
			break;
		case '4':	if(NUM_TICKS < 90000){
						NUM_TICKS = NUM_TICKS * 1.2;
						cout << "speed down." << endl;
					}
					else
						cout << "MIN speed." << endl;
			break;
		case '6':	if(NUM_TICKS > 2500){
						NUM_TICKS = NUM_TICKS / 1.2;
						cout << "speed up." << endl;
					}
					else
						cout << "MAX speed." << endl;
			break;
		case '+':	if(z > 2){
						z--;
						cout << "Becoming larger." << endl;}
					else
						cout << "Touching limit, can't be larger." << endl;
			break;
		case '-':	if(z < 20){
						z++;
						cout << "Becoming smaller." << endl;}
					else
						cout << "Touching limit, can't be smaller." << endl;
			break;
		case 'n':	lineSkip++;
					x_position = 0;
					y_position = y_value[0];
					stillRun = 0;
			mydrawing.readfile();
			break; 
		case 'r':	x_position = 0;
					y_position = y_value[0];
					stillRun = 0;
					cout << "Reset successfully." << endl;
			break;
		case 's':	if(stillRun == 1){
						stillRun = 2;
						cout << "Already started, press 's' again to pause." << endl;
					}
					else if(stillRun == 2){
						stillRun = 3;
						cout << "Pause." << endl;
					}
					else if(stillRun == 3){
						stillRun = 2;
						cout << "Resume." << endl;
					}
					else if(x_position != 1000 && y_position > -550){
						stillRun = 1;
						cout << "Start moving." << endl;
					}
					else
						cout << "You need to reset first." << endl;
			break;
		case 'q':	cout << "Closing." << endl;
					exit(0);
			break;
	}
}
void initmenu ()						// set up menu
{
   glutCreateMenu (mymenu);     		// register callback function
   glutAddMenuEntry ("Menu", 0);
   glutAddMenuEntry ("", 0);
   glutAddMenuEntry ("Press '0' to present linear", 1);
   glutAddMenuEntry ("Press '1' to present quadratic", 2);
   glutAddMenuEntry ("Press '2' to present cubic", 1);
   glutAddMenuEntry ("Press '4' to let the car go slower", 2);
   glutAddMenuEntry ("Press '6' to let the car go faster", 1);
   glutAddMenuEntry ("Press '+' to zoom in", 2);
   glutAddMenuEntry ("Press '-' to zoom out", 1);
   glutAddMenuEntry ("Press 'n' to display next trail", 2);
   glutAddMenuEntry ("Press 'r' to reposition the car", 1);
   glutAddMenuEntry ("Press 's' to start/stop the car", 2);
   glutAddMenuEntry ("Press 'q' to quit", 3);
   glutAttachMenu (GLUT_RIGHT_BUTTON);
}
void mymenu (int value)					// Define actions for the menu based on values given in initmenu()
{
   switch (value) {	
	  case 1: 	cout << "Don't wanna repeat that again in the console." << endl;
			  	break;
	  case 2:	cout << "Well, I want to have an option that allows you using mouse click input, but I couldn't." <<endl;
	  			break;
	  case 3:	cout << "Well, this one does works, goodbye!" << endl;
	  			exit (1);
   }
}
int main (int argc, char** argv)
{
   // instruction();
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   /* set up the window size, in pixels */
   glutInitWindowSize (1500, 1200); 
   /* put the window at 100, 100 on user's screen */
   glutInitWindowPosition (100, 100);
   /* make a title for window */
   glutCreateWindow ("Four Flag Roller Coaster Blue Print");
   cout << "Hint: Right click to see all of the options" << endl;
   /* set up some GL stuff; use class method */
   mydrawing.readfile();
   init ();
   initmenu ();
   /* call display function, which in turn will call class method */
   glutDisplayFunc(display); 
   //GlutPostRedisplay();
   /* can do the same trick with the keyboard function as above */
   glutReshapeFunc (reshape);
   glutKeyboardFunc (Key);
   /* repeat these things indefinitely */
   glutMainLoop();
   /* we're done! */
   return 0; 
}