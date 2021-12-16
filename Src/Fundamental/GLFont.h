//========================================================
/**
*  @file  GLFont.h
*
*  ��Ŀ������ OpenGLλͼ����
*  �ļ�����:  ������  
*  ����ƽ̨�� Windows98/2000/NT/XP
*  
*  ���ߣ�     BrightXu
*  �����ʼ�:  huoxini@hotmail.com
*  �������ڣ� 2006-09-13	
*  �޸����ڣ� 2006-11-21
*
*/     
//========================================================
#ifndef	__GLFONT_H__
#define	__GLFONT_H__
#include <windows.h>

#pragma warning(disable: 4267)  /**< ��ֹ��������������ת���ľ��� */

/** ����λͼ������ */
class GLFont															
{
public:
    /** ���캯������������ */
	GLFont();
	~GLFont();
    ///��Ա����
	bool InitFont(HDC hDC, wchar_t *fontName, int Size);  /**< ��ʼ������ */
	void PrintText(char *string, double x, double y);/**< ��λ��(x,y)�������ַ���string */
	void KillGLFont();  /**< ɾ������ */

protected:
	unsigned int listBase;  /**< �����������ʾ�б�Ŀ�ʼλ�� */
	
};

#endif	// __GLFONT_H__