#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);

// Vektori za kretanje f-forward, b-back
static int mv_Fx=0,mv_Bx=0,mv_Fz=0,mv_Bz=0;

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
		case 'B': printf("BANECARžn");
				break;
				/* kretanje na WASD*/
		case 'W':
		case 'w':
			{
            mv_Fz +=1;
			mv_Fx +=1;
			glutPostRedisplay();
			break;
			}
		case 's':
		case 'S':
			{
            mv_Bz +=1;
			mv_Bx +=1;
			glutPostRedisplay();
			break;
			}
		case 'a':
		case 'A':
			{
			mv_Bz +=1;
            mv_Fx +=1;
			glutPostRedisplay();
			break;
			}
		case 'd':
		case 'D':
			{
            mv_Bx +=1;
			mv_Fz +=1;
			glutPostRedisplay();
			break;
			}
			/*podesavanje blizine kamere c - close ,f- far*/
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
		default: 
				break;
	}
}

static void on_display()
{

	/*Postavlja se boja pozadine*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Postavlja se kamera*/
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(15-mv_Fx+mv_Bx-pom_view, 9.5 - pom_view, 15-mv_Fz+mv_Bz-pom_view, 0-mv_Fx+mv_Bx, 0, 0-mv_Fz+mv_Bz, 0, 1, 0);

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

	/* postavljamo pod*/
	glColor3f(0.5,0,0);
	glBegin(GL_POLYGON);
		glVertex3f(0,0,0);
		glVertex3f(10, 0, 0);
		glVertex3f(10, 0, 10);
		glVertex3f(0, 0, 10);
	glEnd();


	/*postav1ljamo spoljni zid*/

	glColor3f(0,0,0);
	glBegin(GL_LINE_STRIP);
		glVertex3f(0,0,0);
		glVertex3f(10, 0, 0);
		glVertex3f(10, 5, 0);
		glVertex3f(0, 5, 0);
	glEnd();


	glBegin(GL_LINE_STRIP);
		glVertex3f(0,0,0);
		glVertex3f(0, 5, 0);
		glVertex3f(0, 5, 10);
		glVertex3f(0, 0, 10);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3f(10,0,0);
		glVertex3f(10, 5, 0);
		glVertex3f(10, 5, 10);
		glVertex3f(10, 0, 10);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3f(10,0,10);
		glVertex3f(10, 5, 10);
		glVertex3f(0, 5, 10);
		glVertex3f(0, 0, 10);
	glEnd();

	
    /* crtanje probnog igraca i postavljanje na mesto */
	glPushMatrix();
    glTranslatef(0-mv_Fx+mv_Bx, 0, 0-mv_Fz+mv_Bz);
   	glRotatef(45, 0, 1, 0);
    glTranslatef(0, 0.5, 0);
    glutSolidCube(1);

    glPopMatrix();

	/*Postavlja se nova slika u prozor*/
	glutSwapBuffers();
}