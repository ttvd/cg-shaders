 /*
 *  GlutApplication.h
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
 
#ifndef PROJECT_5900_GLUTAPPLICATION_H
#define PROJECT_5900_GLUTAPPLICATION_H

#include "Globals.h"

namespace Project5900
{
	class RenderTexture;
	
	class CgShaderNormal;
	class CgShaderPhong;
	class CgShaderCel;
	class CgShaderOutline;
	class CgShaderCelScreen;
	
	class GlutApplication
	{
		public:
		
			GlutApplication(int argc, char** argv);
			~GlutApplication();
			
		public:
		
			void								RenderRTBegin();
			void								RenderRTEnd();
			
		private:
		
			static void							CallbackDisplay();
			static void							CallbackIdle();
			static void							CallbackKeyboard(unsigned char ui8K, int i32X, int i32Y);
			static void							CallbackMotion(int i32X, int i32Y);
			static void							CallbackMouse(int i32Button, int i32State, int i32X, int i32Y);
			static void							CallbackMenu(int i32MenuValue);
			
		private:
		
			enum DisplayMode
			{
				DisplayCelScreen = 1,
				DisplayCelObject = 2,
				DisplayCel = 3,
				DisplayOutline = 4,
				DisplayPhong = 5,
				DisplayNormal = 6,
				DisplayDepth = 7
			};
			
			enum DisplayModel
			{
				DisplayTorus = 1,
				DisplayTeapot = 2
			};
			
		private:
		
			void								SetupRC();
			
			void								Display();
			void								Motion(int i32X, int i32Y);
			void								Mouse(int i32Button, int i32State, int i32X, int i32Y);
			void								Keyboard(unsigned char ui8K, int i32X, int i32Y);
			void								Menu(int i32Value);
			
		private:
		
			void								RenderGeometry();
		
			void								RenderBufferDepthNormal(RenderTexture* pBuffer);
			void								RenderBufferPhong(RenderTexture* pBuffer);
			void								RenderBufferCel(RenderTexture* pBuffer, bool bOutline = true);
			void								RenderBufferOutline(RenderTexture* pBuffer);
			void								RenderBufferCelScreen(RenderTexture* pBuffer);
			
			void								RenderPresentation();
			
			
		public:
		
			void								Run();


		private:
			
			RenderTexture*						m_pRTBuffer1;
			RenderTexture*						m_pRTBuffer2;
			RenderTexture*						m_pRTBuffer3;
			RenderTexture*						m_pRTBufferPresentation;
			
			CgShaderNormal*						m_pShaderNormal;
			CgShaderPhong*						m_pShaderPhong;
			CgShaderCel*						m_pShaderCel;
			CgShaderOutline*					m_pShaderOutline;
			CgShaderCelScreen*					m_pShaderCelScreen;
			
			CGcontext							m_cgContext;
			
			int									m_i32Width;
			int									m_i32Height;
			
			int									m_i32LastX;
			int									m_i32LastY;
			bool								m_bMouseMoving;
			
			float								m_f32RotationX;
			float								m_f32RotationY;
			
			static float						s_af3CameraPosition[3];
			
			GlutApplication::DisplayMode		m_enDisplayMode;
			GlutApplication::DisplayModel		m_enDisplayModel;
	
	};
	
}

#endif