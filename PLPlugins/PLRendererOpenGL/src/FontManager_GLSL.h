/*********************************************************\
 *  File: FontManager_GLSL.h                             *
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


//  OpenGL 2.0 ("#version 110") GLSL vertex shader source code, "#version" is added by "PLRenderer::ProgramGenerator"
static const PLGeneral::String sVertexShaderSourceCodeGLSL = "\
// Attributes\n\
attribute vec3 VertexPosition;		// Object space vertex position input, lower/left is (0,0) and upper/right is (1,1)\n\
varying   vec2 VertexTexCoordVS;	// Normalized vertex texture coordinate output\n\
\n\
// Uniforms\n\
uniform vec4 GlyphSizePenPosition;			// Object space glyph size (xy) and object space pen position (zw) => scale & bias\n\
uniform vec4 TextureCoordinateMinMax;		// The normalized minimum (xy) and maximum (zw) glyph texture coordinate inside the glyph texture atlas\n\
uniform mat4 ObjectSpaceToClipSpaceMatrix;	// Object space to clip space matrix\n\
\n\
// Programs\n\
void main()\n\
{\n\
	// Calculate the object space vertex position\n\
	vec4 position = vec4(VertexPosition.xy*GlyphSizePenPosition.xy + GlyphSizePenPosition.zw, 0, 1);\n\
\n\
	// Calculate the clip space vertex position, lower/left is (-1,-1) and upper/right is (1,1)\n\
	gl_Position = ObjectSpaceToClipSpaceMatrix*position;\n\
\n\
	// Set the normalized vertex texture coordinate\n\
	if (int(VertexPosition.z) == 0)			// Vertex 0 - lower/left corner\n\
		VertexTexCoordVS = vec2(TextureCoordinateMinMax.x, TextureCoordinateMinMax.w);\n\
	else if (int(VertexPosition.z) == 1)	// Vertex 1 - lower/right corner\n\
		VertexTexCoordVS = TextureCoordinateMinMax.zw;\n\
	else if (int(VertexPosition.z) == 2)	// Vertex 2 - upper/left corner\n\
		VertexTexCoordVS = TextureCoordinateMinMax.xy;\n\
	else if (int(VertexPosition.z) == 3)	// Vertex 3 - upper/right corner\n\
		VertexTexCoordVS = vec2(TextureCoordinateMinMax.z, TextureCoordinateMinMax.y);\n\
}";


// OpenGL 2.0 ("#version 110") GLSL fragment shader source code, "#version" is added by "PLRenderer::ProgramGenerator"
static const PLGeneral::String sFragmentShaderSourceCodeGLSL = "\
// Attributes\n\
varying vec2 VertexTexCoordVS;	// Interpolated vertex texture coordinate from vertex shader\n\
\n\
// Uniforms\n\
uniform sampler2D GlyphMap;	// Glyph atlas texture map\n\
uniform vec4	  Color;	// Text color\n\
\n\
// Programs\n\
void main()\n\
{\n\
	gl_FragColor = vec4(Color.r, Color.g, Color.b, texture2D(GlyphMap, VertexTexCoordVS).a*Color.a);\n\
}";