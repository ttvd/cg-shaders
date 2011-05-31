 /*
 *  RenderTexture.cpp
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

#include "RenderTexture.h"
#include <iostream>

namespace Project5900
{
	//--
	RenderTexture::RenderTexture(int i32Width, int i32Height) :
		m_i32Width(i32Width),
		m_i32Height(i32Height)
	{
		// [rad] Texture - color
		glGenTextures(1, &m_glIDTextureColor);
		glBindTexture(GL_TEXTURE_2D, m_glIDTextureColor);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_i32Width, m_i32Height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_i32Width, m_i32Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);
		
			
		// [rad] Texture - depth
		glGenTextures(1, &m_glIDTextureDepth);
		glBindTexture(GL_TEXTURE_2D, m_glIDTextureDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_i32Width, m_i32Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_ARB, m_i32Width, m_i32Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
				
		
		// [rad] FBO
		glGenFramebuffersEXT(1, &m_glFBO);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_glFBO);
		glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, m_glIDTextureColor, 0);
		glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_glIDTextureDepth, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	
	
	//--
	RenderTexture::~RenderTexture()
	{
		glDeleteTextures(1, &m_glIDTextureColor);
		glDeleteTextures(1, &m_glIDTextureDepth);
		
		glDeleteFramebuffersEXT(1, &m_glFBO);
	}
	
	
	//--
	void 
	RenderTexture::RenderBegin()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_glFBO);
	}
	
	
	//--
	void 
	RenderTexture::RenderEnd()
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	
	
	//--
	void 
	RenderTexture::BindBegin(RenderTexture::Type enType)
	{
		GLuint glTex;
		
		switch(enType)
		{
			case Depth:
				{
					glTex = m_glIDTextureDepth;
				}
				break;
				
			case Color:
			default:
				{
					glTex = m_glIDTextureColor;
				}
		}
				
		glBindTexture(GL_TEXTURE_2D, glTex);
		glEnable(GL_TEXTURE_2D);
	}
	
	
	//--
	void 
	RenderTexture::BindEnd()
	{
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	
	//--
	GLuint
	RenderTexture::GetTexture(RenderTexture::Type enType)
	{
		if(RenderTexture::Depth == enType)
		{
			return(m_glIDTextureDepth);
		}
		
		return(m_glIDTextureColor);
	}
}