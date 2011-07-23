/*********************************************************\
 *  File: PLRendererD3D9.cpp                             *
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/ModuleMain.h>


//[-------------------------------------------------------]
//[ Plugin implementation                                 ]
//[-------------------------------------------------------]
pl_module_plugin("PLRendererD3D9")
	pl_module_vendor("Copyright (C) 2002-2011 by The PixelLight Team")
	pl_module_license("GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version")
	pl_module_description("DirectX 9 (November 2007, http://msdn.microsoft.com/directx/) renderer implementation")
	pl_module_dependencies_win32_release("cg.dll cgD3D9.dll")
	pl_module_dependencies_win32_debug("cg.dll cgD3D9.dll")
	pl_module_dependencies_win64_release("cg64.dll cgD3D964.dll")
	pl_module_dependencies_win64_debug("cg64.dll cgD3D964.dll")
pl_module_end