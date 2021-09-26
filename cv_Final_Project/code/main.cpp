#define _CRT_SECURE_NO_WARNINGS
#define GLUT_DISABLE_ATEXIT_HACK   
#include "SkyBox.h"
#define NDEBUG 
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "LSystem.h"
#include "Camera.h"
using namespace std;

double depthImage[256][256];
LSystem myLSystem;
Camera myCamera;
GLuint texin;
GLuint texout; //����

#define GL_CLAMP_TO_EDGE    0x812F
#define BMP_Header_Length 54  //ͼ���������ڴ���е�ƫ����
#define Pi 3.14159265358979
GLfloat scale = 0.5;//�Ŵ���С

GLfloat lightPosition[4] = { 30.0, 40.0, 50.0, 0.0 };//��Դλ��

CSkyBox m_SkyBox;
GLint WindW = 800;
GLint WindH = 600;
//GLfloat oldx, oldy;//����ƶ�����
GLfloat xangle = 15.0f;
GLfloat yangle = -15.0f;//��ת����  
GLuint texout2; //����
GLfloat Pi1 = 3.14159;
GLboolean showShadow=true;
int p = 20;
int q = 255;
int k = 0;
float incr = 0.05;

char b[128] = "res/��Ҷ.bmp";
char c[128] = "res/����Ҷ.bmp";
char* f[128] = { b,c };


void initParameter() {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			depthImage[i][j] = 1.0f;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	//������Ӱͼ��ع��˷�ʽ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//���ò��ϵ�����
	GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //���ʵľ��淴����ɫ
	GLfloat materialDiffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };//ɢ���ɫ��
	GLfloat materialShininess[] = { 10.0f };//���淴��ָ��
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

	//���ù�Դ������
	GLfloat lightAmbient[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightDiffuse[4] = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat lightSpecular[4] = { 0.9, 0.9, 0.9, 0.0 };
	//GLfloat lightPosition[4] = { 30.0, 40.0, 50.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);//ָ����Դ0����������
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);//ָ����Դ0�ľ�������
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);//ָ����Դ0��λ��


	GLfloat  white[] = { 1.0, 1.0, 1.0, 1.0 };

	//�����������(��Ӱͼ��ֻ������ȣ���ͼ����ɫ��Ϣ�������ģ�����������ݲ���������NULL)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);//GL_DEPTH_COMPONENT:��������ʽ�����ڽ����ֵ��¼������ͼ��

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

	/*�����������*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);//�ȽϹ���
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);//��Ӱͼ����
	/*��Ϊ��ͨ�Ա�ģʽ��GL_NONE�������õ���ͼ�Ա�ģʽ��GL_COMPARE_REF_TO_TEXTURE��,����ʹ�õ����������ͼ�����Թ��˵ģ���ǰ����ֱ����䡣*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	//�Զ�������������
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//���ù�Դ
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//���ڵĳ�ʼ
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//������������Ļ���������������������Ҫ�������������ƬԪ��ɫ���м����
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glMatrixMode(GL_PROJECTION);//������ͶӰ��һ��ƽ����
	glLoadIdentity();//��ǰ������ָ���һ����λ����
	glMatrixMode(GL_MODELVIEW);//��LookAt()�����ĺ���
	glDepthFunc(GL_LEQUAL);//ָ����Ŀ�������뵱ǰ������z������ֵ��С�Ƚϡ��ĺ���
	glEnable(GL_DEPTH_TEST);//���ͨ���ȽϺ����ֵ�����仯�ˣ�����и�����Ȼ������Ĳ�����
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//GL_PERSPECTIVE_CORRECTION_HINT��ʾ��ɫ����������岹��������        GL_NICESTѡ������ȷ�����������ѡ��
	glEnable(GL_ALPHA_TEST);//���ݺ���glAlphaFunc������Ҫ��������ͼ��͸���Ĳ���Ƿ���ʾ

	/** �������� */
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_COLOR);
	glEnable(GL_BLEND);
	myLSystem.generateFractal();//����õ����ζ���������Ϣ
	/** ��ʼ����� */
	if (!m_SkyBox.Init())
	{
		MessageBox(NULL, (LPCWSTR)"��ʼ�����ʧ��!", (LPCWSTR)"����", MB_OK);
		exit(0);
	}
	myCamera.setCamera(40, 10, 40, 0, 0, 0, 0, 1, 0);
}

int power_of_two(int n)
{
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

//��ȡ��֦����
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// ���ļ������ʧ�ܣ�����  
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// ��ȡ�ļ���ͼ��Ŀ�Ⱥ͸߶�  
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// ����ÿ��������ռ�ֽ����������ݴ����ݼ����������ֽ���  
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// �����������ֽ��������ڴ�  
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// ��ȡ��������  
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// �Ծ;ɰ汾�ļ��ݣ����ͼ��Ŀ�Ⱥ͸߶Ȳ��ǵ������η�������Ҫ��������  
	// ��ͼ���߳�����OpenGL�涨�����ֵ��Ҳ����  
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // �涨���ź��µĴ�СΪ�߳���������  
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// ����ÿ����Ҫ���ֽ��������ֽ���  
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// �����ڴ�  
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// ������������  
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// �ͷ�ԭ�����������ݣ���pixelsָ���µ��������ݣ�����������width��height  
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// ����һ���µ�������  
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// ���µ������������������������  
	// �ڰ�ǰ���Ȼ��ԭ���󶨵������ţ��Ա��������лָ�  
	GLint lastTextureID = last_texture_ID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	//glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //�ָ�֮ǰ�������  
	free(pixels);
	return texture_ID;
}

//��ȡ��Ҷ����
int LoadGLTextures(char* Filename, GLuint* ttexture, int i, int r, int a)//ttexture[i]Ϊ�������������ݵ�����,�ɸ���  
{
	FILE* File;
	BITMAPINFOHEADER header;
	BYTE* texture;//texture���������RGBA texture[width][height][4]  
	int width, height, m, j;
	unsigned char* image;
	fopen_s(&File, Filename, "rb");
	//��ȡBMP��Ϣͷ������14�ֽ��ļ�ͷ  
	if (File) {
		fseek(File, 14, SEEK_SET);
		fread(&header, sizeof(BITMAPINFOHEADER), 1, File);
	}
	else return FALSE;
	//��ȡ����  
	width = header.biWidth;
	height = header.biHeight;
	//Ϊimage�������ؿռ䣬��ȡͼƬ���ݣ�Ϊtexture����width*height*4����λ�ռ�������������  
	image = (unsigned char*)malloc(width * height * 3);
	fread(image, sizeof(unsigned char), width * height * 3, File);//Ψһ�Ĳ���֮�����ڽ�3�ֽ����ض�Ϊһά�ַ���  
	texture = (BYTE*)malloc(width * height * 4);// Ψһ�Ĳ���֮�����ڽ�4�ֽ����ض�Ϊһά�ַ���  
											   //���´��뽫��texture�����У�һ����˵ΪBlue Green Red Alpha��ʽ(24λͼ��ʽ),��������ʹ��BGRAģʽ  
											   /****************************************�޸�ģ��,ͨ�����������޸�ͼƬ��ʾ��:*************************************************************/
											   //���δ��뽫����ɫ���ص�͸����(Alpha)��Ϊ100%,������ɫ��Ϊ0%  
	for (m = 0; m < width; m++)
	{
		for (j = 0; j < height; j++)
		{
			//����ɫֵд��   
			texture[m * width * 4 + j * 4] = image[m * width * 3 + j * 3];
			texture[m * width * 4 + j * 4 + 1] = image[m * width * 3 + j * 3 + 1];
			texture[m * width * 4 + j * 4 + 2] = image[m * width * 3 + j * 3 + 2];
			if (texture[m * width * 4 + j * 4] >= 200 && texture[m * width * 4 + j * 4 + 1] >= 200 && texture[m * width * 4 + j * 4 + 2] >= 200) {
				texture[m * width * 4 + j * 4 + 3] = 0;                 //͸����alpha=0   
			}
			else
				texture[m * width * 4 + j * 4 + 3] = a;           //��͸����alpha=255
			//����ɫֵд��   
			//texture[m * width * 4 + j * 4] = image[m * width * 3 + j * 3];
			//texture[m * width * 4 + j * 4 + 1] = image[m * width * 3 + j * 3 + 1];
			//texture[m * width * 4 + j * 4 + 2] = image[m * width * 3 + j * 3 + 2];
			//texture[m * width * 4 + j * 4] = b;
			//texture[m * width * 4 + j * 4 + 1] = g;
			texture[m * width * 4 + j * 4 + 2] = r;
		}
	}

	//ttexture[i]Ϊ�����������  
	//�������������Լ�������  
	glGenTextures(1, &ttexture[i]);
	glBindTexture(GL_TEXTURE_2D, ttexture[i]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texture);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	//glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glAlphaFunc(GL_GREATER, 0.1);//ʹAlphaֵ��Ч  
								 //�����������,�����ͷſռ�  
	free(texture);
	free(image);                        // �ͷ�ͼ������  
	fclose(File);
	return TRUE;                                // ���� Status  
}



void reshape(int width, int height)
{
	glViewport(0, 0, width, height);                                    /**< ���������ӿ� */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 4000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//����֦
void drawcone(double r, double h) {
	glBindTexture(GL_TEXTURE_2D, texin);
	for (int i = 0; i < 360; i += 50) { //��ԭ�㻭��һ��Բ��
		float temp = i * Pi / 180;
		float temp1 = (i + 50) * Pi / 180;
		glBegin(GL_QUADS);//GL_QUADS�������ĸ�������ɵ�һ�鵥�����ı���
		glTexCoord2f((temp * r * myLSystem.getRadiusFactor()) / (2 * Pi), 0.0f);
		glVertex3f(r * cos(temp) * myLSystem.getRadiusFactor(), 0, r * sin(temp) * myLSystem.getRadiusFactor());
		glTexCoord2f((temp * r) / (2 * Pi), 1.0f);
		glVertex3f(r * cos(temp), h, r * sin(temp)); //����2�� ����1/2
		glTexCoord2f((temp1 * r) / (2 * Pi), 1.0f);
		glVertex3f(r * cos(temp1), h, r * sin(temp1));
		glTexCoord2f((temp1 * r * myLSystem.getRadiusFactor()) / (2 * Pi), 0.0f);
		glVertex3f(r * cos(temp1) * myLSystem.getRadiusFactor(), 0.0f, r * sin(temp1) * myLSystem.getRadiusFactor());
		Vector3 p1 = Vector3(r * cos(temp) - r * cos(temp) * myLSystem.getRadiusFactor(), h - 0, r * sin(temp) - r * sin(temp) * myLSystem.getRadiusFactor());
		Vector3 p2 = Vector3(r * cos(temp1) - r * cos(temp) * myLSystem.getRadiusFactor(), h - 0, r * sin(temp1) - r * sin(temp) * myLSystem.getRadiusFactor());
		Vector3 n = p1.crossProduct(p2).normalize();
		glNormal3f(n.x, n.y, n.z);//���ı��εķ�����
		glEnd();
	}
}

//�任����֦��λ��
void  DrawChannel(glm::vec3 A, glm::vec3 B, double r)
{
	// ��ʼ�߶Σ���(0,1,0)Ϊ���,���ĳ���(distance)ͨ��Ŀ���߶μ���,  
	//           �յ�����Ϊ(0,1-distance,0)  
	// Ŀ���߶Σ���(x1,y1,z1)Ϊ��㣬��(x2,y2,z2)Ϊ�յ�  
	// ����Ŀ������  
	GLfloat   dx = B.x - A.x;
	GLfloat   dy = B.y - A.y;
	GLfloat   dz = B.z - A.z;
	// ���Ŀ������ģ(��AB����)  
	GLfloat   distance = sqrt(dx * dx + dy * dy + dz * dz);
	// ����ƽ����  
	GLfloat  px = A.x;
	GLfloat  py = A.y - 1;
	GLfloat  pz = A.z;
	// ��ʼ�߶ε�ĩ�˵�  
	GLfloat  bx = px;
	GLfloat  by = (1 - distance) + py;
	GLfloat  bz = pz;
	// ������ʼ����  
	GLfloat  sx = bx - A.x;
	GLfloat  sy = by - A.y;
	GLfloat  sz = bz - A.z;
	// ��������(sx,sy,sz)������(dx,dy,dz)�ķ�����(sy*dz - dy*sz,sz*dx - sx*dz,sx*dy - dx*sy)  
	GLfloat fx = sy * dz - dy * sz;
	GLfloat fy = sz * dx - sx * dz;
	GLfloat fz = sx * dy - dx * sy;
	// ����������ļн�  
	// ����������ߵĳ���  
	GLfloat ax = fabs(B.x - bx);
	GLfloat ay = fabs(B.y - by);
	GLfloat az = fabs(B.z - bz);
	GLfloat length = sqrt(ax * ax + ay * ay + az * az);
	// �������Ҷ������н�  
	GLfloat angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;
	// ����Բ��   
	glPushMatrix();
	//ƽ��(0,-1,0)
	//�Ʒ�������ת
	//ƽ�� 
	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawcone(r, distance);
	glPopMatrix();
}

//����Ҷ
void drawsquare(double r) {
	glRotated(180, 0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, texout);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(r / 2, 0.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(r / 2, r, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-r / 2, r, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-r / 2, 0, 0);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glEnd();
}

//�任����Ҷλ��
void DrawLeaf(glm::vec3 A, glm::vec3 B, double r) {
	// ��ʼ�߶Σ���(0,1,0)Ϊ���,���ĳ���(distance)ͨ��Ŀ���߶μ���,  
	//           �յ�����Ϊ(0,1-distance,0)  
	// Ŀ���߶Σ���(x1,y1,z1)Ϊ��㣬��(x2,y2,z2)Ϊ�յ�  
	// ����Ŀ������  
	GLfloat   dx = B.x - A.x;
	GLfloat   dy = B.y - A.y;
	GLfloat   dz = B.z - A.z;
	// ���Ŀ������ģ(��AB����)  
	GLfloat   distance = sqrt(dx * dx + dy * dy + dz * dz);
	// ����ƽ����  
	GLfloat  px = A.x;
	GLfloat  py = A.y - 1;
	GLfloat  pz = A.z;
	// ��ʼ�߶ε�ĩ�˵�  
	GLfloat  bx = px;
	GLfloat  by = (1 - distance) + py;
	GLfloat  bz = pz;
	// ������ʼ����  
	GLfloat  sx = bx - A.x;
	GLfloat  sy = by - A.y;
	GLfloat  sz = bz - A.z;
	// ��������(sx,sy,sz)������(dx,dy,dz)�ķ�����(sy*dz - dy*sz,sz*dx - sx*dz,sx*dy - dx*sy)  
	GLfloat fx = sy * dz - dy * sz;
	GLfloat fy = sz * dx - sx * dz;
	GLfloat fz = sx * dy - dx * sy;
	// ����������ļн�  
	// ����������ߵĳ���  
	GLfloat ax = fabs(B.x - bx);
	GLfloat ay = fabs(B.y - by);
	GLfloat az = fabs(B.z - bz);
	GLfloat length = sqrt(ax * ax + ay * ay + az * az);
	// �������Ҷ������н�  
	GLfloat angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;
	//cout << angle << endl;
	glPushMatrix();
	// �任��˳���뺯����д��˳���෴��  
	// ����ƽ��(0,-1,0)�����Ʒ�������ת�������ƽ��  
	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawsquare(r);
	glPopMatrix();
}

//������
void Draw_Cube(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2)
{//����������ɣ���֪�����������ɹ���˸�����  
	int i, j;
	GLfloat vertex[8][3] = {
		//�˸����㣬��v1-v8  
		x1,y1,z2,  //0
		x2,y1,z2,  //1
		x1,y2,z2,  //2
		x2,y2,z2,  //3
		x1,y1,z1,  //4
		x2,y1,z1,  //5
		x1,y2,z1,  //6
		x2,y2,z1,  //7
	};
	GLint surface[6][4] = {
		//v1��Ӧ0,�Դ�����  
		4,5,7,6,  //back
		0,1,3,2,  //front
		2,3,7,6,  //top
		0,1,5,4,  //bottom
		0,2,6,4,  //left
		1,3,7,5   //right
	};
	CBMPLoader texture1;
	char g[128] = "./res/gras.bmp";
	texture1.LoadBitmap(g);
	
	//cout << texture1.imageHeight << " " << texture1.imageWidth << endl;
	BYTE* ttexture = (BYTE*)malloc(256 * 256 * 3);
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			if (depthImage[i][j] < 0.99) {
				//cout << texture1.image[(255 - i) * 256 * 3 + j * 3] << endl;
				ttexture[(255 - i) * 256 * 3 + j * 3] = 0;
				ttexture[(255 - i) * 256 * 3 + j * 3+1] = 0;
				ttexture[(255 - i) * 256 * 3 + j * 3+2] = 0;
			}
			else {
				if (texture1.image) {
					ttexture[(255 - i) * 256 * 3 + j * 3] = texture1.image[(255 - i) * 256 * 3 + j * 3];
					ttexture[(255 - i) * 256 * 3 + j * 3 + 1] = texture1.image[(255 - i) * 256 * 3 + j * 3 + 1];
					ttexture[(255 - i) * 256 * 3 + j * 3 + 2] = texture1.image[(255 - i) * 256 * 3 + j * 3 + 2];
				}
			}
		}
	}
	glGenTextures(1, &texture1.ID);
	glBindTexture(GL_TEXTURE_2D, texture1.ID); 
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, fColor);
	//��������
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture1.imageWidth,
		texture1.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		ttexture);
	CBMPLoader texture2;
	texture2.LoadBitmap(g);

	glGenTextures(1, &texture2.ID);
	glBindTexture(GL_TEXTURE_2D, texture2.ID);
	//�����������
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture2.imageWidth,
		texture2.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		texture2.image);
	//��ÿ��������ֳ���������� 
	for (i = 0; i < 6; i++)
	{
		if (i == 2) {
			glBindTexture(GL_TEXTURE_2D, texture1.ID);
			//glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
			glBegin(GL_POLYGON);

			glTexCoord2f(0.0, 0.0); glVertex3fv(vertex[surface[i][0]]);
			glTexCoord2f(0.0, 1.0); glVertex3fv(vertex[surface[i][1]]);
			glTexCoord2f(1.0, 1.0); glVertex3fv(vertex[surface[i][2]]);
			glTexCoord2f(1.0, 0.0); glVertex3fv(vertex[surface[i][3]]);
		}
		else {
			glBindTexture(GL_TEXTURE_2D, texture2.ID);
			glBegin(GL_POLYGON);//ָ�����Ʒ�ʽ��������� GL_POLYGON ��������� GL_LINE_LOOP

			glTexCoord2f(0.0, 0.0); glVertex3fv(vertex[surface[i][0]]);
			glTexCoord2f(0.0, 1.0); glVertex3fv(vertex[surface[i][1]]);
			glTexCoord2f(1.0, 1.0); glVertex3fv(vertex[surface[i][2]]);
			glTexCoord2f(1.0, 0.0); glVertex3fv(vertex[surface[i][3]]);
		}
		//glDrawPixels(256, 256, GL_LUMINANCE,GL_FLOAT, depthImage);
		Vector3 p1 = Vector3(vertex[surface[i][1]][0] - vertex[surface[i][0]][0], vertex[surface[i][1]][1] - vertex[surface[i][0]][1], vertex[surface[i][1]][2] - vertex[surface[i][0]][2]);
		Vector3 p2 = Vector3(vertex[surface[i][3]][0] - vertex[surface[i][0]][0], vertex[surface[i][3]][1] - vertex[surface[i][0]][1], vertex[surface[i][3]][2] - vertex[surface[i][0]][2]);
		Vector3 n=p1.crossProduct(p2).normalize();
		if (i % 2 == 1)
			n = n * (-1.0f);
		//cout <<i<<" "<< n.x <<" "<< n.y << " " << n.z << endl;
		glNormal3f(n.x, n.y, n.z);
		glEnd();
	}

}

void drawtree(GLboolean shadowRender)
{
	GLboolean textureOn = glIsEnabled(GL_TEXTURE_2D);

	if (shadowRender)
		glDisable(GL_TEXTURE_2D);

	/**������ */
	//glColor4f(0.0, 0.0, 0.0, 0.5);

	glTranslatef(0, -20, 0);
	glScalef(scale, scale, scale);
	Draw_Cube(-40.0, 40.0, -2.0, 0.0, -40.0, 40.0);//������
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < myLSystem.trunks.size(); i++) {
		//cout << lsrule.trunks[i].radius << endl;
		DrawChannel(myLSystem.trunks[i].start, myLSystem.trunks[i].end, myLSystem.trunks[i].radius);
	}
	LoadGLTextures(b, &texout, 0, p, q);
	for (int i = 0; i < myLSystem.leafs.size(); i++) {//����Ҷ
		//cout << lsrule.trunks[i].radius << endl;
		DrawLeaf(myLSystem.leafs[i].pos, myLSystem.leafs[i].end, myLSystem.getLeafRadius());
	}

	if (shadowRender && textureOn)
		glEnable(GL_TEXTURE_2D);
}


void display(void)
{

	/** �û��Զ���Ļ��ƹ��� */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/** ��������� */
	myCamera.setLook();
	/** ������� */
	m_SkyBox.CreateSkyBox(-32, -32, -32, 0.25, 0.25, 0.25);
	
	/**������ */
	drawtree(GL_FALSE);
	//glColor4f(0.0, 0.0, 0.0, 0.5);
	//glFlush();                   
	glutSwapBuffers();//��������������ָ��
}


void motion(int x, int y)
{
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		myCamera.setSpeed(1.0f);
		break;
	case '2':
		myCamera.setSpeed(10.0f);
		break;
	case 'W':
		myCamera.rotateCamera(myCamera.getSpeed());
		break;
	case 'S':
		myCamera.rotateCamera(-myCamera.getSpeed());
		break;
	case 'A':
		myCamera.yawCamera(myCamera.getSpeed());
		break;
	case 'D':
		myCamera.yawCamera(-myCamera.getSpeed());
		break;
	case '3':
		myCamera.moveCamera(myCamera.getSpeed());
		break;
	case '4':
		myCamera.moveCamera(-myCamera.getSpeed());
		break;
	//ʵ����Ҷ���������任
	case '5':
		myLSystem.leafRadius *= 0.97;
		if (myLSystem.leafRadius < 0.1)
			myLSystem.leafRadius = 0.1;
		break;
	case '6':
		myLSystem.leafRadius /= 0.97;
		if (myLSystem.leafRadius > 5.0)
			myLSystem.leafRadius = 5.0;
		break;
	//�����ﶬ�ļ��任
	case '7':
		myLSystem.leafRadius /= 0.97;
		if (myLSystem.leafRadius > 5.0) {
			myLSystem.leafRadius = 5.0;
			p += 4;
			if (p > 255) {
				p = 255;
				q -= 5;
				if (q < 0)
					q = 0;
			}
		}
		break;
	case 0:
		break;
	}
	glutPostRedisplay();
	cout << "key::" << key << endl;
}

int  main(int  argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//GLUT_DOUBLE��ʾʹ��˫���崰�� GLUT_RGB��ɫģʽ  GLUT_DEPTH��Ȼ���
	glutInitWindowPosition(200, 50);//���ó�ʼ���ڵ�λ��(�������Ͻ��������������(x, y))
	glutInitWindowSize(WindW, WindH);
	glutCreateWindow("��3�� L-system������");
	initParameter();
	char b[] = "res/��Ҷ.bmp";
	LoadGLTextures(b, &texout, 0,0,255);
	texin = load_texture("res/bark1.bmp");  //��������
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMainLoop();
	system("pause");
	return 0;

}
