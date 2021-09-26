#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Camera.h" /**< ���������ͷ�ļ� */
#include<iostream>

using namespace std;

/** ���캯�� */
Camera::Camera()
{
    m_R = 50;
    /** ��ʼ������ֵ */
    Vector3 zero = Vector3(m_R, 0.0, 0.0);
    Vector3 view = Vector3(0.0, 0.0, 0.0);
    Vector3 up = Vector3(0.0, 1.0, 0.0);

    /** ��ʼ������� */
    m_Position = zero;
    m_View = view;
    m_UpVector = up;
    m_Speed = 6.0f;
}

Camera::~Camera()
{
}

/** �����������λ��,������������� */
void Camera::setCamera(float positionX, float positionY, float positionZ,
    float viewX, float viewY, float viewZ,
    float upVectorX, float upVectorY, float upVectorZ)
{
    /** �������� */
    Vector3 Position = Vector3(positionX, positionY, positionZ);
    Vector3 View = Vector3(viewX, viewY, viewZ);
    Vector3 UpVector = Vector3(upVectorX, upVectorY, upVectorZ);

    /** ��������� */
    m_Position = Position;
    m_View = View;
    m_UpVector = UpVector;
}

void Camera::moveCamera(float speed) {
    float R = m_R + speed * 0.5;
    if (R <= 2)
        R = 2.5;
    m_Position = m_Position * (R / m_R);
    m_R = R;
}

void Camera::yawCamera(float speed) {
    Vector3 eye = m_Position - m_View;
    float rad = speed * 3.14159 / 180.0f;//����ת�ǶȵĻ���
    Vector3 translate[3];
    translate[0] = Vector3(cosf(rad), 0, sinf(rad));
    translate[1] = Vector3(0, 1, 0);
    translate[2] = Vector3(-sinf(rad), 0, cosf(rad));
    Vector3 res;
    res.x = eye.x * translate[0].x + eye.y * translate[1].x + eye.z * translate[2].x;
    res.y = eye.x * translate[0].y + eye.y * translate[1].y + eye.z * translate[2].y;
    res.z = eye.x * translate[0].z + eye.y * translate[1].z + eye.z * translate[2].z;;
    m_Position = m_View + res;
}

void Camera::rotateCamera(float speed) {
    Vector3 eye = m_Position - m_View;
    float rad = speed * 3.14159 / 180.0f;//����ת�ǶȵĻ���
    Vector3 n = eye.crossProduct(m_UpVector).normalize();
    Vector3 translate[3];
    translate[0] = Vector3(n.x * n.x * (1 - cosf(rad)) + cosf(rad), n.x * n.y * (1 - cosf(rad)) + n.z * sinf(rad), n.x * n.z * (1 - cosf(rad)) - n.y * sinf(rad));
    translate[1] = Vector3(n.x * n.y * (1 - cosf(rad)) - n.z * sinf(rad), n.y * n.y * (1 - cosf(rad)) + cosf(rad), n.y * n.z * (1 - cosf(rad)) + n.x * sinf(rad));
    translate[2] = Vector3(n.x * n.z * (1 - cosf(rad)) + n.y * sinf(rad), n.y * n.z * (1 - cosf(rad)) - n.x * sinf(rad), n.z * n.z * (1 - cosf(rad)) + cosf(rad));
    Vector3 res;
    res.x = eye.x * translate[0].x + eye.y * translate[1].x + eye.z * translate[2].x;
    res.y = eye.x * translate[0].y + eye.y * translate[1].y + eye.z * translate[2].y;
    res.z = eye.x * translate[0].z + eye.y * translate[1].z + eye.z * translate[2].z;
    if (res.crossProduct(m_UpVector).dotProduct(n) < 0) {
        m_UpVector = m_UpVector * (-1.0f);
    }
    m_Position = m_View + res;
}


/** �����ӵ� */
void Camera::setLook()
{
    /** �����ӿ� */
    gluLookAt(m_Position.x, m_Position.y, m_Position.z,
        m_View.x, m_View.y, m_View.z,
        m_UpVector.x, m_UpVector.y, m_UpVector.z);
}

