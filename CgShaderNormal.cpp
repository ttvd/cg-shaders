 /*
 *  CgShaderNormal.cpp
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

#include "CgShaderNormal.h"

namespace Project5900
{
	//--
	CgShaderNormal::CgShaderNormal(CGcontext cgContext) : 
		CgShader(cgContext, CgShader::VertexFragment)
	{
		// [rad] This is vertex and fragment shader
	
		// [rad] Setup vertex shader:
		
		// [rad] Setup vertex shader entry point
		SetEntry(CgShader::Vertex, "main");

		static const std::string sProgramVertex = "							\
																			\
		void main(	float4 inPosition			: POSITION,					\
					float4 inNormal				: NORMAL,					\
																			\
					out float4 outPosition		: POSITION,					\
					out float4 outNormal		: TEXCOORD0,				\
																			\
					uniform float4x4 uniModelViewProjMat)					\
		{																	\
			outPosition = mul(uniModelViewProjMat, inPosition);				\
																			\
			outNormal = inNormal;											\
		}																	";
		
		// [rad] Setup vertex shader program
		SetProgram(CgShader::Vertex, sProgramVertex);
		
		// [rad] Setup fragment shader:
		
		// [rad] Setup fragment shader entry point
		SetEntry(CgShader::Fragment, "main");
		
		static const std::string sProgramFragment = "						\
																			\
		void main(	float4 inNormal		: TEXCOORD0,						\
																			\
					out float4 outColor : COLOR)							\
		{																	\
			float4 n = normalize(inNormal);									\
																			\
			n.x = (n.x + 1.0) * 0.5;										\
			n.y = (n.y + 1.0) * 0.5;										\
			n.z = (n.z + 1.0) * 0.5;										\
			n.w = 1;														\
																			\
			outColor = n;													\
		}																	";
		
		// [rad] Setup fragment shader program
		SetProgram(CgShader::Fragment, sProgramFragment);
		
		// [rad] Create shaders (both fragment and vertex)
		Create();
		
		// [rad] Set params 
		m_cgParamModelViewProjMatrix = cgGetNamedParameter(m_cgShaderVertex, "uniModelViewProjMat");	
	}
	
	
	//--
	void
	CgShaderNormal::SetParamModelMatices()
	{
		cgGLSetStateMatrixParameter(m_cgParamModelViewProjMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	}
}