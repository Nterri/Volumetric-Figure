#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


double* normal(double A[], double B[], double C[])
{
	double x, y, z;
	double a[] = { A[0] - B[0], A[1] - B[1], A[2] - B[2] };
	double c[] = { C[0] - B[0], C[1] - B[1], C[2] - B[2] };
	x = a[1] * c[2] - c[1] * a[2];
	y = c[0] * a[2] - a[0] * c[2];
	z = a[0] * c[1] - c[0] * a[1];
	double L = sqrt(x * x + y * y + z * z);
	double N[] = { x / L, y / L, z / L };
	return N;
}


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	int z = 3;
	double* N;
	double A1[] = { 3, 3, z };
	double B1[] = { 1, 11, z };
	double C1[] = { 6, 11, z };
	double D1[] = { 10, 18, z };
	double E1[] = { 10, 11, z };
	double F1[] = { 16, 9, z };
	double G1[] = { 9, 8, z };
	double H1[] = { 7, 2, z };
	N = normal(F1, C1, B1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(E1);
	glVertex3dv(F1);
	glEnd();
	N = normal(E1, D1, C1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glEnd();
	N = normal(H1, G1, A1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(A1);
	glVertex3dv(G1);
	glVertex3dv(H1);
	glEnd();
	N = normal(G1, F1, B1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(B1);
	glVertex3dv(F1);
	glVertex3dv(G1);
	glEnd();
	float x1 = 3;
	float y1 = 3;
	float x2 = 7;
	float y2 = 2;
	float x0 = (x1 + x2) / 2;
	float y0 = (y1 + y2) / 2;
	float r = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) / 2;
	double centr1[] = { x0, y0, z };
	double circle1X1[20000] = { 0 };
	double circle1Y1[20000] = { 0 };
	float circleZ1 = z;
	int count1 = 0;
	for (float i = r; i < (r + 360); i += 0.01)
	{
		float x = r * cos(i) + x0;
		float y = r * sin(i) + y0;
		if (x <= 7 && y <= 3) {
			circle1X1[count1] = x;
			circle1Y1[count1] = y;
			count1++;
		}
	}
	double point1[] = { circle1X1[0], circle1Y1[0], z };
	double point2[] = { circle1X1[1], circle1Y1[1], z };
	N = normal(centr1, point1, point2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(centr1);
	for (int i = 0; i < count1; i++)
	{
		glVertex3d(circle1X1[i], circle1Y1[i], z);
	}
	glEnd();
	for (int i = 0; i < count1 - 3; i++)
	{
		if (circle1X1[i] - circle1X1[i + 3] < 0.2 && circle1Y1[i] - circle1Y1[i + 3] < 0.2) {
			double point1[] = { circle1X1[i], circle1Y1[i], z };
			double point2[] = { circle1X1[i + 1], circle1Y1[i + 1], z + 3 };
			double point3[] = { circle1X1[i + 2], circle1Y1[i + 2], z + 3 };
			N = normal(point1, point2, point3);
			glNormal3d(N[0], N[1], N[2]);
			glBegin(GL_POLYGON);
			glColor3d(0.4, 0.5, 0.2);
			glVertex3d(circle1X1[i], circle1Y1[i], z);
			glVertex3d(circle1X1[i + 1], circle1Y1[i + 1], z + 3);
			glVertex3d(circle1X1[i + 2], circle1Y1[i + 2], z + 3);
			glVertex3d(circle1X1[i + 3], circle1Y1[i + 3], z);
			glEnd();
		}
	}

	float q = 0;
	float w = 6.5;
	float rr = sqrt((3 - q) * (3 - q) + (3 - w) * (3 - w));
	double centrr[] = { 2, 7, z };
	double circleq1[20000] = { 0 };
	double circleq2[20000] = { 0 };
	int countq = 0;
	for (float i = rr; i < (rr + 360); i += 0.01)
	{
		float x = rr * cos(i) + q;
		float y = rr * sin(i) + w;
		if (y >= 3 && x >= 1) {
			circleq1[countq] = x;
			circleq2[countq] = y;
			countq++;
		}
	}
	for (int i = 0; i < countq - 3; i++)
	{
		if (circleq1[i] - circleq1[i + 3] < 0.2 && circleq2[i] - circleq2[i + 3] < 0.2) {
			double point1[] = { circleq1[i], circleq2[i], z };
			double point2[] = { circleq1[i + 1], circleq2[i + 1], z + 3 };
			double point3[] = { circleq1[i + 2], circleq2[i + 2], z + 3 };
			N = normal(point3, point2, point1);
			glNormal3d(N[0], N[1], N[2]);
			glBegin(GL_POLYGON);
			glColor3d(0.4, 0.5, 0.2);
			glVertex3d(circleq1[i], circleq2[i], z);
			glVertex3d(circleq1[i + 1], circleq2[i + 1], z + 3);
			glVertex3d(circleq1[i + 2], circleq2[i + 2], z + 3);
			glVertex3d(circleq1[i + 3], circleq2[i + 3], z);
			glEnd();
		}
	}
	for (int i = 0; i < countq - 1; i++)
	{
		if (circleq1[i] - circleq1[i + 1] < 0.2 && circleq2[i] - circleq2[i + 1] < 0.2) {
			double point1[] = { circleq1[i], circleq2[i], z };
			double point2[] = { circleq1[i + 1], circleq2[i + 1], z };
			N = normal(point2, point1, G1);
			glNormal3d(N[0], N[1], N[2]);
			glBegin(GL_TRIANGLE_FAN);
			glColor3d(0.4, 0.5, 0.2);
			glVertex3dv(G1);
			glVertex3d(circleq1[i], circleq2[i], z);
			glVertex3d(circleq1[i + 1], circleq2[i + 1], z);
			glEnd();
		}
	}

	z += 3;
	double A2[] = { 3, 3, z };
	double B2[] = { 1, 11, z };
	double C2[] = { 6, 11, z };
	double D2[] = { 10, 18, z };
	double E2[] = { 10, 11, z };
	double F2[] = { 16, 9, z };
	double G2[] = { 9, 8, z };
	double H2[] = { 7, 2, z };
	for (int i = 0; i < countq - 1; i++)
	{
		if (circleq1[i] - circleq1[i + 1] < 0.2 && circleq2[i] - circleq2[i + 1] < 0.2) {
			double point1[] = { circleq1[i], circleq2[i], z };
			double point2[] = { circleq1[i + 1], circleq2[i + 1], z };
			N = normal(G2, point1, point2);
			glNormal3d(N[0], N[1], N[2]);
			glBegin(GL_TRIANGLE_FAN);
			glColor3d(0.4, 0.5, 0.2);
			glVertex3dv(G2);
			glVertex3d(circleq1[i], circleq2[i], z);
			glVertex3d(circleq1[i + 1], circleq2[i + 1], z);
			glEnd();
		}
	}
	double centr2[] = { x0, y0, z };
	double point3[] = { circle1X1[0], circle1Y1[0], z };
	double point4[] = { circle1X1[1], circle1Y1[1], z };
	N = normal(point4, point3, centr2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(centr2);
	for (int i = 0; i < count1; i++)
	{
		glVertex3d(circle1X1[i], circle1Y1[i], z);
	}
	glEnd();
	N = normal(C2, B2, B1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(B1);
	glVertex3dv(B2);
	glVertex3dv(C2);
	glVertex3dv(C1);
	glEnd();
	N = normal(C2, C1, D1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(C1);
	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(D1);
	glEnd();
	N = normal(E2, D2, D1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(D1);
	glVertex3dv(D2);
	glVertex3dv(E2);
	glVertex3dv(E1);
	glEnd();
	N = normal(F2, E2, E1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(E1);
	glVertex3dv(E2);
	glVertex3dv(F2);
	glVertex3dv(F1);
	glEnd();
	N = normal(G2, F2, F1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(F1);
	glVertex3dv(F2);
	glVertex3dv(G2);
	glVertex3dv(G1);
	glEnd();
	N = normal(H2, G2, G1);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(G1);
	glVertex3dv(G2);
	glVertex3dv(H2);
	glVertex3dv(H1);
	glEnd();
	N = normal(B2, C2, F2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(B2);
	glVertex3dv(C2);
	glVertex3dv(E2);
	glVertex3dv(F2);
	glEnd();
	N = normal(C2, D2, E2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(C2);
	glVertex3dv(D2);
	glVertex3dv(E2);
	glEnd();
	N = normal(A2, G2, H2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(A2);
	glVertex3dv(G2);
	glVertex3dv(H2);
	glEnd();
	N = normal(B2, F2, G2);
	glNormal3d(N[0], N[1], N[2]);
	glBegin(GL_POLYGON);
	glColor3d(0.4, 0.5, 0.2);
	glVertex3dv(B2);
	glVertex3dv(F2);
	glVertex3dv(G2);
	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}