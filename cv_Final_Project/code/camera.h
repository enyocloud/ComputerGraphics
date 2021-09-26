#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <windows.h>
#include<gl/glu.h>                     /**< ����glͷ�ļ� */
#include "Vector.h"                 /**< ����������ͷ�ļ� */

/** ������� */
class Camera
{
public:
    /** ���캯������������ */
    Camera();
    ~Camera();

    /** ��������״̬���� */
    Vector3 getPosition() { return m_Position; }
    Vector3 getView() { return m_View; }
    Vector3 getUpVector() { return m_UpVector; }
    float   getSpeed() { return m_Speed; }

    /** �����ٶ� */
    void setSpeed(float speed)
    {
        m_Speed = speed;
    }

    /** �����������λ��, �۲����������� */
    void setCamera(float positionX, float positionY, float positionZ,
        float viewX, float viewY, float viewZ,
        float upVectorX, float upVectorY, float upVectorZ);

    /** ����������ƶ� */
    void yawCamera(float speed);

    /** �����ƶ������ */
    void rotateCamera(float speed);

    /** ǰ���ƶ������ */
    void moveCamera(float speed);

    /** ��������� */
    void setLook();

private:
    /** ��������� */
    Vector3        m_Position;      /**< λ�� */
    Vector3        m_View;          /**< ���� */
    Vector3        m_UpVector;      /**< �������� */
    float          m_Speed;         /**< �ٶ� */
    float          m_R;

};

#endif //__CAMERA_H__