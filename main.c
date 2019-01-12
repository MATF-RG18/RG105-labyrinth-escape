#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "image.h"

/* Imena fajlova sa teksturama. */
#define FILENAME0 "tile.bmp"
#define FILENAME1 "wall.bmp"

/* Identifikatori tekstura. */
static GLuint names[2];

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


// Vektori za kretanje 
static float poz_X = 0, poz_Z = 0;

// Ugao rotacije
static float spin = 0;

// Pomocna promenljiva za podesavanje blizine kamere
static int pom_view = 10;
static int cam_ind = 0;


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

/*  Funkcija koja prihvata niz koji je generisan dfs-om i pretvara ga 
	u matricu koju koristim za reprezentaciju lavirinta*/
void mazeToMatrix(char *maze, int **matrix){
	  for(int i = 1; i <= width*height; i++){
	  	if(maze[i-1] == 1){
	  		matrix[(i-1)/width][(i-1)%width] = 0;
	  	}
	  	else
	  		matrix[(i-1)/width][(i-1)%width] = 2;
   }
}
int checkCollision()
{
	if(poz_X < 0 || poz_Z < 0){
		return 1;
	}

	int roundedX = round(poz_X);
	int roundedZ = round(poz_Z);

	int truncX = (int)(poz_X);
	int truncZ = (int)(poz_Z);

	/*if (matrixMaze[roundedX][roundedZ] == 0)
		//if (poz_X >= truncX + 0.5 || poz_Z >= truncZ + 0.5)
			return 1;*/


	return 0;
}

/*funkcija koja generise random lavirint i predstavlja ga kao niz.
	Kasnije ga pretvaram u matricu
	*************************************************
	preuzeto sa https://en.wikipedia.org/wiki/Maze_generation_algorithm
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





/* takodje https://en.wikipedia.org/wiki/Maze_generation_algorithm */
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



/* Izracunavamo nove koordinate igraca */
void computePos(float deltaX, float deltaZ){

	float angle = (M_PI/180.0);

	int oldx = poz_X;
	int oldz = poz_Z;

	if(deltaX > 0){
		poz_X += deltaX * (cos(spin * angle) - sin(spin * angle));
		poz_Z += deltaZ * (cos(spin * angle) + sin(spin * angle));
	}else if(deltaX < 0){
		poz_X += deltaX * (cos(spin * angle) - sin(spin * angle));
		poz_Z += deltaZ * (cos(spin * angle) + sin(spin * angle));
	}

	if(checkCollision() == 1){
		poz_X = oldx;
		poz_Z = oldz;
	}
}


int main(int argc, char *argv[])
{

   if(argc != 2){
		printf("Greska! Pravilno pokretanje programa: ./Labyrinth [velicina] \n");
		exit(EXIT_FAILURE);
		}


   width = atoi(argv[1]) * 2 + 1;
   height = width;
	
	
   
   maze = (char*)malloc(width * height * sizeof(char));
   if(maze == NULL) {
      printf("error: not enough memory\n");
      exit(EXIT_FAILURE);
   }

   /* Generate and display the maze. */
   GenerateMaze(maze, width, height);

  	matrixMaze = (int**)malloc(width * sizeof(int*));
	for (int index=0; index<width; ++index)
	{
   		matrixMaze[index] = (int*)malloc(width * sizeof(int));
	}

	mazeToMatrix(maze, matrixMaze);


	/*Vrsimo inicijalizaciju osnovnih funkcionalnosti*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/*Inicijalizacija prozora*/
	scr_height = glutGet(GLUT_SCREEN_HEIGHT);
	scr_width = glutGet(GLUT_SCREEN_WIDTH);
	glutInitWindowSize(scr_width, scr_height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	

	/*Registruju se funkcije za obradu dogadjaja*/
	//glutMouseFunc(movelight);
	//glutPassiveMotionFunc(motion);
	glutKeyboardFunc(pressKey);
	glutIgnoreKeyRepeat(1);
	glutKeyboardUpFunc(releaseKey);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutPassiveMotionFunc(mouseMove);
	glutSetCursor(GLUT_CURSOR_NONE);
	/*glutWarpPointer(SizeX/2,SizeY/2);*/

	/* Obavlja se openGL inicijalizacija*/
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.75, 0.75, 0.75, 0);

	glEnable(GL_TEXTURE_2D);

	Image * image;

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  	

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(0, 0);

    /* Kreira se prva tekstura. */
    image_read(image, FILENAME0);

    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);

    glBindTexture(GL_TEXTURE_2D, names[0]);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Kreira se druga tekstura. */
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

	/* Ulazi se u glavnu petlju! ^^ */
	glutMainLoop();
	



	return 0;
}



static void on_reshape(int width, int height)
{

	/* Postavlja se viewport*/
	glViewport(0,0, width, height);

	/*Postavljaju se parametri projekcije*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55, ((float)scr_width+5)/scr_height, 3.6, 100);
	//gluLookAt(12.5-poz_X -pom_view, 12.5- pom_view, 12.5-poz_Z-pom_view, 0-poz_X, 0, 0- poz_Z, 0, 1, 0);
	//glutPostRedisplay();
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
				if(!cam_ind){
					if (!animation_active) {
           			 glutTimerFunc(10, on_timer, 0);
            		animation_active = 1;
       				 }
				deltaX = 0.2f;
				deltaZ = 0.2f;
				}
			break;
			
		case 's':
		case 'S':
				if(!cam_ind){
					if (!animation_active) {
           			 glutTimerFunc(10, on_timer, 0);
            		animation_active = 1;
       				 }
				deltaX = -0.2f;
				deltaZ = -0.2f;
				
				}
			break;
			
		case 'c':
		case 'C':
			{	
				cam_ind = 0;
				pom_view = 10;
				glutPostRedisplay();
				break;
			}
		case 'f':
		case 'F':
			{
				cam_ind = 0;
				pom_view = 7.5;
				glutPostRedisplay();
				break;
			}
		case 'r':
		case 'R':
			{
				GenerateMaze(maze, width, height);
				mazeToMatrix(maze, matrixMaze);
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


// funkcija za crtanje zidova lavirinta
void drawWall(float x1, float x2, float y1, float y2, float z1, float z2){
	glBindTexture(GL_TEXTURE_2D, names[1]);
	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);

	//Desna strana
	glNormal3f(0, 0, 1);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z1);
    glTexCoord2f(1,0);
	glVertex3f(x2, y1, z1);
	glTexCoord2f(1,1);
	glVertex3f(x2, y2, z1);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z1);


	//Leva strana
	glNormal3f(0, 0, -1);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z2);
	glTexCoord2f(1,0);
	glVertex3f(x2, y1, z2);
	glTexCoord2f(1,1);
	glVertex3f(x2, y2, z2);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z2);


	//Gornja strana
	glNormal3f (0,1, 0);
	glTexCoord2f(0,0);
	glVertex3f(x1, -y2, z1);
	glTexCoord2f(1,0);
	glVertex3f(x2, -y2, z1);
	glTexCoord2f(1,1);
	glVertex3f(x2, -y2, z2);
	glTexCoord2f(0,1);
	glVertex3f(x1, -y2, z2);

	//Zadnja strana
	glNormal3f (1,0, 0);
	glTexCoord2f(0,0);
	glVertex3f(x1, y1, z1);
	glTexCoord2f(0,1);
	glVertex3f(x1, y2, z1);
	glTexCoord2f(1,1);
	glVertex3f(x1, y2, z2);
	glTexCoord2f(1,0);
	glVertex3f(x1, y1, z2);


	//Prednja strana
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
    /* Proverava se da li callback dolazi od odgovarajuceg tajmera. */
    if (value != 0)
        return;

    /* Azurira se vreme simulacije. */
    animation_parameter++;

    /* Forsira se ponovno iscrtavanje prozora. */
    glutPostRedisplay();

    /* Po potrebi se ponovo postavlja tajmer. */
    if (animation_active)
        glutTimerFunc(10, on_timer, 0);
}


static void on_display()
{


	if(deltaX || deltaZ){
		computePos(deltaX, deltaZ);
	}
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	/* Postavlja se kamera*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(55, ((float)scr_width+5)/scr_height, 3.6, width*3);
	//u slucaju da je indikator ukljucen, prebacuje se na pticiju perspektivu
	if(cam_ind){
		gluLookAt(-width, width*2, -width, -width, 0, -width, 1, 0, 1);
	}
	else{
		gluLookAt(12.5-poz_X -pom_view, 12.5- pom_view, 12.5-poz_Z-pom_view, 0-poz_X, 0, 0- poz_Z, 0, 1, 0);
	}		


	//podesavamo da se kamera rotira oko igraca
	glTranslatef(-poz_X, 0, -poz_Z);    					
	glRotatef(spin, 0, 1, 0);								
	glTranslatef(poz_X, 0, poz_Z);				

	glRotatef(180, 0, 1, 0);


	//crtamo okolinu lavirinta
	glColor3f(0.13, 0.52, 0.69);
	glBegin(GL_QUADS);
	glVertex3f(-4*width,  -1.01, -4*width);
	glVertex3f(-4*width, -1.01, 2*width);
	glVertex3f(2*width, -1.01, 2*width);
	glVertex3f(2*width, -1.01, -4*width);
	glEnd();

	
	glColor3f(0.13, 0.52, 0.69);
	glBegin(GL_QUADS);
	/*glVertex3f(0-1,-1,0-1);
	glVertex3f(0-1, -1, width*2-1);
	glVertex3f(0-1, width*2, width*2-1);
	glVertex3f(0-1, width*2, 0-1);*/

	glVertex3f(0-width*2, width*2, width*2-1);
	glVertex3f(0-width*2, -1, width*2-1);
	glVertex3f(width*2-1, -1, width*2-1);
	glVertex3f(width*2-1, width*2, width*2-1);
	
	/*lVertex3f(0-1, -1, 0-1);
	glVertex3f(0-1, width*2, 0-1);
	glVertex3f(width*2-1, width*2, 0-1);
	glVertex3f(width*2-1, -1, 0-1);*/
	
	glVertex3f(width*2-1, -1, 0-width*2);
	glVertex3f(width*2-1, width*2, 0-width*2);
	glVertex3f(width*2-1, width*2, width*2-1);
	glVertex3f(width*2-1, -1, width*2-1);
	glEnd();



	//crtamo zidove i pod
	for(int i = 0; i < width; i++) {
		for(int j=0; j < height; j++) {
			//Crtamo zid
			if (matrixMaze[i][j] == 0) {
				glPushMatrix();
					glTranslatef(2*i, 0, j*2);
					glEnable(GL_TEXTURE_2D);
					drawWall(1, -1, 2, -2, 1, -1);
					glDisable(GL_TEXTURE_2D);
				glPopMatrix();
			} else {
				//Crtamo pod
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
    //crtanje igraca i postavljanje na mesto
   	glRotatef(45, 0, 1, 0);
    
   
    glRotatef(-spin, 0, 1, 0);
    glScalef(1, 1, 0.5);
    glutSolidCube(0.5);
 

    //leva ruka
    glPushMatrix();
    glRotatef(
            /* ugao */
            sin(animation_parameter / 5.0f) * 30.0f,
            /* vektor rotacije */
            1,0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(0.33, 0, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();

    //desna ruka
    glPushMatrix();
    glRotatef(
            /* ugao */
            -sin(animation_parameter / 5.0f) * 30.0f,
            /* vektor rotacije */
           1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(-0.33, 0, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();

    //leva noga
    glPushMatrix();
    glRotatef(
            /* ugao */
            -sin(animation_parameter / 5.0f) * 30.0f,
            /* vektor rotacije */
            1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(0.13, -0.5, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();


    //desna noga
    glPushMatrix();
    glRotatef(
            /* ugao */
            sin(animation_parameter / 5.0f) * 30.0f,
            /* vektor rotacije */
            1, 0, 0
        );
    glColor3f(0.5, 0, 0);
    glTranslatef(-0.13, -0.5, 0);
    glScalef(0.85, 3.5, 1);
    glutSolidCube(0.15);
    glPopMatrix();


    glColor3f(1, 1, 1);
    glTranslatef(0, 0.5, 0);
    glutSolidSphere(0.2, 50, 50);

    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

	/*Postavlja se nova slika u prozor*/
	glutSwapBuffers();
}