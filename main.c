#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "image.h"

/* Names of the texture files */
#define FILENAME0 "tile.bmp"
#define FILENAME1 "wall.bmp"

//Text variable
unsigned int g_bitmap_text_handle = 0;

/* texture identificators */
static GLuint names[2];

/*callback functions*/
static void on_display(void);
static void releaseKey(unsigned char key, int x, int y);
static void pressKey(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
static void on_timer(int value);

static char *maze;
static float deltaX = 0.0f;
static float deltaZ = 0.0f;
static int **matrixMaze;

static int width, height;
static int scr_width, scr_height;
static float animation_parameter = 0;
static int animation_active = 0;

int win = 0;

/* Variables for positioning */
static float poz_X = 0, poz_Z = 0;

/* value of the rotation angle */
static float spin = 0;

/* variables for changing the distance of the camera*/
static int pom_view = 10;
static int cam_ind = 0;

//function that creates and renders bitmap characters
unsigned int make_bitmap_text()
{
	unsigned int handle_base = glGenLists(256);

	for (size_t i=0;i<256;i++)
	{
		// a new list for each character
		glNewList(handle_base+i, GL_COMPILE);
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, i);
		glEndList();
	}
	return handle_base;
}

//drawing bitmap characters
void draw_text(const char* text)
{	
	int n = strlen(text);

	glListBase(g_bitmap_text_handle);
	glCallLists(n, GL_UNSIGNED_BYTE, text);
}

void mouseMove(int x, int y)
{

	if(!cam_ind){
	spin =(x - poz_X);
	if (spin>0){
		spin = spin + 4;
	} else if (spin<0)
		spin = spin - 4;
	else
		spin = 0;
	}

	glutPostRedisplay();
}

/*  Function that accepts array that has been generated with DFS algorithm and converts it to matrix which will be used for maze representation */
void mazeToMatrix(char *maze, int **matrix){
	  for(int i = 1; i <= width*height; i++){
	  	if(maze[i-1] == 1){
	  		matrix[(i-1)/width][(i-1)%width] = 0;
	  	}
	  	else
	  		matrix[(i-1)/width][(i-1)%width] = 2;
   }
}

/* Checking for any collision with walls */
int checkCollision()
{	

	float camWorldX = (poz_X+deltaX)/2;
	float camWorldZ = (poz_Z + deltaZ)/2;

	int truncX =(int)camWorldX;
	int truncZ = (int)(camWorldZ);


	int roundedZ = round(camWorldZ);
	int roundedX = round(camWorldX);

	if (poz_X >= (width -1) *2 - 2.3  && poz_Z >= (width -1) *2 - 2.3 ) {
		win = 1;
		return 1;
	}

	if(poz_X < 0 || poz_Z < 0){
		return 1;
	}

	if (matrixMaze[roundedX][roundedZ] == 0) {
		if (camWorldX > truncX+0.5 || truncZ > 1) {
			return 1;
			}
		else return 0;
	} else
	 return 0;
}
	/* This function generates random maze with DFS algorithm and represents it as an array that will be later converted to matrix.
	*************************************************
	Copied from https://en.wikipedia.org/wiki/Maze_generation_algorithm and slightly modified
	**********************************************
	*/
void CarveMaze(char *maze, int width, int height, int x, int y) {

   int x1, y1;
   int x2, y2;
   int dx, dy;
   int dir, count;

   dir = rand() % 4;
   count = 0;
   while(count < 4) {
      dx = 0; dy = 0;
      switch(dir) {
      case 0:  dx = 1;  break;
      case 1:  dy = 1;  break;
      case 2:  dx = -1; break;
      default: dy = -1; break;
      }
      x1 = x + dx;
      y1 = y + dy;
      x2 = x1 + dx;
      y2 = y1 + dy;
      if(   x2 > 0 && x2 < width && y2 > 0 && y2 < height
         && maze[y1 * width + x1] == 1 && maze[y2 * width + x2] == 1) {
         maze[y1 * width + x1] = 0;
         maze[y2 * width + x2] = 0;
         x = x2; y = y2;
         dir = rand() % 4;
         count = 0;
      } else {
         dir = (dir + 1) % 4;
         count += 1;
      }
   }

}





/* https://en.wikipedia.org/wiki/Maze_generation_algorithm */
void GenerateMaze(char *maze, int width, int height) {

   int x, y;

   /* Initialize the maze. */
   for(x = 0; x < width * height; x++) {
      maze[x] = 1;
   }
   maze[1 * width + 1] = 0;

   /* Seed the random number generator. */
   srand(time(0));

   /* Carve the maze. */
   for(y = 1; y < height; y += 2) {
      for(x = 1; x < width; x += 2) {
         CarveMaze(maze, width, height, x, y);
      }
   }

   /* Set up the entry and exit. */
   maze[0 * width + 1] = 0;
   maze[0] = 0;

   maze[(height - 1) * width + (width - 2)] = 0;

}



/* Calculate the new position for the player */
void computePos(float deltaX, float deltaZ){

	float coef = (M_PI/180.0);

	int oldx = poz_X;
	int oldz = poz_Z;

	if(deltaX > 0){
		poz_X += deltaX * (cos(spin * coef) - sin(spin * coef));
		poz_Z += deltaZ * (cos(spin * coef) + sin(spin * coef));
	}else if(deltaX < 0){
		poz_X += deltaX * (cos(spin * coef) - sin(spin * coef));
		poz_Z += deltaZ * (cos(spin * coef) + sin(spin * coef));
	}

	if(checkCollision() == 1){
		poz_X = oldx;
		poz_Z = oldz;
	}
}



int main(int argc, char *argv[])
{


   if(argc != 2){
		printf("Error! Correct launch: ./Labyrinth [size] \n");
		exit(EXIT_FAILURE);
		}


   width = atoi(argv[1]) * 2 + 1;
   height = width;
	
	
   /* Allocating the memory for the maze*/
   maze = (char*)malloc(width * height * sizeof(char));
   if(maze == NULL) {
      printf("error: not enough memory\n");
      exit(EXIT_FAILURE);
   }

   /* Generate the maze. */
   GenerateMaze(maze, width, height);

  	matrixMaze = (int**)malloc(width * sizeof(int*));
	for (int index=0; index<width; ++index)
	{
   		matrixMaze[index] = (int*)malloc(width * sizeof(int));
	}

	mazeToMatrix(maze, matrixMaze);


	/*Initialization of the basic funtionalities*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/*Initialization of the window*/
	scr_height = glutGet(GLUT_SCREEN_HEIGHT);
	scr_width = glutGet(GLUT_SCREEN_WIDTH);
	glutInitWindowSize(scr_width, scr_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	

	/*Registering the event handling functions*/
	glutKeyboardFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseKey);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutPassiveMotionFunc(mouseMove);
	glutSetCursor(GLUT_CURSOR_NONE);
	
	/* OpenGL initialization */
	glEnable(GL_DEPTH_TEST);

	glClearColor(0, 0, 0, 0);
	glEnable(GL_TEXTURE_2D);

	Image * image;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  	

    /*
     * Initializing object that will contain texture images from the file
     */

    image = image_init(0, 0);

    /* Creating the first texture. */
    image_read(image, FILENAME0);

    /* Generating texture identificators */
    glGenTextures(2, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Creating the second texture */
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Turning off the active texture */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Destroying the file for reading the file textures. */
    image_done(image);

	g_bitmap_text_handle = make_bitmap_text();
	/* entering the main loop */
	glutMainLoop();
	



	return 0;
}



static void on_reshape(int width, int height)
{

	/* Setting the viewport*/
	glViewport(0,0, width, height);

	/* Editing the projection parameters */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55, ((float)scr_width+5)/scr_height, 3.6, 100);
}

static void pressKey(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: 
				glDeleteTextures(2, names);
				exit(0);
				break;
		case 'b':
		case 'B': 
				cam_ind = 1;
				pom_view = -20;
				glutPostRedisplay();
				break;
				
				break;
		case 'w': 
		case 'W':
				//if bird's eye view is off and we didnt reach the end of the maze, then we can move
				if(!cam_ind && win == 0){
					if (!animation_active) {
           			 glutTimerFunc(10, on_timer, 0);
            		animation_active = 1;
       				 }
				deltaX = 0.07f;
				deltaZ = 0.07f;
				}
				
			break;
			
		case 's':
		case 'S':
				if(!cam_ind && win == 0){
					if (!animation_active) {
           			 glutTimerFunc(10, on_timer, 0);
            		animation_active = 1;
       				 }
				deltaX = -0.07f;
				deltaZ = -0.07f;
				
				}
			break;
			
		case 'c':
		case 'C':
			{	
				//Change the camera distance to close
				cam_ind = 0;
				pom_view = 10;
				glutPostRedisplay();
				break;
			}
		case 'f':
		case 'F':
			{
				//Change the camera distance to far
				cam_ind = 0;
				pom_view = 7.5;
				glutPostRedisplay();
				break;
			}
		case 'r':
		case 'R':
			{
				//Recreate the maze
				GenerateMaze(maze, width, height);
				mazeToMatrix(maze, matrixMaze);
				glutPostRedisplay();
				break;
			}
		case 'p':
		case 'P':
			{
				//move to starting position
				win = 0;
				poz_X = 0.5;
				poz_Z = 0.5;
				glutPostRedisplay();
				break;
			}

	}
}

static void releaseKey(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: 
				glDeleteTextures(2, names);
				exit(0);
				break;

		case 'w':
		case 'W':
				if(!cam_ind){
				animation_active = 0;
				deltaX = 0;
				deltaZ = 0;
				}
			break;
			
		case 's':
		case 'S':
				if(!cam_ind){
				animation_active = 0;
				deltaX = 0;
				deltaZ = 0;
				}
			break;
	}
}

//name of this function is selfexplanatory
void drawFloor(GLfloat x1, GLfloat x2, GLfloat z1, GLfloat z2)
{
	glBindTexture(GL_TEXTURE_2D, names[0]);
	glBegin(GL_POLYGON);
		glNormal3f( 0.0, 1.0, 0.0);
		glColor4f(1, 1, 1, 1);

		glTexCoord2f(0,0);
		glVertex3f( x1, -1, z2 );
		glTexCoord2f(1,0);
		glVertex3f( x2, -1, z2 );
		glTexCoord2f(1,1);
		glVertex3f( x2, -1, z1 );
		glTexCoord2f(0,1);
		glVertex3f( x1, -1, z1 );
    	glEnd();
}


//yep
void drawWall(float x1, float x2, float y1, float y2, float z1, float z2){
	glBindTexture(GL_TEXTURE_2D, names[1]);
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);

	//Draw the right side
	glNormal3f(0, 0, 1);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z1);
    glTexCoord2f(1,0);
	glVertex3f(x2, y1, z1);
	glTexCoord2f(1,1);
	glVertex3f(x2, y2, z1);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z1);


	//left side
	glNormal3f(0, 0, -1);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z2);
	glTexCoord2f(1,0);
	glVertex3f(x2, y1, z2);
	glTexCoord2f(1,1);
	glVertex3f(x2, y2, z2);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z2);


	//Top
	glNormal3f (0,1, 0);
	glTexCoord2f(0,0);
	glVertex3f(x1, -y2, z1);
	glTexCoord2f(1,0);
	glVertex3f(x2, -y2, z1);
	glTexCoord2f(1,1);
	glVertex3f(x2, -y2, z2);
	glTexCoord2f(0,1);
	glVertex3f(x1, -y2, z2);

	// Back side
	glNormal3f (1,0, 0);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z1);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z1);
	glTexCoord2f(1,1);
	glVertex3f(x1, y2, z2);
	glTexCoord2f(1,0);
	glVertex3f(x1, y1, z2);


	//Front side
	glNormal3f (-1,0, 0);
	glTexCoord2f(0,0);
	glVertex3f(x2, y1, z1);
	glTexCoord2f(0,1);
	glVertex3f(x2, y2, z1);
	glTexCoord2f(1,1);
	glVertex3f(x2, y2, z2);
	glTexCoord2f(1,0);
	glVertex3f(x2, y1, z2);

	glEnd();
}


static void on_timer(int value)
{
    /* Check if the callback is coming from the correct timer. */
    if (value != 0)
        return;

    /* Adjusting the simulation time. */
    animation_parameter++;

    /* Force window redisplay. */
    glutPostRedisplay();

    /* If needed, set the timer again. */
    if (animation_active)
        glutTimerFunc(10, on_timer, 0);
}


static void on_display()
{

	//if 'w' or 's' is pressed, compute the position
	if(deltaX || deltaZ){
		computePos(deltaX, deltaZ);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Setting up the camera*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55, ((float)scr_width+5)/scr_height, 2.8, width*3);
	//If indicator equals 1 then switch to bird's eye view, otherwise go to normal view
	if(cam_ind){
		gluLookAt(-width, width*2, -width, -width, 0, -width, 1, 0, 1);
	}
	else{
		gluLookAt(12.5-poz_X -pom_view, 11.5- pom_view, 12.5-poz_Z-pom_view, 0-poz_X, 0, 0- poz_Z, 0, 1, 0);
	}		

	if (!win) {
		//Helper text
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(0, 1, 0);
			glRasterPos2i(0, 0); // centre the text
			draw_text("You start here. Watch out for those evil walls. GL!");
		glPopMatrix();
	} else {
		//Text when goal is reached
		glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(-(poz_X + 1) , 1,- (poz_Z + 1));
			glRasterPos2i(0, 0); // centre the text
			draw_text("YOU WIN! press P and R for restarting");
		glPopMatrix();
	}


	//This makes the player center of camera rotation
	glTranslatef(-poz_X, 0, -poz_Z);    					
	glRotatef(spin, 0, 1, 0);								
	glTranslatef(poz_X, 0, poz_Z);				

	glRotatef(180, 0, 1, 0);


	//draw the floor under the maze so it isn't a floating island
	glColor3f(0.13, 0.52, 0.69);
	glBegin(GL_QUADS);
	glVertex3f(-4*width,  -1.01, -4*width);
	glVertex3f(-4*width, -1.01, 2*width);
	glVertex3f(2*width, -1.01, 2*width);
	glVertex3f(2*width, -1.01, -4*width);
	glEnd();



	//Draw walls and the floor
	for(int i = 0; i < width; i++) {
		for(int j=0; j < height; j++) {
			if (matrixMaze[i][j] == 0) {
				glPushMatrix();
					glTranslatef(2*i, 0, j*2);
					glEnable(GL_TEXTURE_2D);
					drawWall(1, -1, 2, -2, 1, -1);
					glDisable(GL_TEXTURE_2D);
				glPopMatrix();
			} else {
				glPushMatrix();
					glTranslatef(2*i, 0, j*2);
					glEnable(GL_TEXTURE_2D);
					drawFloor(1, -1, 1, -1);
					glDisable(GL_TEXTURE_2D);
				glPopMatrix();

			}
		}
	}

	glColor3f(0, 0, 0);
	glPushMatrix();
    glTranslatef(0+poz_X, 0, 0+poz_Z);
    //Draw the player and put it on correct position
   	glRotatef(45, 0, 1, 0);
    
   //this is player's body
    glRotatef(-spin, 0, 1, 0);
    glScalef(1, 1, 0.5);
    glutSolidCube(0.5);
 

    //draw the left hand
    glPushMatrix();
    glRotatef(
            /* angle */
            sin(animation_parameter / 5.0f) * 30.0f,
            /* rotation vector */
            1,0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(0.33, 0, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();

    //right hand
    glPushMatrix();
    glRotatef(
            /* angle */
            -sin(animation_parameter / 5.0f) * 30.0f,
            /* vector of rotation */
           1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(-0.33, 0, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();

    //left leg
    glPushMatrix();
    glRotatef(
            /* angle */
            -sin(animation_parameter / 5.0f) * 30.0f,
            /* vector of rotation */
            1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(0.13, -0.5, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();


    //right leg
    glPushMatrix();
    glRotatef(
            /* angle */
            sin(animation_parameter / 5.0f) * 30.0f,
            /*vector of rotation */
            1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(-0.13, -0.5, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();

    //player's head
    glColor3f(1, 1, 1);
    glTranslatef(0, 0.5, 0);
    glutSolidSphere(0.2, 50, 50);

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

	/*Put the new picture in the window*/
	glutSwapBuffers();
}