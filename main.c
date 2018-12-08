#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);


// Parametri za menjanje pozicije igraca
static float poz_X = 0, poz_Z = 0;

//Ugao za koji se rotira kamera i igrac
static float spin = 0;


//pomocna promenljiva za podesavanje blizine kamere
static int pom_view = 0;


int main(int argc, char *argv[])
{
	/*Vrsimo inicijalizaciju osnovnih funkcionalnosti*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	/*Inicijalizacija prozora*/
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	/*Registruju se funkcije za obradu dogadjaja*/
	glutKeyboardFunc(on_keyboard);
	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);

	/* Obavlja se openGL inicijalizacija*/
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.75, 0.75, 0.75, 0);

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
	gluPerspective(60, (float)width/height, 1, 1500);
}

static void on_keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27: exit(0);
				break;
		case 'b':
		case 'B': printf("BANECARžn");
				break;
		case 'w': //Pomeranje napred i nazad, A i D su za pomeranje ugla kamere i igraca
		case 'W':
		/**************************************************************
		FIXME: srediti uglove spina, provali sta je greska, namesti lepo koordinate...
		Mora da se podeli nekim brojem koji lepo namesta sve. 
		Kamera je ok, samo koordinate rastu naglo zbog spina
		***************************************************************/
				poz_X += cos(spin * (M_PI/180.0)) - sin(spin * (M_PI/180.0));
				poz_Z += cos(spin * (M_PI/180.0)) + sin(spin * (M_PI/180.0));
        
			glutPostRedisplay();
			break;
			
		case 's':
		case 'S':
				poz_X -= cos(spin * (M_PI/180.0)) - sin(spin * (M_PI/180.0));
				poz_Z -= cos(spin * (M_PI/180.0)) + sin(spin * (M_PI/180.0));
			glutPostRedisplay();
			break;
			
		case 'a':
		case 'A':
			{
			spin -=	3;
			spin = (int)spin % 360;
			printf("%f\n", spin);
			glutPostRedisplay();
			break;
			}
		case 'd':
		case 'D':
			{
            spin += 3;
            spin = (int)spin % 360;
            printf("%f\n", spin);
			glutPostRedisplay();
			break;
			}
		case 'c':
		case 'C':
			{
				pom_view = 7.5;
				glutPostRedisplay();
				break;
			}
		case 'f':
		case 'F':
			{
				pom_view = 0;
				glutPostRedisplay();
				break;
			}
	}
}

/* funkcija za crtanje poda*/
void drawFloor(GLfloat x1, GLfloat x2, GLfloat z1, GLfloat z2)
{
	
	glBegin(GL_POLYGON);
		glNormal3f( 0.0, 1.0, 0.0);
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f( x1, -1, z2 );
		glVertex3f( x2, -1, z1 );
		glVertex3f( x1, -1, z1 );
    	glEnd();
}


//2 - pod, 0- zid   
/*************************************************
TODO: DFS algo za generisanje random lavirinta!
**************************************************/
/* matrica lavirinta */
int a[12][12] = {
	   {2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0} ,
	   {0, 2, 0, 2, 0, 0, 0, 2, 2, 2, 2, 0} ,
	   {0, 2, 2, 2, 2, 0, 0, 2, 0, 2, 2, 0} ,
	   {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0} ,
	   {0, 0, 2, 2, 0, 2, 0, 0, 0, 2, 0, 0},
	   {0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0} ,
	   {0, 0, 0, 2, 0, 0, 0, 2, 0, 2, 0, 0} ,
	   {0, 0, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0} ,
	   {0, 2, 2, 2, 0, 2, 0, 2, 0, 2, 0, 0} ,
	   {0, 0, 2, 2, 0, 0, 2, 2, 2, 2, 2, 0},
	   {0, 0, 2, 2, 0, 0, 0, 0, 2, 2, 2, 0},
	   {0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0}
};

/*************************************************************************
TODO: Srediti teksture i velicinu zida, promeni da crta poligon mozda?
 NAMESTI KOLIZIJE
****************************************************************************/


/*f-ja za crtanje zida*/
void drawWall(float x1, float x2, float y1, float y2, float z1, float z2){

	glBegin(GL_QUADS);
	glColor3f(0.5, 0.5, 0.5);

	//Desna strana
	glNormal3f(0, 0, 1);
	
	glVertex3f(x1, y1, z1);
  	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y2, z1);
	glVertex3f(x1, y2, z1);


	//Leva
	glNormal3f(0, 0, -1);

	glVertex3f(x1, y1, z2);
	glVertex3f(x2, y1, z2);
	glVertex3f(x2, y2, z2);
	glVertex3f(x1, y2, z2);


	//Gornja
	glColor3f(0, 0, 0);
	glNormal3f (0,1, 0);
	
	glVertex3f(x1, y2, z1);
	glVertex3f(x2, y2, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x1, y2, z2);

	//Zadnja
	
	glNormal3f (1,0, 0);
	glVertex3f(x1, y1, z1);
	glVertex3f(x1, y2, z1);
	glVertex3f(x1, y2, z2);
	glVertex3f(x1, y1, z2);


	//Prednja

	glNormal3f (-1,0, 0);	
	glVertex3f(x2, y1, z1);
	glVertex3f(x2, y2, z1);
	glVertex3f(x2, y2, z2);
	glVertex3f(x2, y1, z2);

	glEnd();
}

static void on_display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Postavlja se kamera*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(15-poz_X -pom_view, 9.5 - pom_view, 15-poz_Z-pom_view, 0-poz_X, 0, 0- poz_Z, 0, 1, 0);
	glTranslatef(-poz_X, 0, -poz_Z);    					// TODO: Namesti da se kamera rotira oko igraca!!!!!!!   /  /
	glRotatef(spin, 0, 1, 0);													//	obavezno! SREDJENO			   \/ \/
	glTranslatef(poz_X, 0, poz_Z);												

	/*Crtamo koordinatni sistem*/
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);
	glEnd(); 

/****************************************************
TODO: Postavi teksturu i za zid i za pod
***************************************************/
for(int i = 0; i < 12; i++) {
		for(int j=0; j < 12; j++) {
			//Crtamo zid
			if (a[i][j] == 0) {
				glPushMatrix();
					glTranslatef(2*i, 0, j*2);
					drawWall(1, -1, 1, -1, 1, -1);
				glPopMatrix();
			} else {
				//Crtamo pod
				glPushMatrix();
					glTranslatef(2*i, 0, j*2);
					drawFloor(1, -1, 1, -1);
				glPopMatrix();

			}
		}
	}
	
	glPushMatrix();
    glTranslatef(0-poz_X, 0, 0-poz_Z);
    
    /*crtanje probnog igraca i postavljanje na mesto*/
    /*************************************************
    TODO: Napravi lepo igraca, noge, ruke, kretanje ruku i nogu 
    **********************************************************/
   	glRotatef(45, 0, 1, 0);
    
    glRotatef(-spin, 0, 1, 0);
    glutSolidCube(1);

    glColor3f(1, 1, 1);
    glTranslatef(0, 1, 0);
    glutSolidSphere(0.4, 50, 50);

    glPopMatrix();

	/*Postavlja se nova slika u prozor*/
	glutSwapBuffers();
}