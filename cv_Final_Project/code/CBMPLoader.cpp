#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include<iostream>
#include<errno.h>
using namespace std;
#include"CBMPLoader.h" 

#define BITMAP_ID 0x4D42

/** ���캯�� */
CBMPLoader::CBMPLoader()
{
    /** ��ʼ����ԱֵΪ0 */
    ID = 0;
    image = 0;
    imageWidth = 0;
    imageHeight = 0;
}

/** �������� */
CBMPLoader::~CBMPLoader()
{
    FreeImage(); /**< �ͷ�ͼ������ռ�ݵ��ڴ� */
}

/** װ��һ��λͼ�ļ� */
bool CBMPLoader::LoadBitmapW(const char* file)
{
    FILE* pFile; /**< �ļ�ָ�� */

    /** ����λͼ�ļ���Ϣ��λͼ�ļ�ͷ�ṹ */
    BITMAPINFOHEADER bitmapInfoHeader;
    BITMAPFILEHEADER header;

    unsigned char textureColors = 0;/**< ���ڽ�ͼ����ɫ��BGR�任��RGB */

   /** ���ļ�,�������� */
    pFile = fopen(file, "rb");
    if (!pFile) {
        cout << file << endl;
        cout << pFile << endl;
        cout << errno<<strerror(errno) << endl;
        cout << "�ļ��򿪴���" << endl;
        fclose(pFile);
        return false;
    }
    /** ����λͼ�ļ�ͷ��Ϣ */
    fread(&header, sizeof(BITMAPFILEHEADER), 1, pFile);

    /** �����ļ��Ƿ�Ϊλͼ�ļ� */
    if (header.bfType != BITMAP_ID)
    {
        fclose(pFile);             /**< ������λͼ�ļ�,��ر��ļ������� */
        return false;
    }

    /** ����λͼ�ļ���Ϣ */
    fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

    /** ����ͼ��Ŀ�Ⱥ͸߶� */
    imageWidth = bitmapInfoHeader.biWidth;
    imageHeight = bitmapInfoHeader.biHeight;

    /** ȷ����ȡ���ݵĴ�С */
    if (bitmapInfoHeader.biSizeImage == 0)
        bitmapInfoHeader.biSizeImage = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3;

    /** ��ָ���Ƶ����ݿ�ʼλ�� */
    fseek(pFile, header.bfOffBits, SEEK_SET);

    /** �����ڴ� */
    image = new unsigned char[bitmapInfoHeader.biSizeImage];

    /** ����ڴ�����Ƿ�ɹ� */
    if (!image)                        /**< �������ڴ�ʧ���򷵻� */
    {
        delete[] image;
        fclose(pFile);
        return false;
    }

    /** ��ȡͼ������ */
    fread(image, 1, bitmapInfoHeader.biSizeImage, pFile);

    /** ��ͼ����ɫ���ݸ�ʽ���н���,��BGRת��ΪRGB */
    for (int index = 0; index < (int)bitmapInfoHeader.biSizeImage; index += 3)
    {
        textureColors = image[index];
        image[index] = image[index + 2];
        image[index + 2] = textureColors;
    }

    fclose(pFile);
    return true;
}

/** �ͷ��ڴ� */
void CBMPLoader::FreeImage()
{
    /** �ͷŷ�����ڴ� */
    if (image)
    {
        delete[] image;
        image = 0;
    }
}