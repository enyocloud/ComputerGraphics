#include "SkyBox.h"
#include<iostream>
using namespace std;

CSkyBox::CSkyBox()
{
}

CSkyBox::~CSkyBox()
{
    /** 删除纹理对象及其占用的内存 */
    for (int i = 0; i < 6; i++)
    {
        m_texture[i].FreeImage();
        glDeleteTextures(1, &m_texture[i].ID);
    }

}

/** 天空盒初始化 */
bool CSkyBox::Init()
{
    char filename[128];                                         /**< 用来保存文件名 */

    const char* bmpName[] = { "back","front","bottom","top","right","left" };
    
    for (int i = 0; i < 6; i++)
    {
        sprintf(filename, "res/%s", bmpName[i]);
        strcat(filename, ".bmp");
        //cout << filename << endl;
        if (!m_texture[i].LoadBitmap(filename))                     /**< 载入位图文件 */
        {
            cout << "装载位图文件失败！" << endl;
            exit(0);
        }
        glGenTextures(1, &m_texture[i].ID);
        glBindTexture(GL_TEXTURE_2D, m_texture[i].ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        /** 创建纹理 */
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_texture[i].imageWidth,
            m_texture[i].imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
            m_texture[i].image);
    }
    return true;

}

/** 构造天空盒 */
void  CSkyBox::CreateSkyBox(float x, float y, float z,
    float box_width, float box_height,
    float box_length)
{
    /** 获得场景中光照状态 */
    GLboolean lp;
    glGetBooleanv(GL_LIGHTING, &lp);

    /** 计算天空盒长 宽 高 */
    float width = MAP * box_width / 8;
    float height = MAP * box_height / 8;
    float length = MAP * box_length / 8;

    /** 计算天空盒中心位置 */
    //x = x + MAP / 8 - width / 2;
    //y = y + MAP / 24 - height / 2;
    //z = z + MAP / 8 - length / 2;

    glDisable(GL_LIGHTING);            /**< 关闭光照 */

    /** 开始绘制 */
    glPushMatrix();
    //glTranslatef(-x, -y, -z);
    glTranslatef(x, y, z);

    /** 绘制背面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[0].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);

    glEnd();

    /** 绘制前面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[1].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);

    glEnd();

    /** 绘制底面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[2].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);

    glEnd();

    /** 绘制顶面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[3].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);

    glEnd();

    /** 绘制左面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[4].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);

    glEnd();

    /** 绘制右面 */
    glBindTexture(GL_TEXTURE_2D, m_texture[5].ID);

    glBegin(GL_QUADS);

    /** 指定纹理坐标和顶点坐标 */
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glEnd();

    glPopMatrix();                 /** 绘制结束 */

    if (lp)                         /** 恢复光照状态 */
        glEnable(GL_LIGHTING);

}