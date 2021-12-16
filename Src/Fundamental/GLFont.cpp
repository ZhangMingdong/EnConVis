/**
*  @file  GLFont.cpp
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

#include<windows.h>              /**< ����windowsͷ�ļ� */
#include<gl/gl.h>                /**< ����glͷ�ļ� */
#include "GLFont.h"

GLFont::GLFont()
{
}
/** �������� */
GLFont::~GLFont()
{
	KillGLFont();/**< ����KillFont()ɾ������ */
}
/** ��ʼ������ */
bool GLFont::InitFont(HDC hDC, wchar_t *fontName, int Size)
{
   HFONT oldFont; /**< ����ɵ������� */
   HFONT hFont;  /**< ������ */
   /**< ����96����ʾ�б� */
   listBase = glGenLists(96);
   if(listBase == 0) /**< ����ʧ��,�򷵻� */
	   return false;
   
    /**< �������� */
    hFont = CreateFont(Size,					/**< ����߶� */
						0,						/**< ������ */
						0,						/**< �������ת�Ƕ� Angle Of Escapement */
						0,						/**< ������ߵ���ת�Ƕ�Orientation Angle */
						FW_BOLD,				/**< ��������� */
						FALSE,					/**< �Ƿ�ʹ��б�� */
						FALSE,					/**< �Ƿ�ʹ���»��� */
						FALSE,					/**< �Ƿ�ʹ��ɾ���� */
						ANSI_CHARSET,			/**< �����ַ��� */
						OUT_TT_PRECIS,			/**< ������� */
						CLIP_DEFAULT_PRECIS,	/**< �ü����� */
						ANTIALIASED_QUALITY,	/**< ������� */
				FF_DONTCARE|DEFAULT_PITCH,		/**< Family And Pitch */
						fontName);				/**< �������� */
	 if(!hFont)
		return false;  /**< ��������ʧ���򷵻� */
 
	oldFont = (HFONT)SelectObject(hDC, hFont); /**< ѡ��������Ҫ������ */
	wglUseFontBitmaps(hDC, 32, 96, listBase); /**< ����96����ʾ�б����ƴ�ASCII��Ϊ32-128���ַ� */
	//SelectObject(hDC,oldFont); /**< �ָ�ԭ�������� */
	//DeleteObject(hFont); /**< ɾ������ */
 
   return true;
}



/** ��ָ��λ������ַ��� */
void GLFont::PrintText(char *string, double x, double y)
{
   ///������.���listBaseΪ0��stringΪNULL���û���ֱ�ӷ���
   if((listBase == 0 || string == NULL))
      return ;

   glPushMatrix();
// 	   glDisable(GL_DEPTH_TEST); /**< �ر���Ȳ��� */
// 	   glDisable(GL_LIGHTING); /**< �رչ��� */

	   glRasterPos2f(x, y); /**< ����դλ���ƶ���ָ���� */
	   ///���ַ����������Ļ��
	   glPushAttrib(GL_LIST_BIT); /**< ����ʾ�б�����ѹ�����Զ�ջ */
		   glListBase(listBase - 32); /**< ������ʾ�б�Ļ�ֵ */
		   glCallLists(strlen(string), GL_UNSIGNED_BYTE, string); /**< ִ�ж����ʾ�б�����ַ��� */
	   glPopAttrib();  /**< ����ʾ�б����Ե������Զ�ջ */
	   ///�ָ�ԭ��״̬
// 	   glEnable(GL_LIGHTING); /**< ���ù��� */
// 	   glEnable(GL_DEPTH_TEST); /**< ������Ȳ��� */
   glPopMatrix();
 
}

/**< ɾ������ */
void GLFont::KillGLFont()
{
   /** ɾ��������ʾ�б� */
   if(listBase != 0)
      glDeleteLists(listBase, 96);
}

