 /*
 *  CgShaderCel.cpp
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

#include "CgShaderCel.h"

namespace Project5900
{
	//--
	CgShaderCel::CgShaderCel(CGcontext cgContext) : 
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
					out float4 outOPosition		: POSITION,					\
					out float4 outVPosition		: TEXCOORD0,				\
					out float4 outNormal		: TEXCOORD1,				\
																			\
					uniform float4x4 uniModelViewProjMat,					\
					uniform float4x4 uniModelViewITMat,						\
					uniform float4x4 uniModelViewMat)						\
		{																	\
			outOPosition = mul(uniModelViewProjMat, inPosition);			\
			outVPosition = mul(uniModelViewMat, inPosition);				\
																			\
			outNormal = mul(uniModelViewITMat, inNormal);					\
			outNormal.xyz = normalize(outNormal.xyz);						\
		}																	";
		
		// [rad] Setup vertex shader program
		SetProgram(CgShader::Vertex, sProgramVertex);
		
		// [rad] Setup fragment shader:
		
		// [rad] Setup fragment shader entry point
		SetEntry(CgShader::Fragment, "main");
		
		static const std::string sProgramFragment = "						\
																			\
		void main(															\
					float4 inPosition					: TEXCOORD0,		\
					float4 inNormal						: TEXCOORD1,		\
																			\
					out float4 outColor					: COLOR,			\
																			\
					uniform sampler1D uniTexDiffuse,						\
					uniform sampler1D uniTexSpecular,						\
					uniform sampler1D uniTexOutline,						\
																			\
					uniform float3 uniMatDiffuse,							\
					uniform float3 uniMatSpecular,							\
					uniform float3 uniLightColor,							\
					uniform float3 uniLightPosition,						\
					uniform float3 uniEyePosition,							\
					uniform float uniShininess,								\
																			\
					uniform int uniComputeOutline)							\
		{																	\
																			\
			float3 p = inPosition.xyz;										\
			float3 n = normalize(inNormal.xyz);								\
																			\
			float3 l = normalize(uniLightPosition - p);						\
			float diffuseL = max(dot(n, l), 0);								\
			diffuseL = tex1D(uniTexDiffuse, diffuseL).x;					\
			float3 diffuse = uniLightColor * diffuseL * uniMatDiffuse;		\
																			\
			float3 v = normalize(uniEyePosition - p);						\
			float3 h = normalize(l + v);									\
			float specularL = pow(max(dot(n, h), 0), uniShininess);			\
			if(diffuseL <= 0) specularL = 0;								\
			specularL = tex1D(uniTexSpecular, specularL).x;					\
			float3 specular = uniLightColor * specularL * uniMatSpecular;	\
																			\
			float edgeL = 1;												\
																			\
			if(uniComputeOutline)											\
			{																\
				edgeL = max(dot(n, v), 0);									\
				edgeL = tex1D(uniTexOutline, edgeL).x;						\
			}																\
																			\
			outColor.x = edgeL * (diffuse + specular);						\
			outColor.y = edgeL * (diffuse + specular);						\
			outColor.z = edgeL * (diffuse + specular);						\
			outColor.w = 1;													\
		}																	";

		// float3 diffuse = diffuseL * uniMatDiffuse;
		// float3 specular = specularL * uniMatSpecular;
		
		// [rad] Setup fragment shader program
		SetProgram(CgShader::Fragment, sProgramFragment);
		
		// [rad] Create shaders (both fragment and vertex)
		Create();
		
		// [rad] Set params - vertex shader
		m_cgParamModelViewProjMatrix = cgGetNamedParameter(m_cgShaderVertex, "uniModelViewProjMat");
		m_cgParamModelViewMatrix = cgGetNamedParameter(m_cgShaderVertex, "uniModelViewMat");
		m_cgParamModelViewITMatrix = cgGetNamedParameter(m_cgShaderVertex, "uniModelViewITMat");
		
		// [rad] Set params - fragment shader				
		m_cgParamLightColorVector = cgGetNamedParameter(m_cgShaderFragment, "uniLightColor");
		m_cgParamLightPositionVector = cgGetNamedParameter(m_cgShaderFragment, "uniLightPosition");	
		
		m_cgParamEyePositionVector = cgGetNamedParameter(m_cgShaderFragment, "uniEyePosition");	
		
		m_cgParamShininess = cgGetNamedParameter(m_cgShaderFragment, "uniShininess");	
		
		m_cgParamMatDiffuseVector = cgGetNamedParameter(m_cgShaderFragment, "uniMatDiffuse");	
		m_cgParamMatSpecularVector = cgGetNamedParameter(m_cgShaderFragment, "uniMatSpecular");
		
		m_cgParamSamplerDiffuse = cgGetNamedParameter(m_cgShaderFragment, "uniTexDiffuse");	
		m_cgParamSamplerSpecular = cgGetNamedParameter(m_cgShaderFragment, "uniTexSpecular");	
		m_cgParamSamplerOutline = cgGetNamedParameter(m_cgShaderFragment, "uniTexOutline");	
		
		m_cgParamComputeOutline = cgGetNamedParameter(m_cgShaderFragment, "uniComputeOutline");	
		
		// [rad] Manually assign other (material) related params
		cgSetParameter1f(m_cgParamShininess, 128.0f);
	
		static const float af3ColorDiffuse[3] = {0.8f, 0.8f, 0.8f};
		cgSetParameter3fv(m_cgParamMatDiffuseVector, af3ColorDiffuse); 
		
		static const float af3ColorSpecular[3] = {1.0f, 1.0f, 1.0f};
		cgSetParameter3fv(m_cgParamMatSpecularVector, af3ColorSpecular);
		
		static const float af3Color[3] = {1.0f, 1.0f, 1.0f};
		cgSetParameter3fv(m_cgParamLightColorVector, af3Color);
		
		// [rad] Set texture params
		GenerateTextures();
		
		cgGLSetTextureParameter(m_cgParamSamplerDiffuse, m_glIDTextureDiffuse);
		cgGLSetTextureParameter(m_cgParamSamplerSpecular, m_glIDTextureSpecular);
		cgGLSetTextureParameter(m_cgParamSamplerOutline, m_glIDTextureOutline);
		
	}
	
	
	//--
	CgShaderCel::~CgShaderCel()
	{
		// [rad] Delete used textures
		glDeleteTextures(1, &m_glIDTextureDiffuse);
		glDeleteTextures(1, &m_glIDTextureSpecular);
		glDeleteTextures(1, &m_glIDTextureOutline);
	}
	
	
	//--
	void
	CgShaderCel::SetParamModelMatices()
	{
		cgGLSetStateMatrixParameter(m_cgParamModelViewProjMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
		cgGLSetStateMatrixParameter(m_cgParamModelViewMatrix, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
		cgGLSetStateMatrixParameter(m_cgParamModelViewITMatrix, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	}
	
	
	//--
	void
	CgShaderCel::SetParamLightPosition(const float* pVec)
	{
		cgSetParameter3fv(m_cgParamLightPositionVector, pVec); 
	}
	
	
	//--
	void
	CgShaderCel::SetParamEyePosition(const float* pVec)
	{
		cgSetParameter3fv(m_cgParamEyePositionVector, pVec); 
	}
	
	
	//--
	void
	CgShaderCel::SetParamLightColor(const float* pVec)
	{
		cgSetParameter3fv(m_cgParamLightColorVector, pVec); 
	}
	
	
	//--
	void
	CgShaderCel::SetParamComputeOutline(bool bEnabled)
	{
		if(bEnabled)
		{
			cgSetParameter1i(m_cgParamComputeOutline, 1);
		}
		else
		{
			cgSetParameter1i(m_cgParamComputeOutline, 0);
		}
	}
	
	
	//--
	void
	CgShaderCel::GenerateTextures()
	{		
		float afBuf[256];
		
		// [rad] Create/Generate diffuse 1D texture
		glGenTextures(1, &m_glIDTextureDiffuse);
		glBindTexture(GL_TEXTURE_1D, m_glIDTextureDiffuse);
		
		for(int iIndex = 0; iIndex < 256; iIndex++)
		{	
			if(iIndex < 118)
			{
				afBuf[iIndex] = 75.0f / 255.0f;
				//afBuf[iIndex] = 125.0f / 255.0f;
			}
			else if(iIndex < 180)
			{
				afBuf[iIndex] = 150.0f / 255.0f;
				//afBuf[iIndex] = 200.0f / 255.0f;
			}
			else if(iIndex < 230)
			{
				afBuf[iIndex] = 170.0f / 255.0f;
				//afBuf[iIndex] = 220.0f / 255.0f;
			}
			else
			{
				afBuf[iIndex] = 250.0f / 255.0f;
				//afBuf[iIndex] = 250.0f / 255.0f;
			}
		}
		
		glTexImage1D(GL_TEXTURE_1D, 0, GL_INTENSITY16, 256, 0, GL_LUMINANCE, GL_FLOAT, &afBuf);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_1D, 0);
		
		// [rad] Create/Generate specular 1D texture
		glGenTextures(1, &m_glIDTextureSpecular);
		glBindTexture(GL_TEXTURE_1D, m_glIDTextureSpecular);
				
		for(int iIndex = 0; iIndex < 256; iIndex++)
		{
			//if(iIndex < 192)
			if(iIndex < 128)
			{
				afBuf[iIndex] = 0.0f;
			}
			else
			{
				afBuf[iIndex] = 1.0f;
			}
			
		}
		
		glTexImage1D(GL_TEXTURE_1D, 0, GL_INTENSITY16, 256, 0, GL_LUMINANCE, GL_FLOAT, &afBuf);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_1D, 0);
		
		// [rad] Create/Generate edge 1D texture
		glGenTextures(1, &m_glIDTextureOutline);
		glBindTexture(GL_TEXTURE_1D, m_glIDTextureOutline);
				
		for(int iIndex = 0; iIndex < 256; iIndex++)
		{
			if(iIndex < 50)
			{
				afBuf[iIndex] = 0.0f;
			}
			else
			{
				afBuf[iIndex] = 1.0f;
			}
		}
		
		glTexImage1D(GL_TEXTURE_1D, 0, GL_INTENSITY16, 256, 0, GL_LUMINANCE, GL_FLOAT, &afBuf);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_1D, 0);
		
	}
	
	
	//--
	void
	CgShaderCel::BindBegin()
	{
		CgShader::BindBegin();
		
		cgGLEnableTextureParameter(m_cgParamSamplerDiffuse);
		cgGLEnableTextureParameter(m_cgParamSamplerSpecular);		
		cgGLEnableTextureParameter(m_cgParamSamplerOutline);
	}
	
	
	//--
	void
	CgShaderCel::BindEnd()
	{
		CgShader::BindEnd();
		
		cgGLDisableTextureParameter(m_cgParamSamplerOutline);
		cgGLDisableTextureParameter(m_cgParamSamplerSpecular);
		cgGLDisableTextureParameter(m_cgParamSamplerDiffuse);	
	}
}