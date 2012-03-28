//  
// Flight Simulator
// Alex Beach 
//
#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "vector3D.h"
#include "Quaternion.h"

using namespace std;

#define GRASSDIM 256        //square dimenstion of the two textures
#define RUNWAYDIM 512
#define grass "grass2.ppm"  //file names for the two textures
#define runway "830.ppm"
#define rotationRate 0.04

GLuint grassTexture;        //texture object for the grass
GLuint roadTexture;         //texture object for the grass
int nFPS = 60;		        //Frame rate for animation
							//The how much the angle is updated 

double lefFlapAngle = 0;

double angleUp = rotationRate * -1;;   //angles for the rotations
double angleDown = rotationRate;
double angle2up = rotationRate * -1;
double angle2down = rotationRate;

bool drawParticles = false;

Vector3D normal = Vector3D(0,1,0);      //inital vectors of the plane, defines coordinage space for plane
Vector3D forwardVector = Vector3D(0,0,1);
Vector3D barrel = Vector3D(-1,0,0);
Quaternion transform;                   //transofmration quaternion
Quaternion axis1;		                //forwardVector axis quaterion
Quaternion axis2;		                //horizaontal axis quaternion

double leftFlapAngle = 0;               //angles of the flaps
double rightFlapAngle = 0;
double speed = .2;			            //initiazl speed factor of the airplane

double cameraHeight = 0.57;             //default camera height
double cameraReferenceHeight = 0.93;    //desult reference height
double cameraforwardVectorFactor = 10.0;	    //defulat forwardVector position of the camera

double planePos[3] = {1.0, 10.0, 1.0};  //starting position of the plane
double particlePos[3] = {1.0, 10.0, 1.0};
Vector3D particleVel = Vector3D(0,0,0);

int counter = 3;



//Loads a texure file giving the file name
//only excepts ppm files
void loadTextureFile(const char * filename)
{		
	FILE* pFile;
	pFile= fopen(filename, "rb"); 
	//If there is an error print error and return
	if (!pFile) 
	{ 
		printf("Fail Open File :%s\n",filename); 
		return; 
	} 
	double w,h,d;  //w is the widht, h is the height
	//There is no information about the size of the pixture
	//because ppm files do no have this informatoin in the header

	if(strcmp(filename, runway) == 0)//road texture
	{	//textures have to be squares
		w=RUNWAYDIM; 
		h=RUNWAYDIM;
	}
	else
	{		
		w = GRASSDIM;
		h = GRASSDIM;
	}	 
	//byte data for the pixel values
	//each pixel is reresnted as 3 bytes, one for R,g, and b
	unsigned char* imagedata; //char array that stores pixel data 
	imagedata=(unsigned char*)malloc(sizeof(unsigned char)*w*h*4);   
	fread(imagedata,sizeof(unsigned char),h*w*4,pFile); //readys from the data file
	//Sets the wrap parameter for texture coordinate T GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR_MIPMAP_LINEAR);
	////Sets the wrap parameter for texture coordinate T GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR_MIPMAP_LINEAR);
	//whenever a surface is rendered with larger dimensions than its corresponding texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//whenever a surface is rendered with smaller dimensions than its corresponding texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//
	gluBuild2DMipmaps( GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, imagedata );
	delete [] imagedata; 
	fclose(pFile);  

}
//Creates Glunit objects that store the texture information
//There is on for each type of texture, i.e. one for grass, the other
//for the road texture
void bindTextures( const char * filename1, const char * filename2)
{

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //sets the format of the pixel data
	glGenTextures(1, &grassTexture); //binds texure
	glBindTexture(GL_TEXTURE_2D, grassTexture); //binds texture
	loadTextureFile(filename1);//loads file
	glGenTextures(1, &roadTexture); //binds texture
	glBindTexture(GL_TEXTURE_2D, roadTexture); //loads file
	loadTextureFile(filename2);
	printf("textures have been bound\n");

}

//Sets some openGL parameters, initalizes textures
void init(void)
{
	transform.fromAxis(forwardVector,0);	//initalize the transformation quaternion
	glClearColor(0.53,0.81,0.98,1.0); // clear color is gray
	glEnable (GL_DEPTH_TEST);    //specifies what is written to the depth buffer 
	glShadeModel(GL_SMOOTH);     //sets shading model, which is how colors are iterpolated across polygons
	bindTextures(grass, runway); //opens the texture files
	printf("Init. complete\n");
}


void updateCamera()
{
	glLoadIdentity ();     //loads identiy to reset an transformations on the martrix stack	
	Vector3D move = Vector3D(0,0,1);  //gets the base direction of the plane
	move = transform * move;		//applies tranformation to diretion vector
	move.normalize();				//normalize the vector
	
	Vector3D dir;				//Get the reverise of the direction vector
	dir.x = -1 * move.x;
	dir.y = -1 * move.y;
	dir.z = -1 * move.z;

	dir.x *= cameraforwardVectorFactor;					//scale the vector to set forwardVector, backward camera position
	dir.y *= cameraforwardVectorFactor;
	dir.z *= cameraforwardVectorFactor;

	normal = Vector3D(0,1,0);	//get the normal vector of the airplane
	normal = transform * normal;
	normal.normalize();

	double aty = normal.y * cameraHeight;	//set camera hiehgt
	double looky = normal.y * cameraReferenceHeight;	//set height of camera reference point
	gluLookAt(planePos[0] + dir.x , planePos[1] + dir.y + aty,  planePos[2] + dir.z , 
		planePos[0]+ move.x, planePos[1] + move.y + looky, planePos[2] + move.z ,
	    normal.x,normal.y,normal.z);
}


void displayTerrain()
{
	double terrainSize = 400.0;
	double textureSize = 32.0;
	glColor3f(1.0,1.0,1.0);
	glPolygonMode(GL_BACK, GL_FILL);	
	glEnable (GL_TEXTURE_2D); // enable texture mapping 
	glBindTexture (GL_TEXTURE_2D, grassTexture); //bind grass texture
	glPushMatrix ();
	glRotatef(300,0.f,1.f,0.f);  //roates the terain
	glBegin( GL_QUADS );   //being drawing verticies for quadrolaterals
	glTexCoord2d(0.0,0.0); glVertex3d(-terrainSize,1.0,-terrainSize);   //draws the polygon for the grasss
	glTexCoord2d(textureSize,0.0);   glVertex3d(terrainSize,1.0,-terrainSize);	  //sets the texel coordinates 
	glTexCoord2d(textureSize,textureSize); glVertex3d(terrainSize,1.0,terrainSize);
	glTexCoord2d(0.0,textureSize); glVertex3d(-terrainSize,1.0,terrainSize);
	glEnd();  //end draw
	glPopMatrix ();
	glDisable (GL_TEXTURE_2D); // disable texture mapping

}
//Draws the body of the airplane
void drawBody()
{
	//Begin drawing plygon vertex by vertex
	glBegin (GL_POLYGON);
	glEdgeFlag (0);//If mode is set to wireframe, them this edge is not draw
	glVertex3d(2,0,-1);
	glEdgeFlag (1);//if wire frame, this edage is now supposed to be drawn
	glVertex3d(12,0,-1);
	glVertex3d(12,0,0);
	glVertex3d(3,0,6);
	glVertex3d(0,0,14);
	glVertex3d(-3,0,6);
	glVertex3d(-12,0,0);
	glEdgeFlag (0); //make edge not visible
	glVertex3d(-12,0,-1);
	glEdgeFlag (1);//make edges visible again
	glVertex3d(-2,0,-1);
	glVertex3d(-2,0,0);
	glVertex3d(0,0,0);
	glVertex3d(2,0,0);	
	glEnd();
	glBegin (GL_POLYGON);
	glVertex3d(-2,0,-7);
	glVertex3d(2,0,-7);
	glVertex3d(2,0,0);
	glVertex3d(-2,0,0);		
	glEnd();

}

//Draws the trailing edge portion of the main wing
void drawWingEdge()
{
	glBegin (GL_POLYGON);
	glVertex3d(-5,0,-3);
	glVertex3d(5,0,-3);
	glEdgeFlag (0);//makes edges not visable
	glVertex3d(5,0,-1);
	glEdgeFlag (1);//makes edges visiable
	glVertex3d(-5,0,-1);
	glEnd();
}
//draws the tips on the wings
void drawWingTips()
{
	glBegin (GL_POLYGON);
	glVertex3d(10,0,-3);
	glVertex3d(12,0,-3);
	glEdgeFlag (0);//eges visable
	glVertex3d(12,0,-1);
	glEdgeFlag (1);//not visable
	glVertex3d(10,0,-1);	
	glEnd();//end first wingtip
	glBegin (GL_POLYGON);
	glVertex3d(-12,0,-3);
	glVertex3d(-10,0,-3);
	glEdgeFlag (0);
	glVertex3d(-10,0,-1);
	glEdgeFlag (1);
	glVertex3d(-12,0,-1);
	glEnd();
}
//method to draw read wings
void drawRearWings()
{
	glBegin (GL_POLYGON);
	glVertex3d(-2,0,-7);
	glVertex3d(-6,0,-9);
	glVertex3d(-6,0,-11);
	glVertex3d(6,0,-11);
	glVertex3d(6,0,-9);
	glVertex3d(2,0,-7);
	glEnd();
}
//draws the vertical tail
void drawTail()
{
	glBegin (GL_POLYGON);
	glVertex3d(0,0.1,-11);
	glVertex3d(0,0.1,-7);
	glVertex3d(0,5,-9);
	glVertex3d(0,5,-11);
	glEnd();
}
//draws the cockpit
void drawCockpit()
{	//every three vertexes form a triangle
	glBegin (GL_TRIANGLES);
	glVertex3d(0,0,14);
	glVertex3d(-3,0,6);
	glVertex3d(0,3,6);
	glVertex3d(0,0,14);	
	glVertex3d(0,3,6);
	glVertex3d(3,0,6);
	glVertex3d(0,3,6);	
	glVertex3d(-2,0,-3);
	glVertex3d(2,0,-3);
	glVertex3d(2,0.1,-3);	
	glVertex3d(3,0.1,6);
	glVertex3d(0,3,6);
	glVertex3d(-2,0.1,-3);	
	glVertex3d(0,3,6);
	glVertex3d(-3,0.1,6);
	glEnd();
}
//Draws the left flap, also rotates it
void drawLeftFlap()
{	//operations happen in reverse order because of matrix multiplications 
	glTranslatef (-1, 0, -1);				//third, object moved back into place
	glRotatef(leftFlapAngle,1.f,0.f,0.f);	//2nd, object rotates
	glTranslatef (1, 0, 1);    //first, object moved to origin
	glBegin (GL_POLYGON);
	glVertex3d(5,0,-3);
	glVertex3d(10,0.0,-3);
	glVertex3d(10,0,-1);
	glVertex3d(5,0,-1);
	glEnd();
}

//draws right flap and rotates right flap
void drawRightFlap()
{	//operations occur in reverse oders
	glTranslatef (-1, 0, -1);		//third op
	glRotatef(rightFlapAngle,1.f,0.f,0.f); //2nd op
	glTranslatef (1, 0, 1);  //1st op
	glBegin (GL_POLYGON);
	glVertex3d(-10,0,-3);
	glVertex3d(-5,0.0,-3);
	glVertex3d(-5,0,-1);
	glVertex3d(-10,0,-1);
	glEnd();
}
//draws the airpane it also rotates the airplane
//and builds airplane through function calls
void displayPlane(int type)
{	//if type == GL_FILL them the polygons are filled in
	//if type == GL_LINE then just a wire frame is drawn
	//if tyep == GL_LINE the the color is set to red, so that the wire frame is red
	glPolygonMode(GL_FRONT_AND_BACK , type);	
	glLineWidth(2); //incrase the size of the wireframe to make it more visable
	glPushMatrix ();	
	glMatrixMode(GL_MODELVIEW);		
	glTranslatef (planePos[0],planePos[1],planePos[2]);				//third, airplane moved	
	glScalef( .25, .25, .25);   //first, airplane scaled	 	
	glMultMatrixf((GLfloat*) transform.getMatrix());  //apply quaternion transformation=-=-=-=-=-=-=--=-=-=-=-=-=-
	if(type == GL_LINE)
		glColor3f(1.0,0.0,0.0); //set color to red
	else
		glColor3f(1.0,1.0,1.0); //set color to white
	drawBody();
	drawWingEdge();
	drawWingTips();
	drawRearWings();
	drawTail();
	drawCockpit();
	glPushMatrix ();//push matrix on stack, create heirachy so all parts can move together
	drawLeftFlap();
	glPopMatrix ();	//pop off the matrix stack so transformations of the small part dont carry to other parts
	glPushMatrix ();//done for each sub part
	drawRightFlap();
	glPopMatrix ();

	
	
	glPopMatrix ();//pop off the root matrix, which is the transformation matrix of the entire plane
}


//This function generates polygons, binds textures
//This function is called continuously to animate the rotation of the terrain
void display(void)
{		
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clears depth buffer
	glMatrixMode (GL_MODELVIEW);  //modelview mode, for drawing polygons
	updateCamera();				//update the third person camera
	displayTerrain();	
	displayPlane(GL_FILL);
	displayPlane(GL_LINE);	
	glFlush();  //flush buffers
	glutSwapBuffers();	// swap front/back framebuffer to avoid flickering 
}
//this function updates the planes position based on the direction
//and the speed factor
void updatePlanePos()
{
	//updates the plane positino based on the direction that the plane is currently facein
	Vector3D move = Vector3D(0,0,1);//get base forwardVector vector
	move = transform.operator *(move);//apply transformation
	move.normalize(); //normalize the vector

	planePos[0] = planePos[0] + move.x * speed;  //updeate the position by adding a factor
	planePos[1] = planePos[1] + move.y * speed;	//of the direction components
	planePos[2] = planePos[2] + move.z * speed;

	particlePos[0] = particlePos[0] + particleVel.x ;
	particlePos[1] = particlePos[1] + particleVel.y ;
	particlePos[2] = particlePos[2] + particleVel.z ;
	
	
}

//This timer slowly returns the flaps to their non-moving position
void timer(int v)
{
	counter--;

	//printf("%i\n", counter);
	if(counter <= 0 )
	{	
		if( rightFlapAngle > 0)
			rightFlapAngle -= 1;
		else if(rightFlapAngle < 0)
			rightFlapAngle += 1;

		
		if( leftFlapAngle > 0)
			leftFlapAngle -= 1;
		else if(leftFlapAngle < 0)
			leftFlapAngle += 1;

	}

	glutPostRedisplay(); // trigger display function by sending redraw into message queue
	updatePlanePos();	
	glutTimerFunc(100/nFPS,timer,v); // restart timer again
}
//Used for when the user adjusts the size of the window
void reshape (int w, int h)
{
	printf("Resizing\n");
	// reset viewport ( drawing screen ) size
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h; 
	// reset OpenGL projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,fAspect,1,1000);//set up a perspective projection matrix 
}

//Handles keyboard input for various movement controls, mostly for testing purposes

void handleMoveKeys(unsigned char key)
{
	switch(key)
	{
	case (int)'q':		
		planePos[0] +=.2; //Move in + x		          
		break;
	case (int)'w':
		planePos[1] +=.2;  //Move in -x
		break;
	case (int)'e':			
		planePos[2] +=.2;  //Move in +z 			
		break;
	case (int)'a':	
		planePos[0] -=.2;  //Move in -z
		break;
	case (int)'s':	
		planePos[1] -=.2;  //Move in +x 
		break;
	case (int)'d':	
		planePos[2] -=.2;  //Movie in -x
		break;
	case (int) 't':        //increase speed
		speed += .1;
		break;
	case (int)'g':         //decrease speed
		speed -= .1;
	default:
		break;
	}	
}
//This function handles the flight controls
void handleFlightControls(unsigned char key)
{
	if(key == (int)'x') //barrel roll right
	{
		counter = 3;
		forwardVector = Vector3D(0,0,1);  //load base forwardVector vector
		barrel = Vector3D(-1,0,0);  //load baase horizaontal vector
		axis1.fromAxis(forwardVector, angleDown);	 //set quaternion about forwardVector vector, set angle
		barrel = axis1.operator *(barrel);   //apply quaternion to the barrel vector
		axis1.normalise();					//normalize the quaternions
		transform.normalise();
		transform = transform * axis1;      //update the transform quaternion
		
		if(rightFlapAngle < 30)
			rightFlapAngle +=1;			
		if(leftFlapAngle > -30)
			leftFlapAngle -=1;
	}
	if(key == (int)'z') //barrel roll left
	{
		counter = 3;
		forwardVector = Vector3D(0,0,1);  //load base forwardVector vector
		barrel = Vector3D(-1,0,0);  //load base horiazontal vector
		axis1.fromAxis(forwardVector, angleUp);		 //set quaternion about the forwardVector axis
		barrel = axis1.operator *(barrel);		//transform barrel 
		axis1.normalise();			//normalize the quaternions
		transform.normalise();
		transform = transform * axis1;//update the transform quaternion

		if(leftFlapAngle < 30)
			leftFlapAngle +=1;			
		if(rightFlapAngle > -30)
			rightFlapAngle -=1;
	}
	if(key == (int)'f')  //push plane down
	{
		counter = 3;

		forwardVector = Vector3D(0,0,1);   //set base vectors
		barrel = Vector3D(-1,0,0);
		axis2.fromAxis(barrel, angle2up);	//set rotation axis, angle
		axis2.normalise();					//normalize the quaternions
		transform.normalise();				
		transform =  transform * axis2;		//update the tranformation quaternion


		if(rightFlapAngle < 30)
			rightFlapAngle +=1;			
		if(leftFlapAngle < 30)
			leftFlapAngle +=1;
	}
	if(key == (int)'v') //pull plane up
	{

		counter = 3;

		forwardVector = Vector3D(0,0,1);		//set base vectors
		barrel = Vector3D(-1,0,0);
		axis2.fromAxis(barrel, angle2down);	//set rotation axis, angle
		axis2.normalise();					//normalize the quaternions
		transform.normalise();
		transform =  transform * axis2;	//upate the transform axis

		if(rightFlapAngle < 30 && rightFlapAngle > -30)
			rightFlapAngle -=1;			
		if(leftFlapAngle < 30 && leftFlapAngle > -30)
			leftFlapAngle -=1;
	}	
}
//handles camera keyboard controls
void cameraControls(unsigned char key)
{
	switch (key)
	{
		case (int)'u':   //increase camera y reference point
			cameraReferenceHeight +=.01;
			break;
		case (int)'j':
			cameraReferenceHeight -=.01;	//decrease camera y reference point
			break;
		case(int)'o':
			cameraforwardVectorFactor +=.2;		//move camera forwardVector
			break;
		case (int)'l':
			cameraforwardVectorFactor -=.2;		//move camera backward
			break;
		case (int)'i':
			cameraHeight +=.01;             //move camera up
			break;
		case (int)'k':
			cameraHeight -=.01;             //move camera down
			break;
		default:
			break;
	}

}
//this function handles when the use presses a key
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) 
	{
		// ESC hit, so quit
		printf("Exiting Program\n");
		exit(0);
	}
	handleMoveKeys(key);
	handleFlightControls(key);
	cameraControls(key);

	if(key == (int)'b')        // reset key
	{
		planePos[0] = 0;       // reset plane position
		planePos[1] = 60;
		planePos[2] = -400;
		Vector3D base1 = Vector3D(0,0,1);
		Quaternion Temp1;	  //reset the transformation quaternion
		Temp1.fromAxis(base1,0);
		transform = Temp1;
		speed = .1;		      //reset the speed
	}

	if(key == (int)'n')
	{
		printf("in n\n");
		particlePos[0] = planePos[0];
		particlePos[1] = planePos[1];
		particlePos[2] = planePos[2];

		Vector3D base1 = Vector3D(0,0,1);
		particleVel = transform * base1;

	}

}


int main(int argc, char* argv[])
{

	glutInit(&argc, (char**)argv);
	// set up for double-buffering & RGB color buffer & depth test
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ((const char*)"FlightSim");
	init(); // initialize settins	
	glutTimerFunc(100,timer,nFPS); // a periodic timer
	glutDisplayFunc(display);  // called when drawing 
	glutReshapeFunc(reshape);  // called when change window size
	glutKeyboardFunc(keyboard); // called when received keyboard interaction
	glutMainLoop(); // start the main message-callback loop


}

