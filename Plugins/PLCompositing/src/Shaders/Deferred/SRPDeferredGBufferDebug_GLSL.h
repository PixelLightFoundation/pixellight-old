/*********************************************************\
 *  File: SRPDeferredGBufferDebug_GLSL.h                 *
 *
 *  Copyright (C) 2002-2011 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  PixelLight is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PixelLight is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with PixelLight. If not, see <http://www.gnu.org/licenses/>.
\*********************************************************/


// OpenGL 2.0 ("#version 110") GLSL vertex shader source code, "#version" is added by "PLRenderer::ProgramGenerator"
static const PLCore::String sDeferredGBufferDebug_GLSL_VS = "\
// Attributes\n\
attribute vec4 VertexPosition;		// Clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)\n\
									// zw = Vertex texture coordinate, lower/left is (0,0) and upper/right is (1,1)\n\
varying   vec2 VertexTexCoordVS;	// Vertex texture coordinate 0 output\n\
\n\
// Uniforms\n\
uniform ivec2 TextureSize;	// Texture size in texel\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Set the clip space vertex position\n\
	gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);\n\
\n\
	// Pass through the scaled vertex texture coordinate\n\
	VertexTexCoordVS = VertexPosition.zw*vec2(TextureSize);\n\
}";


// OpenGL 2.0 ("#version 110") GLSL fragment shader source code, "#version" is added by "PLRenderer::ProgramGenerator"
static const PLCore::String sDeferredGBufferDebug_GLSL_FS = "\
// GLSL extensions\n\
#extension GL_ARB_texture_rectangle : enable\n\
\n\
// Attributes\n\
varying vec2 VertexTexCoordVS;	// Vertex texture coordinate input from vertex shader\n\
\n\
// Uniforms\n\
uniform float			NearPlane;	// Camera near plane distance\n\
uniform float			Range;		// Distance between camera far and new plane (never 0!)\n\
uniform sampler2DRect	Map;		// Input texture containing the data to visualize\n\
\n\
// Programs\n\
// Decodes a 2 component normal vector to a 3 component normal vector\n\
vec3 decodeNormalVector(vec2 normal)\n\
{\n\
	vec2 fenc = normal*4.0 - 2.0;\n\
	float f = dot(fenc, fenc);\n\
	float g = sqrt(1.0 - f/4.0);\n\
	vec3 n;\n\
	n.xy = fenc*g;\n\
	n.z = 1.0 - f/2.0;\n\
	return n;\n\
}\n\
\n\
// Program entry point\n\
void main()\n\
{\n\
	// Fetch the required texel data and set the RGB output color\n\
#ifdef FS_SHOW_BLACK\n\
	gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);\n\
#elif defined FS_SHOW_ALBEDO\n\
	gl_FragColor.rgb = texture2DRect(Map, VertexTexCoordVS).rgb;\n\
#elif defined FS_SHOW_AMBIENTOCCLUSION\n\
	gl_FragColor.rgb = texture2DRect(Map, VertexTexCoordVS).aaa;\n\
#elif defined FS_SHOW_NORMALS\n\
	gl_FragColor.rgb = decodeNormalVector(texture2DRect(Map, VertexTexCoordVS).rg);\n\
#elif defined FS_SHOW_DEPTH\n\
	// Normalized the linear view space depth, else we can't inspect the depth properly\n\
	float value = (texture2DRect(Map, VertexTexCoordVS).b - NearPlane)/Range;\n\
	gl_FragColor.rgb = vec3(value, value, value);\n\
#elif defined FS_SHOW_SPECULAR_COLOR\n\
	gl_FragColor.rgb = texture2DRect(Map, VertexTexCoordVS).rgb;\n\
#elif defined FS_SHOW_SPECULAR_EXPONENT\n\
	float value = texture2DRect(Map, VertexTexCoordVS).a/128.0;\n\
	gl_FragColor.rgb = vec3(value, value, value);\n\
#elif defined FS_SHOW_SELFILLUMINATION\n\
	gl_FragColor.rgb = texture2DRect(Map, VertexTexCoordVS).rgb;\n\
#elif defined FS_SHOW_GLOW\n\
	gl_FragColor.rgb = texture2DRect(Map, VertexTexCoordVS).aaa;\n\
#else\n\
	// Invalid state, should never ever happen!\n\
	gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);\n\
#endif\n\
\n\
	// Alpha is always 1\n\
	gl_FragColor.a = 1.0;\n\
}";