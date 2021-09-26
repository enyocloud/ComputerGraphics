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
GLuint texout; //纹理

#define GL_CLAMP_TO_EDGE    0x812F
#define BMP_Header_Length 54  //图像数据在内存块中的偏移量
#define Pi 3.14159265358979
GLfloat scale = 0.5;//放大缩小

GLfloat lightPosition[4] = { 30.0, 40.0, 50.0, 0.0 };//光源位置

CSkyBox m_SkyBox;
GLint WindW = 800;
GLint WindH = 600;
//GLfloat oldx, oldy;//鼠标移动因子
GLfloat xangle = 15.0f;
GLfloat yangle = -15.0f;//旋转因子  
GLuint texout2; //纹理
GLfloat Pi1 = 3.14159;
GLboolean showShadow=true;
int p = 20;
int q = 255;
int k = 0;
float incr = 0.05;

char b[128] = "res/树叶.bmp";
char c[128] = "res/银杏叶.bmp";
char* f[128] = { b,c };


void initParameter() {
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			depthImage[i][j] = 1.0f;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	//设置阴影图相关过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//设置材料的属性
	GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //材质的镜面反射颜色
	GLfloat materialDiffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };//散射白色光
	GLfloat materialShininess[] = { 10.0f };//镜面反射指数
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

	//设置光源的属性
	GLfloat lightAmbient[4] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightDiffuse[4] = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat lightSpecular[4] = { 0.9, 0.9, 0.9, 0.0 };
	//GLfloat lightPosition[4] = { 30.0, 40.0, 50.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);//指定光源0的漫射光参数
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);//指定光源0的镜面光参数
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);//指定光源0的位置


	GLfloat  white[] = { 1.0, 1.0, 1.0, 1.0 };

	//生成深度纹理(阴影图，只关心深度，对图像颜色信息并不关心，所以最后数据参数可以是NULL)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		256, 256, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);//GL_DEPTH_COMPONENT:深度纹理格式，用于将深度值记录到这张图中

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

	/*深度纹理特有*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);//比较规则
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);//阴影图亮度
	/*分为普通对比模式（GL_NONE）和引用到贴图对比模式（GL_COMPARE_REF_TO_TEXTURE）,后者使用的深度纹理贴图是线性过滤的，而前者是直接填充。*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	//自动生成纹理坐标
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//启用光源
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	//窗口的初始
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//用来设置纹理的环境参数，纹理环境参数主要控制纹理如何与片元颜色进行计算的
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glMatrixMode(GL_PROJECTION);//把物体投影到一个平面上
	glLoadIdentity();//当前矩阵均恢复成一个单位矩阵
	glMatrixMode(GL_MODELVIEW);//像LookAt()这样的函数
	glDepthFunc(GL_LEQUAL);//指定“目标像素与当前像素在z方向上值大小比较”的函数
	glEnable(GL_DEPTH_TEST);//如果通过比较后深度值发生变化了，会进行更新深度缓冲区的操作。
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);//GL_PERSPECTIVE_CORRECTION_HINT表示颜色和纹理坐标插补的质量。        GL_NICEST选择最正确或最高质量的选项
	glEnable(GL_ALPHA_TEST);//根据函数glAlphaFunc的条件要求来决定图形透明的层度是否显示

	/** 启用纹理 */
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_COLOR);
	glEnable(GL_BLEND);
	myLSystem.generateFractal();//计算得到几何顶点的相关信息
	/** 初始化天空 */
	if (!m_SkyBox.Init())
	{
		MessageBox(NULL, (LPCWSTR)"初始化天空失败!", (LPCWSTR)"错误", MB_OK);
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

//读取树枝纹理
GLuint load_texture(const char* file_name)
{
	GLint width, height, total_bytes;
	GLubyte* pixels = 0;
	GLuint last_texture_ID = 0, texture_ID = 0;

	// 打开文件，如果失败，返回  
	FILE* pFile = fopen(file_name, "rb");
	if (pFile == 0)
		return 0;

	// 读取文件中图象的宽度和高度  
	fseek(pFile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pFile);
	fread(&height, 4, 1, pFile);
	fseek(pFile, BMP_Header_Length, SEEK_SET);

	// 计算每行像素所占字节数，并根据此数据计算总像素字节数  
	{
		GLint line_bytes = width * 3;
		while (line_bytes % 4 != 0)
			++line_bytes;
		total_bytes = line_bytes * height;
	}

	// 根据总像素字节数分配内存  
	pixels = (GLubyte*)malloc(total_bytes);
	if (pixels == 0)
	{
		fclose(pFile);
		return 0;
	}

	// 读取像素数据  
	if (fread(pixels, total_bytes, 1, pFile) <= 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// 对就旧版本的兼容，如果图象的宽度和高度不是的整数次方，则需要进行缩放  
	// 若图像宽高超过了OpenGL规定的最大值，也缩放  
	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!power_of_two(width)
			|| !power_of_two(height)
			|| width > max
			|| height > max)
		{
			const GLint new_width = 256;
			const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形  
			GLint new_line_bytes, new_total_bytes;
			GLubyte* new_pixels = 0;

			// 计算每行需要的字节数和总字节数  
			new_line_bytes = new_width * 3;
			while (new_line_bytes % 4 != 0)
				++new_line_bytes;
			new_total_bytes = new_line_bytes * new_height;

			// 分配内存  
			new_pixels = (GLubyte*)malloc(new_total_bytes);
			if (new_pixels == 0)
			{
				free(pixels);
				fclose(pFile);
				return 0;
			}

			// 进行像素缩放  
			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

			// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height  
			free(pixels);
			pixels = new_pixels;
			width = new_width;
			height = new_height;
		}
	}

	// 分配一个新的纹理编号  
	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		fclose(pFile);
		return 0;
	}

	// 绑定新的纹理，载入纹理并设置纹理参数  
	// 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复  
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
	glBindTexture(GL_TEXTURE_2D, lastTextureID);  //恢复之前的纹理绑定  
	free(pixels);
	return texture_ID;
}

//读取树叶纹理
int LoadGLTextures(char* Filename, GLuint* ttexture, int i, int r, int a)//ttexture[i]为用来绑定纹理数据的整数,可更改  
{
	FILE* File;
	BITMAPINFOHEADER header;
	BYTE* texture;//texture长宽和像素RGBA texture[width][height][4]  
	int width, height, m, j;
	unsigned char* image;
	fopen_s(&File, Filename, "rb");
	//读取BMP信息头，跳过14字节文件头  
	if (File) {
		fseek(File, 14, SEEK_SET);
		fread(&header, sizeof(BITMAPINFOHEADER), 1, File);
	}
	else return FALSE;
	//读取长宽  
	width = header.biWidth;
	height = header.biHeight;
	//为image分配像素空间，读取图片数据，为texture分配width*height*4的四位空间用来生成纹理  
	image = (unsigned char*)malloc(width * height * 3);
	fread(image, sizeof(unsigned char), width * height * 3, File);//唯一的不足之处在于将3字节像素读为一维字符串  
	texture = (BYTE*)malloc(width * height * 4);// 唯一的不足之处在于将4字节像素读为一维字符串  
											   //以下代码将对texture重排列，一般来说为Blue Green Red Alpha格式(24位图格式),生成纹理使用BGRA模式  
											   /****************************************修改模块,通过像素任意修改图片，示例:*************************************************************/
											   //本段代码将纯黑色像素点透明度(Alpha)设为100%,其余颜色设为0%  
	for (m = 0; m < width; m++)
	{
		for (j = 0; j < height; j++)
		{
			//把颜色值写入   
			texture[m * width * 4 + j * 4] = image[m * width * 3 + j * 3];
			texture[m * width * 4 + j * 4 + 1] = image[m * width * 3 + j * 3 + 1];
			texture[m * width * 4 + j * 4 + 2] = image[m * width * 3 + j * 3 + 2];
			if (texture[m * width * 4 + j * 4] >= 200 && texture[m * width * 4 + j * 4 + 1] >= 200 && texture[m * width * 4 + j * 4 + 2] >= 200) {
				texture[m * width * 4 + j * 4 + 3] = 0;                 //透明，alpha=0   
			}
			else
				texture[m * width * 4 + j * 4 + 3] = a;           //不透明，alpha=255
			//把颜色值写入   
			//texture[m * width * 4 + j * 4] = image[m * width * 3 + j * 3];
			//texture[m * width * 4 + j * 4 + 1] = image[m * width * 3 + j * 3 + 1];
			//texture[m * width * 4 + j * 4 + 2] = image[m * width * 3 + j * 3 + 2];
			//texture[m * width * 4 + j * 4] = b;
			//texture[m * width * 4 + j * 4 + 1] = g;
			texture[m * width * 4 + j * 4 + 2] = r;
		}
	}

	//ttexture[i]为绑定纹理的整数  
	//下面生成纹理以及纹理处理  
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
	glAlphaFunc(GL_GREATER, 0.1);//使Alpha值生效  
								 //纹理生成完毕,下面释放空间  
	free(texture);
	free(image);                        // 释放图像数据  
	fclose(File);
	return TRUE;                                // 返回 Status  
}



void reshape(int width, int height)
{
	glViewport(0, 0, width, height);                                    /**< 重新设置视口 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 4000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//画树枝
void drawcone(double r, double h) {
	glBindTexture(GL_TEXTURE_2D, texin);
	for (int i = 0; i < 360; i += 50) { //在原点画了一个圆柱
		float temp = i * Pi / 180;
		float temp1 = (i + 50) * Pi / 180;
		glBegin(GL_QUADS);//GL_QUADS绘制由四个顶点组成的一组单独的四边形
		glTexCoord2f((temp * r * myLSystem.getRadiusFactor()) / (2 * Pi), 0.0f);
		glVertex3f(r * cos(temp) * myLSystem.getRadiusFactor(), 0, r * sin(temp) * myLSystem.getRadiusFactor());
		glTexCoord2f((temp * r) / (2 * Pi), 1.0f);
		glVertex3f(r * cos(temp), h, r * sin(temp)); //竖着2倍 横着1/2
		glTexCoord2f((temp1 * r) / (2 * Pi), 1.0f);
		glVertex3f(r * cos(temp1), h, r * sin(temp1));
		glTexCoord2f((temp1 * r * myLSystem.getRadiusFactor()) / (2 * Pi), 0.0f);
		glVertex3f(r * cos(temp1) * myLSystem.getRadiusFactor(), 0.0f, r * sin(temp1) * myLSystem.getRadiusFactor());
		Vector3 p1 = Vector3(r * cos(temp) - r * cos(temp) * myLSystem.getRadiusFactor(), h - 0, r * sin(temp) - r * sin(temp) * myLSystem.getRadiusFactor());
		Vector3 p2 = Vector3(r * cos(temp1) - r * cos(temp) * myLSystem.getRadiusFactor(), h - 0, r * sin(temp1) - r * sin(temp) * myLSystem.getRadiusFactor());
		Vector3 n = p1.crossProduct(p2).normalize();
		glNormal3f(n.x, n.y, n.z);//该四边形的法向量
		glEnd();
	}
}

//变换到树枝的位置
void  DrawChannel(glm::vec3 A, glm::vec3 B, double r)
{
	// 起始线段：以(0,1,0)为起点,它的长度(distance)通过目标线段计算,  
	//           终点坐标为(0,1-distance,0)  
	// 目标线段：以(x1,y1,z1)为起点，以(x2,y2,z2)为终点  
	// 计算目标向量  
	GLfloat   dx = B.x - A.x;
	GLfloat   dy = B.y - A.y;
	GLfloat   dz = B.z - A.z;
	// 算出目标向量模(即AB长度)  
	GLfloat   distance = sqrt(dx * dx + dy * dy + dz * dz);
	// 计算平移量  
	GLfloat  px = A.x;
	GLfloat  py = A.y - 1;
	GLfloat  pz = A.z;
	// 起始线段的末端点  
	GLfloat  bx = px;
	GLfloat  by = (1 - distance) + py;
	GLfloat  bz = pz;
	// 计算起始向量  
	GLfloat  sx = bx - A.x;
	GLfloat  sy = by - A.y;
	GLfloat  sz = bz - A.z;
	// 计算向量(sx,sy,sz)与向量(dx,dy,dz)的法向量(sy*dz - dy*sz,sz*dx - sx*dz,sx*dy - dx*sy)  
	GLfloat fx = sy * dz - dy * sz;
	GLfloat fy = sz * dx - sx * dz;
	GLfloat fz = sx * dy - dx * sy;
	// 求两向量间的夹角  
	// 计算第三条边的长度  
	GLfloat ax = fabs(B.x - bx);
	GLfloat ay = fabs(B.y - by);
	GLfloat az = fabs(B.z - bz);
	GLfloat length = sqrt(ax * ax + ay * ay + az * az);
	// 根据余弦定理计算夹角  
	GLfloat angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;
	// 绘制圆柱   
	glPushMatrix();
	//平移(0,-1,0)
	//绕法向量旋转
	//平移 
	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawcone(r, distance);
	glPopMatrix();
}

//画树叶
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

//变换到树叶位置
void DrawLeaf(glm::vec3 A, glm::vec3 B, double r) {
	// 起始线段：以(0,1,0)为起点,它的长度(distance)通过目标线段计算,  
	//           终点坐标为(0,1-distance,0)  
	// 目标线段：以(x1,y1,z1)为起点，以(x2,y2,z2)为终点  
	// 计算目标向量  
	GLfloat   dx = B.x - A.x;
	GLfloat   dy = B.y - A.y;
	GLfloat   dz = B.z - A.z;
	// 算出目标向量模(即AB长度)  
	GLfloat   distance = sqrt(dx * dx + dy * dy + dz * dz);
	// 计算平移量  
	GLfloat  px = A.x;
	GLfloat  py = A.y - 1;
	GLfloat  pz = A.z;
	// 起始线段的末端点  
	GLfloat  bx = px;
	GLfloat  by = (1 - distance) + py;
	GLfloat  bz = pz;
	// 计算起始向量  
	GLfloat  sx = bx - A.x;
	GLfloat  sy = by - A.y;
	GLfloat  sz = bz - A.z;
	// 计算向量(sx,sy,sz)与向量(dx,dy,dz)的法向量(sy*dz - dy*sz,sz*dx - sx*dz,sx*dy - dx*sy)  
	GLfloat fx = sy * dz - dy * sz;
	GLfloat fy = sz * dx - sx * dz;
	GLfloat fz = sx * dy - dx * sy;
	// 求两向量间的夹角  
	// 计算第三条边的长度  
	GLfloat ax = fabs(B.x - bx);
	GLfloat ay = fabs(B.y - by);
	GLfloat az = fabs(B.z - bz);
	GLfloat length = sqrt(ax * ax + ay * ay + az * az);
	// 根据余弦定理计算夹角  
	GLfloat angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;
	//cout << angle << endl;
	glPushMatrix();
	// 变换的顺序与函数书写的顺序相反，  
	// 即先平移(0,-1,0)，再绕法向量旋转，最后再平移  
	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawsquare(r);
	glPopMatrix();
}

//画地面
void Draw_Cube(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2)
{//由立方体组成，可知六个参数即可构造八个顶点  
	int i, j;
	GLfloat vertex[8][3] = {
		//八个顶点，从v1-v8  
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
		//v1对应0,以此类推  
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
	//设置纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

	//glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, fColor);
	//加载纹理
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture1.imageWidth,
		texture1.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		ttexture);
	CBMPLoader texture2;
	texture2.LoadBitmap(g);

	glGenTextures(1, &texture2.ID);
	glBindTexture(GL_TEXTURE_2D, texture2.ID);
	//设置纹理过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture2.imageWidth,
		texture2.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		texture2.image);
	//将每个立方体分成六个面绘制 
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
			glBegin(GL_POLYGON);//指定绘制方式，填充多边形 GL_POLYGON 不填充多边形 GL_LINE_LOOP

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

	/**绘制树 */
	//glColor4f(0.0, 0.0, 0.0, 0.5);

	glTranslatef(0, -20, 0);
	glScalef(scale, scale, scale);
	Draw_Cube(-40.0, 40.0, -2.0, 0.0, -40.0, 40.0);//画地面
	glEnable(GL_TEXTURE_2D);
	for (int i = 0; i < myLSystem.trunks.size(); i++) {
		//cout << lsrule.trunks[i].radius << endl;
		DrawChannel(myLSystem.trunks[i].start, myLSystem.trunks[i].end, myLSystem.trunks[i].radius);
	}
	LoadGLTextures(b, &texout, 0, p, q);
	for (int i = 0; i < myLSystem.leafs.size(); i++) {//画树叶
		//cout << lsrule.trunks[i].radius << endl;
		DrawLeaf(myLSystem.leafs[i].pos, myLSystem.leafs[i].end, myLSystem.getLeafRadius());
	}

	if (shadowRender && textureOn)
		glEnable(GL_TEXTURE_2D);
}


void display(void)
{

	/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	/** 放置摄像机 */
	myCamera.setLook();
	/** 绘制天空 */
	m_SkyBox.CreateSkyBox(-32, -32, -32, 0.25, 0.25, 0.25);
	
	/**绘制树 */
	drawtree(GL_FALSE);
	//glColor4f(0.0, 0.0, 0.0, 0.5);
	//glFlush();                   
	glutSwapBuffers();//交换两个缓冲区指针
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
	//实现树叶春夏生长变换
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
	//春夏秋冬四季变换
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);//GLUT_DOUBLE表示使用双缓冲窗口 GLUT_RGB颜色模式  GLUT_DEPTH深度缓存
	glutInitWindowPosition(200, 50);//设置初始窗口的位置(窗口左上角相对于桌面坐标(x, y))
	glutInitWindowSize(WindW, WindH);
	glutCreateWindow("第3组 L-system分形树");
	initParameter();
	char b[] = "res/树叶.bmp";
	LoadGLTextures(b, &texout, 0,0,255);
	texin = load_texture("res/bark1.bmp");  //加载纹理
	glutReshapeFunc(reshape);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMainLoop();
	system("pause");
	return 0;

}
