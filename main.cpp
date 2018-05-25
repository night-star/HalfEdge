#include <halfEdge.h>
#include <Loop_subdivision.h>
#include<Collapse.h>
#include<GL\glut.h>


#define  GLUT_WHEEL_UP 3 //定义滚轮操作  
#define  GLUT_WHEEL_DOWN 4  

std::vector<face>faces;
std::vector<vertex> vers;
std::vector<half_edge> halfs;

float xRotate = 0.0f;
float yRotate = 0.0f;
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;
GLdouble sizeOfE = 1.0f;
double width = 700.0; //窗体的宽
double height = 700.0;//窗体的高

void onMouseMove(int x, int y) {
	if (mousetate) {
		//x对应y是因为对应的是法向量
		yRotate += x - Oldx;
		glutPostRedisplay();
		Oldx = x;
		xRotate += y - Oldy;
		glutPostRedisplay();
		Oldy = y;
	}
}
void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_WHEEL_DOWN) {
		sizeOfE -= 0.1f;
		glutPostRedisplay();
	}
	if (button == GLUT_WHEEL_UP) {
		sizeOfE += 0.1f;
		glutPostRedisplay();
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousetate = 1;
		Oldx = x;
		Oldy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mousetate = 0;
}
void LoopS(int key, int x, int y)
{
	if (key == GLUT_KEY_UP) {
		LoopSub *loopsub = new LoopSub(vers, halfs, faces);
		loopsub->Subdivision();
		faces = loopsub->upfaces;
		vers = loopsub->upvers;
		halfs = loopsub->uphes;
		delete loopsub;
	}

	if (key == GLUT_KEY_DOWN) {

		Collapse *col = new Collapse(vers, halfs, faces);
		col->coll();

		faces = col->faces;
		vers = col->vers;
		halfs = col->hes;
		delete col;
	}

	glutPostRedisplay();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glScalef(sizeOfE, sizeOfE, sizeOfE); // 缩放
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f); // 让物体旋转的函数 第一个参数是角度大小，后面的参数是旋转的法向量
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	int num = faces.size();
	for (int i = 0; i < num; i++)
	{
		int v1 = faces[i].v1;
		int v2 = faces[i].v2;
		int v3 = faces[i].v3;
		glColor3f(0.0f, 1.0f, 1.0f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(vers[v1].x, vers[v1].y, vers[v1].z);
		glVertex3f(vers[v2].x, vers[v2].y, vers[v2].z);
		glVertex3f(vers[v3].x, vers[v3].y, vers[v3].z);
		glEnd();
		/*glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_TRIANGLES);
		glVertex3f(vers[v1].x, vers[v1].y, vers[v1].z);
		glVertex3f(vers[v2].x, vers[v2].y, vers[v2].z);
		glVertex3f(vers[v3].x, vers[v3].y, vers[v3].z);
		glEnd();*/

	}
	glPopMatrix();
	glutSwapBuffers();
}

void changeWindowSize(GLsizei w, GLsizei h)
{
	GLdouble length = 1.0f;
	width = (double)w;
	height = (double)h;
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 

	if (w <= h) glOrtho(-length, length, -length * h / w, length * h / w, -length, length);
	else glOrtho(-length * w / h, length * w / h, -length, length, -length, length);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
int main(int argc, char *argv[])
{
	std::string path = "C:\\Users\\Administrator\\source\\repos\\Gl_3\\Gl_3\\bunny.off";
	//std::string path = "C:\\Users\\Administrator\\Desktop\\my.off";
	//std::string path = "E:\\Downloads\\octahedron.off";

	halfEdge *h = new halfEdge(path);
	faces = h->faces;
	vers = h->vers;
	halfs = h->hes;


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Half_edge");

	glutDisplayFunc(display);
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glutReshapeFunc(changeWindowSize);
	glutMouseFunc(myMouse);
	glutMotionFunc(onMouseMove); // 鼠标移动的时候的函数
	glutSpecialFunc(LoopS);
	glutMainLoop();
	system("pause");
    return 0;
}

