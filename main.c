#include <stdio.h>
#include <GL/glut.h>
#include <stdlib.h>

static void on_display(void);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);


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
		case 'a':
		case 'A': printf("ACAFACAžn");
				break;
		case 'b':
		case 'B': printf("BANECARžn");
				break;
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
	gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);

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

	/*Postavlja se nova slika u prozor*/
	glutSwapBuffers();
}