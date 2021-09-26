#include "SkyBox.h"
#include<iostream>
using namespace std;

CSkyBox::CSkyBox()
{
}

CSkyBox::~CSkyBox()
{
    /** ɾ�����������ռ�õ��ڴ� */
    for (int i = 0; i < 6; i++)
    {
        m_texture[i].FreeImage();
        glDeleteTextures(1, &m_texture[i].ID);
    }

}

/** ��պг�ʼ�� */
bool CSkyBox::Init()
{
    char filename[128];                                         /**< ���������ļ��� */

    const char* bmpName[] = { "back","front","bottom","top","right","left" };
    
    for (int i = 0; i < 6; i++)
    {
        sprintf(filename, "res/%s", bmpName[i]);
        strcat(filename, ".bmp");
        //cout << filename << endl;
        if (!m_texture[i].LoadBitmap(filename))                     /**< ����λͼ�ļ� */
        {
            cout << "װ��λͼ�ļ�ʧ�ܣ�" << endl;
            exit(0);
        }
        glGenTextures(1, &m_texture[i].ID);
        glBindTexture(GL_TEXTURE_2D, m_texture[i].ID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        /** �������� */
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_texture[i].imageWidth,
            m_texture[i].imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
            m_texture[i].image);
    }
    return true;

}

/** ������պ� */
void  CSkyBox::CreateSkyBox(float x, float y, float z,
    float box_width, float box_height,
    float box_length)
{
    /** ��ó����й���״̬ */
    GLboolean lp;
    glGetBooleanv(GL_LIGHTING, &lp);

    /** ������պг� �� �� */
    float width = MAP * box_width / 8;
    float height = MAP * box_height / 8;
    float length = MAP * box_length / 8;

    /** ������պ�����λ�� */
    //x = x + MAP / 8 - width / 2;
    //y = y + MAP / 24 - height / 2;
    //z = z + MAP / 8 - length / 2;

    glDisable(GL_LIGHTING);            /**< �رչ��� */

    /** ��ʼ���� */
    glPushMatrix();
    //glTranslatef(-x, -y, -z);
    glTranslatef(x, y, z);

    /** ���Ʊ��� */
    glBindTexture(GL_TEXTURE_2D, m_texture[0].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);

    glEnd();

    /** ����ǰ�� */
    glBindTexture(GL_TEXTURE_2D, m_texture[1].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z + length);

    glEnd();

    /** ���Ƶ��� */
    glBindTexture(GL_TEXTURE_2D, m_texture[2].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);

    glEnd();

    /** ���ƶ��� */
    glBindTexture(GL_TEXTURE_2D, m_texture[3].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);

    glEnd();

    /** �������� */
    glBindTexture(GL_TEXTURE_2D, m_texture[4].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + height, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + height, z + length);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + length);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z);

    glEnd();

    /** �������� */
    glBindTexture(GL_TEXTURE_2D, m_texture[5].ID);

    glBegin(GL_QUADS);

    /** ָ����������Ͷ������� */
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y, z + length);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z + length);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
    glEnd();

    glPopMatrix();                 /** ���ƽ��� */

    if (lp)                         /** �ָ�����״̬ */
        glEnable(GL_LIGHTING);

}