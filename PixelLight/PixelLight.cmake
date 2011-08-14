##################################################
## PixelLight cmake scripts
##
## Contains scripts and macros that are specific for
## the PixelLight project.
##################################################


##################################################
## Project information
##################################################

# Check which architecture has the host system
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES amd64*)
	set(X86_64 1)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES x86_64*)
	set(X86_64 1)
endif()

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES i686*)
	set(X86 1)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES i386*)
	set(X86 1)
endif()
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES x86*)
	set(X86 1)
endif() 

# Project name
set(CMAKETOOLS_PROJECT_NAME "PixelLight")

# Nightly build
set(CMAKETOOLS_CONFIG_NIGHTLY "0" CACHE BOOL "Create a nightly build?")

# Target architecture (x86, arm...)
set(CMAKETOOLS_TARGET_ARCH "x86" CACHE STRING "Target architecture")

# Set default bit size depending of the host system architecture (currently set 64bit only for x86_64 hosts)
if(X86_64 AND CMAKETOOLS_TARGET_ARCH MATCHES "x86")
	# Target bitsize (32/64)
	set(CMAKETOOLS_TARGET_BITSIZE "64" CACHE STRING "Target bitsize")

	# Target architecture & bitsize (x86, x64 - yes, there are many names for the 64 bit version, but e.g. "x64" is just more handy as the technically "x86_64")
	set(CMAKETOOLS_TARGET_ARCHBITSIZE "x64" CACHE STRING "Target architecture & bitsize")
else(X86_64 AND CMAKETOOLS_TARGET_ARCH MATCHES "x86")
	# Target bitsize (32/64)
	set(CMAKETOOLS_TARGET_BITSIZE "32" CACHE STRING "Target bitsize")

	# Target architecture & bitsize (x86, x64 - yes, there are many names for the 64 bit version, but e.g. "x64" is just more handy as the technically "x86_32")
	set(CMAKETOOLS_TARGET_ARCHBITSIZE "x86" CACHE STRING "Target architecture & bitsize")
endif(X86_64 AND CMAKETOOLS_TARGET_ARCH MATCHES "x86")

# Use system libraries or build externals?
set(CMAKETOOLS_USE_SYSTEM_LIBS "0" CACHE BOOL "Use system libraries or build own external libraries?")

# Repository URL (where to download the external pre-built packages from)
set(PL_EXTERNAL_REPOSITORY "pixellight.sourceforge.net/externals" CACHE STRING "External repository")

# User name for access to the repository
set(PL_EXTERNAL_USER "" CACHE STRING "External repository user name")

# User password for access to the repository
set(PL_EXTERNAL_PASS "" CACHE STRING "External repository user password")

# Use native plproject?
set(PL_NATIVE_PLPROJECT "1" CACHE BOOL "Use native plproject?")

# Build minimal projects?
set(CMAKETOOLS_MINIMAL "0" CACHE BOOL "Build minimal projects (or all)?")

# Build for mobile platforms?
set(PL_MOBILE "0" CACHE BOOL "Build for mobile platforms?")

# Build or exclude plugins
if(NOT CMAKETOOLS_MINIMAL)
	# Include everything
	set (PL_PLUGIN_SCRIPT_NULL 				"1" CACHE BOOL "Build plugin 'PLScriptNull'?")
	set (PL_PLUGIN_SCRIPT_LUA	 			"1" CACHE BOOL "Build plugin 'PLScriptLua'?")
	set (PL_PLUGIN_SCRIPT_V8		 		"1" CACHE BOOL "Build plugin 'PLScriptV8'?")
	set (PL_PLUGIN_SCRIPT_PYTHON			"1" CACHE BOOL "Build plugin 'PLScriptPython'?")
	set (PL_PLUGIN_SCRIPT_ANGELSCRIPT		"1" CACHE BOOL "Build plugin 'PLScriptAngelScript'?")
	set (PL_PLUGIN_DATABASE_NULL 			"1" CACHE BOOL "Build plugin 'PLDatabaseNull'?")
	set (PL_PLUGIN_DATABASE_MYSQL 			"1" CACHE BOOL "Build plugin 'PLDatabaseMySQL'?")
	set (PL_PLUGIN_DATABASE_POSTGRESQL 		"1" CACHE BOOL "Build plugin 'PLDatabasePostgreSQL'?")
	set (PL_PLUGIN_DATABASE_SQLITE 			"1" CACHE BOOL "Build plugin 'PLDatabaseSQLite'?")
	set (PL_PLUGIN_PHYSICS_NULL 			"1" CACHE BOOL "Build plugin 'PLPhysicsNull'?")
	set (PL_PLUGIN_PHYSICS_NEWTON 			"1" CACHE BOOL "Build plugin 'PLPhysicsNewton'?")
	set (PL_PLUGIN_PHYSICS_ODE 				"1" CACHE BOOL "Build plugin 'PLPhysicsODE'?")
	set (PL_PLUGIN_PHYSICS_BULLET			"1" CACHE BOOL "Build plugin 'PLPhysicsBullet'?")
	if(WIN32)
		set (PL_PLUGIN_PHYSICS_PHYSX 		"1" CACHE BOOL "Build plugin 'PLPhysicsPhysX'?")
		set (PL_PLUGIN_RENDERER_D3D9 		"1" CACHE BOOL "Build plugin 'PLRendererD3D9'?")
		set (PL_PLUGIN_RENDERER_D3D11 		"1" CACHE BOOL "Build plugin 'PLRendererD3D11'?")
		set (PL_PLUGIN_FRONTEND_ACTIVEX		"1" CACHE BOOL "Build plugin 'PLFrontendActiveX'?")
		set (PL_PLUGIN_FRONTEND_MOZILLA		"1" CACHE BOOL "Build plugin 'PLFrontendMozilla'?")
		set (PL_EXPORTER_3DSMAX_2008		"1" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2008'?")
		set (PL_EXPORTER_3DSMAX_2009		"1" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2009'?")
		set (PL_EXPORTER_3DSMAX_2010		"1" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2010'?")
		set (PL_EXPORTER_3DSMAX_2011		"1" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2011'?")
		set (PL_EXPORTER_3DSMAX_2012		"1" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2012'?")
		set (PL_PLUGIN_ENGINE_IMAGEEXR 		"1" CACHE BOOL "Build plugin 'PLImageLoaderEXR'?")
	else(WIN32)
		# Disable Windows only plugins
		set (PL_PLUGIN_PHYSICS_PHYSX 		"0" CACHE BOOL "Build plugin 'PLPhysicsPhysX'?")	# NVIDIA stopped supporting linux/unix in PHYSX for PC
		set (PL_PLUGIN_RENDERER_D3D9 		"0" CACHE BOOL "Build plugin 'PLRendererD3D9'?")
		set (PL_PLUGIN_RENDERER_D3D11 		"0" CACHE BOOL "Build plugin 'PLRendererD3D11'?")
		set (PL_PLUGIN_FRONTEND_ACTIVEX		"0" CACHE BOOL "Build plugin 'PLFrontendActiveX'?")
		set (PL_PLUGIN_FRONTEND_MOZILLA		"0" CACHE BOOL "Build plugin 'PLFrontendMozilla'?")
		set (PL_EXPORTER_3DSMAX_2008		"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2008'?")
		set (PL_EXPORTER_3DSMAX_2009		"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2009'?")
		set (PL_EXPORTER_3DSMAX_2010		"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2010'?")
		set (PL_EXPORTER_3DSMAX_2011		"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2011'?")
		set (PL_EXPORTER_3DSMAX_2012		"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2012'?")
		# Currently only under Windows buildable
		set (PL_PLUGIN_ENGINE_IMAGEEXR 		"0" CACHE BOOL "Build plugin 'PLImageLoaderEXR'?")
	endif(WIN32)
	set (PL_PLUGIN_RENDERER_NULL 			"1" CACHE BOOL "Build plugin 'PLRendererNull'?")
	set (PL_PLUGIN_RENDERER_OPENGL 			"1" CACHE BOOL "Build plugin 'PLRendererOpenGL'?")
	set (PL_PLUGIN_RENDERER_OPENGLCG 		"1" CACHE BOOL "Build plugin 'PLRendererOpenGLCg'?")
	set (PL_PLUGIN_RENDERER_OPENGLES		"1" CACHE BOOL "Build plugin 'PLRendererOpenGLES'?")
	set (PL_PLUGIN_SOUND_NULL 				"1" CACHE BOOL "Build plugin 'PLSoundNull'?")
	set (PL_PLUGIN_SOUND_OPENAL				"1" CACHE BOOL "Build plugin 'PLSoundOpenAL'?")
	set (PL_PLUGIN_SOUND_FMOD 				"1" CACHE BOOL "Build plugin 'PLSoundFMOD'?")
	set (PL_PLUGIN_SOUND_FMODEX 			"1" CACHE BOOL "Build plugin 'PLSoundFMODEx'?")
	set (PL_PLUGIN_ENGINE_ASSIMP			"1" CACHE BOOL "Build plugin 'PLAssimp'?")
	set (PL_PLUGIN_ENGINE_PARTICLEGROUPS	"1" CACHE BOOL "Build plugin 'PLParticleGroups'?")
	set (PL_PLUGIN_ENGINE_SPARK				"1" CACHE BOOL "Build plugin 'SPARK_PL'?")
	set (PL_PLUGIN_ENGINE_LIBROCKET			"1" CACHE BOOL "Build plugin 'libRocket_PL'?")
	set (PL_PLUGIN_ENGINE_POSTPROCESS		"1" CACHE BOOL "Build plugin 'PLPostProcessEffects'?")
	set (PL_PLUGIN_GUI_XMLTEXT				"1" CACHE BOOL "Build plugin 'PLGuiXmlText'?")
	set (PL_PLUGIN_FRONTEND_NULL 			"1" CACHE BOOL "Build plugin 'PLFrontendNull'?")
	set (PL_PLUGIN_FRONTEND_OS 				"1" CACHE BOOL "Build plugin 'PLFrontendOS'?")
	set (PL_PLUGIN_FRONTEND_PLGUI 			"1" CACHE BOOL "Build plugin 'PLFrontendPLGui'?")
	set (PL_PLUGIN_FRONTEND_QT 				"1" CACHE BOOL "Build plugin 'PLFrontendQT'?")
	set (PL_EDITOR							"0" CACHE BOOL "Build 'PLEditor'?")
else()
	# Minimal build
	set (PL_PLUGIN_SCRIPT_NULL 				"1" CACHE BOOL "Build plugin 'PLScriptNull'?")
	set (PL_PLUGIN_SCRIPT_LUA	 			"0" CACHE BOOL "Build plugin 'PLScriptLua'?")
	set (PL_PLUGIN_SCRIPT_V8		 		"0" CACHE BOOL "Build plugin 'PLScriptV8'?")
	set (PL_PLUGIN_SCRIPT_PYTHON			"0" CACHE BOOL "Build plugin 'PLScriptPython'?")
	set (PL_PLUGIN_SCRIPT_ANGELSCRIPT		"0" CACHE BOOL "Build plugin 'PLScriptAngelScript'?")
	set (PL_PLUGIN_DATABASE_NULL 			"1" CACHE BOOL "Build plugin 'PLDatabaseNull'?")
	set (PL_PLUGIN_DATABASE_MYSQL 			"0" CACHE BOOL "Build plugin 'PLDatabaseMySQL'?")
	set (PL_PLUGIN_DATABASE_POSTGRESQL 		"0" CACHE BOOL "Build plugin 'PLDatabasePostgreSQL'?")
	set (PL_PLUGIN_DATABASE_SQLITE 			"0" CACHE BOOL "Build plugin 'PLDatabaseSQLite'?")
	set (PL_PLUGIN_PHYSICS_NULL 			"1" CACHE BOOL "Build plugin 'PLPhysicsNull'?")
	# [TODO] Enable one physics plugin (newton, make it work on 64bit)
	set (PL_PLUGIN_PHYSICS_NEWTON	 		"0" CACHE BOOL "Build plugin 'PLPhysicsNewton'?")
	set (PL_PLUGIN_PHYSICS_ODE 				"0" CACHE BOOL "Build plugin 'PLPhysicsODE'?")
	set (PL_PLUGIN_PHYSICS_PHYSX 			"0" CACHE BOOL "Build plugin 'PLPhysicsPhysX'?")
	set (PL_PLUGIN_PHYSICS_BULLET			"0" CACHE BOOL "Build plugin 'PLPhysicsBullet'?")
	set (PL_PLUGIN_RENDERER_NULL	 		"1" CACHE BOOL "Build plugin 'PLRendererNull'?")
	if (PL_MOBILE)
		set (PL_PLUGIN_RENDERER_OPENGL 		"0" CACHE BOOL "Build plugin 'PLRendererOpenGL'?")
		set (PL_PLUGIN_RENDERER_OPENGLCG 	"0" CACHE BOOL "Build plugin 'PLRendererOpenGLCg'?")
		set (PL_PLUGIN_RENDERER_OPENGLES	"1" CACHE BOOL "Build plugin 'PLRendererOpenGLES'?")
	else()
		set (PL_PLUGIN_RENDERER_OPENGL 		"1" CACHE BOOL "Build plugin 'PLRendererOpenGL'?")
		set (PL_PLUGIN_RENDERER_OPENGLCG	"0" CACHE BOOL "Build plugin 'PLRendererOpenGLCg'?")
		set (PL_PLUGIN_RENDERER_OPENGLES	"0" CACHE BOOL "Build plugin 'PLRendererOpenGLES'?")
	endif()
	set (PL_PLUGIN_RENDERER_D3D9 			"0" CACHE BOOL "Build plugin 'PLRendererD3D9'?")
	set (PL_PLUGIN_RENDERER_D3D11 			"0" CACHE BOOL "Build plugin 'PLRendererD3D11'?")
	set (PL_PLUGIN_SOUND_NULL 				"1" CACHE BOOL "Build plugin 'PLSoundNull'?")
	# [TODO] Enable one sound plugin (OpenAL, make it work on 64bit)
	set (PL_PLUGIN_SOUND_OPENAL				"0" CACHE BOOL "Build plugin 'PLSoundOpenAL'?")
	set (PL_PLUGIN_SOUND_FMOD 				"0" CACHE BOOL "Build plugin 'PLSoundFMOD'?")
	set (PL_PLUGIN_SOUND_FMODEX 			"0" CACHE BOOL "Build plugin 'PLSoundFMODEx'?")
	set (PL_PLUGIN_ENGINE_ASSIMP			"0" CACHE BOOL "Build plugin 'PLAssimp'?")
	set (PL_PLUGIN_ENGINE_IMAGEEXR 			"0" CACHE BOOL "Build plugin 'PLImageLoaderEXR'?")
	set (PL_PLUGIN_ENGINE_PARTICLEGROUPS	"0" CACHE BOOL "Build plugin 'PLParticleGroups'?")
	set (PL_PLUGIN_ENGINE_SPARK				"0" CACHE BOOL "Build plugin 'SPARK_PL'?")
	set (PL_PLUGIN_ENGINE_LIBROCKET			"0" CACHE BOOL "Build plugin 'libRocket_PL'?")
	set (PL_PLUGIN_ENGINE_POSTPROCESS		"0" CACHE BOOL "Build plugin 'PLPostProcessEffects'?")
	set (PL_PLUGIN_GUI_XMLTEXT				"0" CACHE BOOL "Build plugin 'PLGuiXmlText'?")
	set (PL_PLUGIN_FRONTEND_NULL 			"1" CACHE BOOL "Build plugin 'PLFrontendNull'?")
	set (PL_PLUGIN_FRONTEND_OS 				"1" CACHE BOOL "Build plugin 'PLFrontendOS'?")
	set (PL_PLUGIN_FRONTEND_PLGUI 			"1" CACHE BOOL "Build plugin 'PLFrontendPLGui'?")
	set (PL_PLUGIN_FRONTEND_QT 				"0" CACHE BOOL "Build plugin 'PLFrontendQT'?")
	set (PL_PLUGIN_FRONTEND_ACTIVEX			"0" CACHE BOOL "Build plugin 'PLFrontendActiveX'?")
	set (PL_PLUGIN_FRONTEND_MOZILLA			"0" CACHE BOOL "Build plugin 'PLFrontendMozilla'?")
	set (PL_EXPORTER_3DSMAX_2008			"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2008'?")
	set (PL_EXPORTER_3DSMAX_2009			"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2009'?")
	set (PL_EXPORTER_3DSMAX_2010			"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2010'?")
	set (PL_EXPORTER_3DSMAX_2011			"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2011'?")
	set (PL_EXPORTER_3DSMAX_2012			"0" CACHE BOOL "Build exporter plugin 'PL3dsMaxSceneExport_2012'?")
	set (PL_EDITOR							"0" CACHE BOOL "Build 'PLEditor'?")
endif()

# Project suffix
#   The suffix is appended to every library (e.g. [lib]MyLibrary-<suffix>.dll/lib/so/a)
#   and can be used to install several versions of PixelLight at the same time
set(CMAKETOOLS_CONFIG_SUFFIX "" CACHE STRING "Library suffix")

# Version
set(CMAKETOOLS_PROJECT_VERSION_MAJOR    0)
set(CMAKETOOLS_PROJECT_VERSION_MINOR    9)
set(CMAKETOOLS_PROJECT_VERSION_PATCH    8)
set(CMAKETOOLS_PROJECT_VERSION_RELEASE "R1")
if(CMAKETOOLS_CONFIG_NIGHTLY)
	# [TODO] Add date to string (e.g. "nightly-20100101")
	set(CMAKETOOLS_PROJECT_VERSION_RELEASE "nightly")
endif()

# Upload destination
if(CMAKETOOLS_CONFIG_NIGHTLY)
	set(CMAKETOOLS_UPLOAD_LOCATION "pixellight.org:/srv/www/pl-dev/files/nightly")
else()
	set(CMAKETOOLS_UPLOAD_LOCATION "pixellight.org:/srv/www/pl-dev/files/v${CMAKETOOLS_PROJECT_VERSION_MAJOR}.${CMAKETOOLS_PROJECT_VERSION_MINOR}")
endif()
message(STATUS "Upload destination is '${CMAKETOOLS_UPLOAD_LOCATION}'")

# Install locations
if(WIN32)
	set(PL_INSTALL_ROOT				".")										# C:\Programme\PixelLight
	set(PL_INSTALL_BIN				".")										# C:\Programme\PixelLight
	set(PL_INSTALL_SHARED			"Runtime")									# C:\Programme\PixelLight\Runtime
	set(PL_INSTALL_LIB				"Lib")										# C:\Programme\PixelLight\Lib
	set(PL_INSTALL_INCLUDE			"Include")									# C:\Programme\PixelLight\Include
	set(PL_INSTALL_DOCS				"Docs")										# C:\Programme\PixelLight\Docs
	set(PL_INSTALL_RUNTIME			"Runtime")									# C:\Programme\PixelLight\Runtime
	set(PL_INSTALL_RUNTIME_BIN		"Runtime/${CMAKETOOLS_TARGET_ARCHBITSIZE}")	# C:\Programme\PixelLight\Runtime\x86
	set(PL_INSTALL_RUNTIME_PLUGINS	"Runtime/Plugins")							# C:\Programme\PixelLight\Runtime\Plugins
	set(PL_INSTALL_SAMPLES			"Samples")									# C:\Programme\PixelLight\Samples
	set(PL_INSTALL_SAMPLES_BIN		"Samples/Bin")								# C:\Programme\PixelLight\Samples\Bin
	set(PL_INSTALL_SAMPLES_PLUGINS	"Samples/Bin/Plugins")						# C:\Programme\PixelLight\Samples\Bin\Plugins
	set(PL_INSTALL_TOOLS			"Tools")									# C:\Programme\PixelLight\Tools
	set(PL_INSTALL_TOOLS_BIN		"Tools")									# C:\Programme\PixelLight\Tools
	set(PL_INSTALL_TOOLS_PLUGINS	"Tools/Plugins")							# C:\Programme\PixelLight\Tools\Plugins
	set(PL_INSTALL_BROWSER			"BrowserPlugins")							# C:\Programme\PixelLight\BrowserPlugins
elseif(LINUX)
	set(PL_INSTALL_ROOT				"share/pixellight")							# /usr/share/pixellight
	set(PL_INSTALL_BIN				"bin")										# /usr/bin
	set(PL_INSTALL_SHARED			"lib")										# /usr/lib
	set(PL_INSTALL_LIB				"lib")										# /usr/lib
	set(PL_INSTALL_INCLUDE			"include")									# /usr/include
	set(PL_INSTALL_DOCS				"share/docs/pixellight")					# /usr/share/docs/pixellight
	set(PL_INSTALL_RUNTIME			"share/pixellight/Runtime")					# /usr/share/pixellight/Runtime
	set(PL_INSTALL_RUNTIME_BIN		"bin")										# /usr/bin
	set(PL_INSTALL_RUNTIME_PLUGINS	"share/pixellight/Runtime/Plugins")			# /usr/share/pixellight/Runtime/Plugins
	set(PL_INSTALL_SAMPLES			"share/pixellight/Samples")					# /usr/share/pixellight/Samples
	set(PL_INSTALL_SAMPLES_BIN		"share/pixellight/Samples/Bin")				# /usr/share/pixellight/Samples/Bin
	set(PL_INSTALL_SAMPLES_PLUGINS	"share/pixellight/Samples/Bin/Plugins")		# /usr/share/pixellight/Samples/Bin/Plugins
	set(PL_INSTALL_TOOLS			"share/pixellight/Tools")					# /usr/share/pixellight/Tools
	set(PL_INSTALL_TOOLS_BIN		"bin")										# /usr/bin
	set(PL_INSTALL_TOOLS_PLUGINS	"share/pixellight/Tools/Plugins")			# /usr/share/pixellight/Tools/Plugins
	set(PL_INSTALL_BROWSER			"share/pixellight/BrowserPlugins")			# /usr/share/pixellight/BrowserPlugins
endif()

# Extensions of files to exclude when e.g. copying a directory
# sdf	= VisualStudio 2010 file
# ncb	= VisualStudio 2010 file
# suo	= VisualStudio 2010 file
# user	= VisualStudio 2010 file
# *.*~	= Backup file ending used by some editors under Linux
set(PL_EXCLUDE_FILE_EXTENSIONS ".*\\.(sdf|ncb|suo|user|.*~")							# Set opening bracket as well as some standard unwanted file extensions
if(NOT WIN32)
	set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS}|sln|vcxproj|filters")	# VisualStudio 2010 files only for MS Windows, please
endif()
if(NOT PL_PLUGIN_SCRIPT_LUA)
	set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS}|lua")					# Lua
endif()
if(NOT PL_PLUGIN_SCRIPT_V8)
	set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS}|js")					# JavaScript
endif()
if(NOT PL_PLUGIN_SCRIPT_PYTHON)
	set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS}|py")					# Python
endif()
if(NOT PL_PLUGIN_SCRIPT_ANGELSCRIPT)
	set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS}|as")					# AngelScript
endif()
set(PL_EXCLUDE_FILE_EXTENSIONS "${PL_EXCLUDE_FILE_EXTENSIONS})")						# Set closing bracket
message(STATUS "Extensions of files to exclude when e.g. copying a directory: ${PL_EXCLUDE_FILE_EXTENSIONS}")


##################################################
## Macros
##################################################

##################################################
## MACRO: call plproject
##
## Call plproject tool
##################################################
macro(pl_plproject dir outputpath writeplugin)
	add_dependencies(${CMAKETOOLS_CURRENT_TARGET} PLProject)
	add_custom_command(TARGET ${CMAKETOOLS_CURRENT_TARGET}
		COMMAND ${PL_PLPROJECT_COMMAND} ${dir} --output-path ${outputpath} --write-plugin ${writeplugin}
	)
endmacro(pl_plproject dir outputpath writeplugin)

##################################################
## MACRO: pl_create_plugin_description
##
## Create PixelLight plugin description
##################################################
macro(pl_create_plugin_description src dest)
	if(CMAKETOOLS_CONFIG_SUFFIX)
		set(suffix --suffix ${CMAKETOOLS_CONFIG_SUFFIX})
	else()
		set(suffix "")
	endif()

	# Call PLProject
	pl_plproject(${src} ${dest} "${suffix}")
endmacro(pl_create_plugin_description src dest)


##################################################
## Initialization
##################################################

# Set path to PLProject
if(PL_NATIVE_PLPROJECT)
	if(CMAKE_BUILD_TYPE MATCHES "Debug")
		set(PL_PLPROJECT_COMMAND "${CMAKE_SOURCE_DIR}/Bin-${CMAKETOOLS_HOST_SYSTEM}/Tools/PLProjectD${CMAKE_EXECUTABLE_SUFFIX}")
	else()
		set(PL_PLPROJECT_COMMAND "${CMAKE_SOURCE_DIR}/Bin-${CMAKETOOLS_HOST_SYSTEM}/Tools/PLProject${CMAKE_EXECUTABLE_SUFFIX}")
	endif()
else()
	set(PL_PLPROJECT_COMMAND "${CMAKE_SOURCE_DIR}/Tools/PLProject/src/PLProject.py")
endif()

# Configure use of inline assembly
if(NOT CMAKETOOLS_TARGET_ARCH STREQUAL "x86")
	SET(CMAKETOOLS_CONFIG_NO_INLINE_ASM "1")
endif()
if(NOT CMAKETOOLS_TARGET_BITSIZE STREQUAL "32")
	SET(CMAKETOOLS_CONFIG_NO_INLINE_ASM "1")
endif()
