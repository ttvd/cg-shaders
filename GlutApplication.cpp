 /*
 *  GlutApplication.cpp
 *  5900_Project, Assignment 02
 *
 *  Created by radix on 08/03/09.
 *  Copyright Mykola Konyk, <mykola@konyk.org>, 2011.
 *  
 *  This code is under Microsoft Reciprocal License (Ms-RL)
 *  Please see http://www.opensource.org/licenses/ms-rl.html
 *
 *  Important points about the license (from Ms-RL):
 *
 *  [A] For any file you distribute that contains code from the software (in source code or binary format), you must provide 
 *  recipients the source code to that file along with a copy of this license, which license will govern that file. 
 *  You may license other files that are entirely your own work and do not contain code from the software under any terms 
 *  you choose.
 *
 *  [B] No Trademark License- This license does not grant you rights to use any contributors' name, logo, or trademarks.
 *
 *  [C] If you bring a patent claim against any contributor over patents that you claim are infringed by the software, your 
 *  patent license from such contributor to the software ends automatically.
 *
 *  [D] If you distribute any portion of the software, you must retain all copyright, patent, trademark, and attribution notices 
 *  that are present in the software.
 *
 *  [E] If you distribute any portion of the software in source code form, you may do so only under this license by including a
 *  complete copy of this license with your distribution. If you distribute any portion of the software in compiled or object 
 *  code form, you may only do so under a license that complies with this license.
 *
 *  [F] The software is licensed "as-is." You bear the risk of using it. The contributors give no express warranties, guarantees 
 *  or conditions. You may have additional consumer rights under your local laws which this license cannot change. To the extent 
 *  permitted under your local laws, the contributors exclude the implied warranties of merchantability, fitness for a particular 
 *  purpose and non-infringement.
 *
 */

#include "GlutApplication.h"

#include "RenderTexture.h"

#include "CgShaderNormal.h"
#include "CgShaderPhong.h"
#include "CgShaderCel.h"
#include "CgShaderOutline.h"
#include "CgShaderCelScreen.h"

#include <iostream>

namespace Project5900
{
	//--
	GlutApplication*	g_pApplication;
	
	
	//--
	float	GlutApplication::s_af3CameraPosition[3] = {0.0f, 0.0f, 0.0f};
	
	
	//--
	GlutApplication::GlutApplication(int argc, char** argv) :
		m_i32Width(800),
		m_i32Height(800),
		m_bMouseMoving(false),
		m_i32LastX(0),
		m_i32LastY(0),
		m_f32RotationX(0.0f),
		m_f32RotationY(0.0f),
		m_enDisplayMode(GlutApplication::DisplayCelScreen),
		m_enDisplayModel(GlutApplication::DisplayTorus)
	{
		glutInit(&argc, argv);
		
		SetupRC();
		
		g_pApplication = this;
		
	}
	
	
	//--
	GlutApplication::~GlutApplication()
	{
		// [rad] Delete RTs
		delete(m_pRTBufferPresentation);
		delete(m_pRTBuffer1);
		
		// [rad] Delete shaders
		delete(m_pShaderNormal);
		delete(m_pShaderPhong);
		delete(m_pShaderCel);
		delete(m_pShaderOutline);
		delete(m_pShaderCelScreen);
	
		// [rad] Free resources allocated for Cg context
		cgDestroyContext(m_cgContext);
	}
	

	//--
	void 
	GlutApplication::SetupRC()
	{
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
		glutInitWindowSize(m_i32Width, m_i32Height);
		
		glutCreateWindow("GLUT Program");
		
		glutDisplayFunc(CallbackDisplay);
		glutIdleFunc(CallbackIdle);
		glutKeyboardFunc(CallbackKeyboard);
		glutMotionFunc(CallbackMotion);
		glutMouseFunc(CallbackMouse);
		
		glutCreateMenu(CallbackMenu);
		glutAddMenuEntry("[1] cel shading, screen space outline", GlutApplication::DisplayCelScreen);
		glutAddMenuEntry("[2] cel shading, object space outline", GlutApplication::DisplayCelObject);
		glutAddMenuEntry("[3] cel shading, no outline", GlutApplication::DisplayCel);
		glutAddMenuEntry("[4] outline, screen space", GlutApplication::DisplayOutline);
		glutAddMenuEntry("[5] phong shading", GlutApplication::DisplayPhong);
		glutAddMenuEntry("[6] normal buffer", GlutApplication::DisplayNormal);
		glutAddMenuEntry("[7] depth buffer", GlutApplication::DisplayDepth);
		glutAddMenuEntry("                                     ", 8);
		glutAddMenuEntry("[q] torus model", 9);
		glutAddMenuEntry("[w] teapot model", 10);
		
		glutAttachMenu(GLUT_RIGHT_BUTTON);
    	
		// [rad] Initialize Cg
		m_cgContext = cgCreateContext();
		cgSetParameterSettingMode(m_cgContext, CG_DEFERRED_PARAMETER_SETTING);
		
		// [rad] Create render textures (buffers)	
		m_pRTBufferPresentation = new RenderTexture(m_i32Width, m_i32Height);
		m_pRTBuffer1 = new RenderTexture(m_i32Width, m_i32Height);
		m_pRTBuffer2 = new RenderTexture(m_i32Width, m_i32Height);
		m_pRTBuffer3 = new RenderTexture(m_i32Width, m_i32Height);
		
		// [rad] Create Cg shaders
		m_pShaderNormal = new CgShaderNormal(m_cgContext);
		//m_pShaderNormal->CheckError();
		
		m_pShaderPhong = new CgShaderPhong(m_cgContext);
		//m_pShaderPhong->CheckError();
		
		m_pShaderCel = new CgShaderCel(m_cgContext);
		//m_pShaderCel->CheckError();
		
		m_pShaderOutline = new CgShaderOutline(m_cgContext);
		//m_pShaderOutline->CheckError();
		
		m_pShaderCelScreen = new CgShaderCelScreen(m_cgContext);
		//m_pShaderCelScreen->CheckError();
	}
	
	
	//--
	void
	GlutApplication::RenderGeometry()
	{		
		switch(m_enDisplayModel)
		{
			case GlutApplication::DisplayTeapot:
				{
					glutSolidTeapot(1.0f);
				}
				break;
				
			case GlutApplication::DisplayTorus:
				{
					glutSolidTorus(0.8f, 2.5f, 100, 500);
				}
				break;
		}
	}
	
	
	//--
	void
	GlutApplication::RenderBufferCelScreen(RenderTexture* pBuffer)
	{
		RenderBufferOutline(m_pRTBuffer2);
		RenderBufferCel(m_pRTBuffer3, false);
		
		pBuffer->RenderBegin();
		
			glViewport(0, 0, m_i32Width, m_i32Height);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glPushMatrix();
			glLoadIdentity();
			
			gluOrtho2D(0, m_i32Width, 0, m_i32Height);
			
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();	
			
			m_pShaderCelScreen->BindBegin();
			
			glBegin(GL_QUADS);
				
				glTexCoord2f(0.0, 0.0);
				glVertex2f(-1.0, -1.0);
				
				glTexCoord2f(1.0, 0.0);
				glVertex2f( 1.0, -1.0);
				
				glTexCoord2f(1.0, 1.0);
				glVertex2f( 1.0,  1.0);
				
				glTexCoord2f(0.0, 1.0);
				glVertex2f(-1.0,  1.0);
			glEnd();
				
			m_pShaderCelScreen->BindEnd();
			m_pShaderCelScreen->SetParamModelMatices();
			m_pShaderCelScreen->SetParamSamplerCel(m_pRTBuffer3->GetTexture(RenderTexture::Color));
			m_pShaderCelScreen->SetParamSamplerOutline(m_pRTBuffer2->GetTexture(RenderTexture::Color));
			m_pShaderCelScreen->SetParamSamplerSize(m_i32Width, m_i32Height);
				
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			
			glutSwapBuffers();
	
		pBuffer->RenderEnd();
	}
	
	
	
	//--
	void
	GlutApplication::RenderBufferOutline(RenderTexture* pBuffer)
	{
		RenderBufferDepthNormal(m_pRTBuffer1);
	
		pBuffer->RenderBegin();
		
			glViewport(0, 0, m_i32Width, m_i32Height);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glPushMatrix();
			glLoadIdentity();
			
			gluOrtho2D(0, m_i32Width, 0, m_i32Height);
			
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();	
			
			m_pShaderOutline->BindBegin();
			m_pShaderOutline->SetParamModelMatices();
			m_pShaderOutline->SetParamSamplerNormal(m_pRTBuffer1->GetTexture(RenderTexture::Color));
			m_pShaderOutline->SetParamSamplerDepth(m_pRTBuffer1->GetTexture(RenderTexture::Depth));
			m_pShaderOutline->SetParamSamplerSize(m_i32Width, m_i32Height);
			
			glBegin(GL_QUADS);
				
				glTexCoord2f(0.0, 0.0);
				glVertex2f(-1.0, -1.0);
				
				glTexCoord2f(1.0, 0.0);
				glVertex2f( 1.0, -1.0);
				
				glTexCoord2f(1.0, 1.0);
				glVertex2f( 1.0,  1.0);
				
				glTexCoord2f(0.0, 1.0);
				glVertex2f(-1.0,  1.0);
			glEnd();
				
			m_pShaderOutline->BindEnd();
							
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			
			glutSwapBuffers();
	
		pBuffer->RenderEnd();
	}
	
	
	//--
	void
	GlutApplication::RenderBufferDepthNormal(RenderTexture* pBuffer)
	{
		pBuffer->RenderBegin();
		
			glViewport(0, 0, m_i32Width, m_i32Height);
			glClearColor(1.0, 1.0, 1.0, 1.0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			
			gluPerspective(45.0f, (float) m_i32Width / (float) m_i32Height, 200.0f, 400.0f);
			
			glMatrixMode(GL_MODELVIEW);
			
			glLoadIdentity();	
			glPushMatrix();					
			
			glTranslatef(0, 0, -300.0f);
			
			glRotatef(m_f32RotationX, 0, 1, 0);
			glRotatef(m_f32RotationY, 1, 0, 0);
			
			glScalef(25.0f, 25.0f, 25.0f);
			
			m_pShaderNormal->BindBegin();
			
			m_pShaderNormal->SetParamModelMatices();
			
		    // [rad] Render our scene
			RenderGeometry();
			
			m_pShaderNormal->BindEnd();
			
			glDisable(GL_DEPTH_TEST);
		
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

		pBuffer->RenderEnd();
	}
	
	
	//--
	void
	GlutApplication::RenderBufferCel(RenderTexture* pBuffer, bool bOutline)
	{
		pBuffer->RenderBegin();
		
			glViewport(0, 0, m_i32Width, m_i32Height);
			glClearColor(1.0, 1.0, 1.0, 1.0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			
			gluPerspective(45.0f, (float) m_i32Width / (float) m_i32Height, 200.0f, 400.0f);
			
			glMatrixMode(GL_MODELVIEW);
			
			glLoadIdentity();	
			glPushMatrix();					
			
			glTranslatef(0, 0, -300.0f);
			
			glRotatef(m_f32RotationX, 0, 1, 0);
			glRotatef(m_f32RotationY, 1, 0, 0);
			
			glScalef(25.0f, 25.0f, 25.0f);
			
			m_pShaderCel->BindBegin();
			
			m_pShaderCel->SetParamModelMatices();
			m_pShaderCel->SetParamLightPosition(s_af3CameraPosition);
			m_pShaderCel->SetParamEyePosition(s_af3CameraPosition);		
			m_pShaderCel->SetParamComputeOutline(bOutline);
			
			// [rad] Render our scene
			RenderGeometry();
			
			m_pShaderCel->BindEnd();
			
			glDisable(GL_DEPTH_TEST);
		
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		
		pBuffer->RenderEnd();		
	}
	
	
	//--
	void
	GlutApplication::RenderBufferPhong(RenderTexture* pBuffer)
	{
		pBuffer->RenderBegin();
		
			glViewport(0, 0, m_i32Width, m_i32Height);
			glClearColor(1.0, 1.0, 1.0, 1.0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			
			gluPerspective(45.0f, (float) m_i32Width / (float) m_i32Height, 200.0f, 400.0f);
			
			glMatrixMode(GL_MODELVIEW);
			
			glLoadIdentity();	
			glPushMatrix();					
			
			glTranslatef(0, 0, -300.0f);
			
			glRotatef(m_f32RotationX, 0, 1, 0);
			glRotatef(m_f32RotationY, 1, 0, 0);
			
			glScalef(25.0f, 25.0f, 25.0f);
			
			m_pShaderPhong->BindBegin();
			
			m_pShaderPhong->SetParamModelMatices();
			m_pShaderPhong->SetParamLightPosition(s_af3CameraPosition);
			m_pShaderPhong->SetParamEyePosition(s_af3CameraPosition);		
			
			// [rad] Render our scene
			RenderGeometry();
			
			m_pShaderPhong->BindEnd();
			
			glDisable(GL_DEPTH_TEST);
		
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

		
		pBuffer->RenderEnd();	
	}
	
	
	//--
	void
	GlutApplication::RenderPresentation()
	{
		switch(m_enDisplayMode)
		{
			case GlutApplication::DisplayCelScreen:
				{
					RenderBufferCelScreen(m_pRTBufferPresentation);
				}
				break;
				
			case GlutApplication::DisplayCelObject:
				{
					RenderBufferCel(m_pRTBufferPresentation);
				}
				break;
				
			case GlutApplication::DisplayCel:
				{
					RenderBufferCel(m_pRTBufferPresentation, false);
				}
				break;
				
			case GlutApplication::DisplayOutline:
				{
					RenderBufferOutline(m_pRTBufferPresentation);
				}
				break;
				
			case GlutApplication::DisplayPhong:
				{
					RenderBufferPhong(m_pRTBufferPresentation);
				}
				break;
				
			case GlutApplication::DisplayNormal:
				{
					RenderBufferDepthNormal(m_pRTBufferPresentation);
				}
				break;
				
			case GlutApplication::DisplayDepth:
				{
					RenderBufferDepthNormal(m_pRTBufferPresentation);
				}
				break;
		}
	}
	
	
	//--
	void 
	GlutApplication::Display()
	{ 	
		RenderPresentation();
		
		glViewport(0, 0, m_i32Width, m_i32Height);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glPushMatrix();
		glLoadIdentity();
		
		gluOrtho2D(0, m_i32Width, 0, m_i32Height);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();	
				
		if(GlutApplication::DisplayDepth == m_enDisplayMode)
		{
			m_pRTBufferPresentation->BindBegin(RenderTexture::Depth);
		}
		else
		{
			m_pRTBufferPresentation->BindBegin(RenderTexture::Color);
		}
		
		glBegin(GL_QUADS);
			
			glTexCoord2f(0.0, 0.0);
			glVertex2f(-1.0, -1.0);
			
			glTexCoord2f(1.0, 0.0);
			glVertex2f( 1.0, -1.0);
			
			glTexCoord2f(1.0, 1.0);
			glVertex2f( 1.0,  1.0);
			
			glTexCoord2f(0.0, 1.0);
			glVertex2f(-1.0,  1.0);
		glEnd();
			
			
		m_pRTBufferPresentation->BindEnd();
			
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		
		glutSwapBuffers();
	}
	
	
	//--
	void 
	GlutApplication::Motion(int i32X, int i32Y)
	{
		if(m_bMouseMoving)
		{
			m_f32RotationX += static_cast<float>(m_i32LastX - i32X) * 0.55f;
			m_f32RotationY += static_cast<float>(m_i32LastY - i32Y) * 0.55f;
			
			m_i32LastX = i32X;
			m_i32LastY = i32Y;
		}
	}
	
	
	//--
	void 
	GlutApplication::Mouse(int i32Button, int i32State, int i32X, int i32Y)
	{
		if(i32Button == GLUT_LEFT_BUTTON && i32State == GLUT_DOWN) 
		{
			m_i32LastX = i32X;
			m_i32LastY = i32Y;
			
			m_bMouseMoving = true;
		}
		if(i32Button == GLUT_LEFT_BUTTON && i32State == GLUT_UP)
		{
			m_bMouseMoving = false;
		}
	}
	
	
	//--
	void
	GlutApplication::Keyboard(unsigned char ui8K, int i32X, int i32Y)
	{
		switch(ui8K) 
		{
			case '1':
				{
					m_enDisplayMode = GlutApplication::DisplayCelScreen;
				}
				break;
				
			case '2':
				{
					m_enDisplayMode = GlutApplication::DisplayCelObject;
				}
				break;
				
			case '3':
				{
					m_enDisplayMode = GlutApplication::DisplayCel;
				}
				break;
				
			case '4':
				{
					m_enDisplayMode = GlutApplication::DisplayOutline;
				}
				break;
				
			case '5':
				{
					m_enDisplayMode = GlutApplication::DisplayPhong;
				}
				break;
				
			case '6':
				{
					m_enDisplayMode = GlutApplication::DisplayNormal;
				}
				break;
				
			case '7':
				{
					m_enDisplayMode = GlutApplication::DisplayDepth;
				}
				break;
				
			case 'q':
				{
					m_enDisplayModel = GlutApplication::DisplayTorus;
				}
				break;
				
			case 'w':
				{
					m_enDisplayModel = GlutApplication::DisplayTeapot;
				}
				break;
		}
	}
	
	
	//--
	void
	GlutApplication::Menu(int i32Value)
	{
		if(i32Value < 8)
		{
			m_enDisplayMode = (GlutApplication::DisplayMode)i32Value;
		}
		else if(i32Value == 9)
		{
			m_enDisplayModel = GlutApplication::DisplayTorus;
		}
		else if(i32Value == 10)
		{
			m_enDisplayModel = GlutApplication::DisplayTeapot;
		}
	}
	
	
	//--
	void 
	GlutApplication::CallbackDisplay()
	{
		g_pApplication->Display();			
	}
	
	
	//--
	void 
	GlutApplication::CallbackKeyboard(unsigned char ui8K, int i32X, int i32Y)
	{
		g_pApplication->Keyboard(ui8K, i32X, i32Y);
	}
	
	
	//--
	void 
	GlutApplication::CallbackIdle()
	{
		glutPostRedisplay();
	}
	
	
	//--
	void 
	GlutApplication::CallbackMotion(int i32X, int i32Y)
	{
		g_pApplication->Motion(i32X, i32Y);
	}
	
	
	//--
	void 
	GlutApplication::CallbackMouse(int i32Button, int i32State, int i32X, int i32Y)
	{
		g_pApplication->Mouse(i32Button, i32State, i32X, i32Y);
	}


	//--
	void
	GlutApplication::CallbackMenu(int i32MenuValue)
	{
		g_pApplication->Menu(i32MenuValue);
	}
	
	//--
	void 
	GlutApplication::Run()
	{
		glutMainLoop();
	}
}