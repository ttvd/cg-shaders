 /*
 *  Shader.cpp
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

#include "CgShader.h"

#include <iostream>

namespace Project5900
{
	//--
	CgShader::CgShader(CGcontext cgContext, CgShader::Type enType) :
		m_cgContext(cgContext),
		m_sProgramVertex(""),
		m_sProgramFragment(""),
		m_sEntryFragment("main"),
		m_sEntryVertex("main"),
		m_enType(enType)
	{
		switch(enType)
		{
			case CgShader::Vertex :
				{
					m_cgProfileVertex = cgGLGetLatestProfile(CG_GL_VERTEX);
					cgGLSetOptimalOptions(m_cgProfileVertex);
				}
				break;
				
			case CgShader::Fragment :
				{
					m_cgProfileFragment = cgGLGetLatestProfile(CG_GL_FRAGMENT);
					cgGLSetOptimalOptions(m_cgProfileFragment);
				}
				break;
				
			case VertexFragment :
				{
					m_cgProfileVertex = cgGLGetLatestProfile(CG_GL_VERTEX);
					cgGLSetOptimalOptions(m_cgProfileVertex);
					
					m_cgProfileFragment = cgGLGetLatestProfile(CG_GL_FRAGMENT);
					cgGLSetOptimalOptions(m_cgProfileFragment);
				}
				break;			
				
			default:
				{
					m_enType = CgShader::Invalid;
				}
		}
	}
	
	
	//--
	CgShader::~CgShader()
	{
		// [rad] Free resources allocated for this shader		 
		switch(m_enType)
		{
			case CgShader::Vertex :
				{
					cgDestroyProgram(m_cgShaderVertex);
				}
				break;
				
			case CgShader::Fragment :
				{
					cgDestroyProgram(m_cgShaderFragment);
				}
				break;
				
			case VertexFragment :
				{
					cgDestroyProgram(m_cgShaderVertex);
					cgDestroyProgram(m_cgShaderFragment);
				}
				break;			
		}
	}
	
	
	//--
	void
	CgShader::CheckError() const
	{
		CGerror cgError;
		const char* pString = cgGetLastErrorString(&cgError);

		if(cgError != CG_NO_ERROR) 
		{
			std::cout << pString << std::endl;
			
			if(cgError == CG_COMPILER_ERROR) 
			{
				std::cout << cgGetLastListing(m_cgContext) << std::endl;
			}
			
			exit(1);
		}
	}
	
	
	//--
	void
	CgShader::SetProgram(const std::string& refProgram)
	{
		SetProgram(m_enType, refProgram);
	}
	
	
	//--
	void
	CgShader::SetProgram(CgShader::Type enType, const std::string& refProgram)
	{
		switch(enType)
		{
			case CgShader::Vertex :
				{
					m_sProgramVertex = refProgram;
				}
				break;
				
			case CgShader::Fragment :
				{
					m_sProgramFragment = refProgram;
				}
				break;								
		}
	}
	
	
	//--
	void
	CgShader::SetEntry(const std::string& refEntry)
	{
		SetEntry(m_enType, refEntry);
	}
	
	
	//--
	void
	CgShader::SetEntry(CgShader::Type enType, const std::string& refEntry)
	{
		switch(enType)
		{
			case CgShader::Vertex :
				{
					m_sEntryVertex = refEntry;
				}
				break;
				
			case CgShader::Fragment :
				{
					m_sEntryFragment = refEntry;
				}
				break;								
		}
	}
	
	
	//--
	void
	CgShader::Create()
	{
		switch(m_enType)
		{
			case CgShader::Vertex :
				{
					m_cgShaderVertex = cgCreateProgram(m_cgContext, CG_SOURCE, m_sProgramVertex.c_str(), m_cgProfileVertex, m_sEntryVertex.c_str(), 0);
					cgGLLoadProgram(m_cgShaderVertex);
				}
				break;
				
			case CgShader::Fragment :
				{
					m_cgShaderFragment = cgCreateProgram(m_cgContext, CG_SOURCE, m_sProgramFragment.c_str(), m_cgProfileFragment, m_sEntryFragment.c_str(), 0);
					cgGLLoadProgram(m_cgShaderFragment);
				}
				break;
				
			case VertexFragment :
				{
					m_cgShaderVertex = cgCreateProgram(m_cgContext, CG_SOURCE, m_sProgramVertex.c_str(), m_cgProfileVertex, m_sEntryVertex.c_str(), 0);
					cgGLLoadProgram(m_cgShaderVertex);
					
					m_cgShaderFragment = cgCreateProgram(m_cgContext, CG_SOURCE, m_sProgramFragment.c_str(), m_cgProfileFragment, m_sEntryFragment.c_str(), 0);
					cgGLLoadProgram(m_cgShaderFragment);
				}
				break;			
		}
	}
	
	
	//--
	void
	CgShader::BindBegin()
	{
		switch(m_enType)
		{
			case CgShader::Vertex :
				{
					cgGLEnableProfile(m_cgProfileVertex);
					cgGLBindProgram(m_cgShaderVertex);
				}
				break;
				
			case CgShader::Fragment :
				{
					cgGLEnableProfile(m_cgProfileFragment);
					cgGLBindProgram(m_cgShaderFragment);
				}
				break;
				
			case VertexFragment :
				{
					cgGLEnableProfile(m_cgProfileVertex);
					cgGLBindProgram(m_cgShaderVertex);
					
					cgGLEnableProfile(m_cgProfileFragment);
					cgGLBindProgram(m_cgShaderFragment);
				}
				break;			
		}

	}
	
	
	//--
	void
	CgShader::BindEnd()
	{
		switch(m_enType)
		{
			case CgShader::Vertex :
				{
					cgGLDisableProfile(m_cgProfileVertex);
				}
				break;
				
			case CgShader::Fragment :
				{
					cgGLDisableProfile(m_cgProfileFragment);
				}
				break;
				
			case VertexFragment :
				{
					cgGLDisableProfile(m_cgProfileVertex);
					
					cgGLDisableProfile(m_cgProfileFragment);
				}
				break;			
		}
	}
}