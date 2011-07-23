/*********************************************************\
 *  File: PL3dsMaxSceneExport.cpp                        *
 *      Scene export plugin for 3ds Max - main plugin stuff
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
#include "PL3dsMaxSceneExport/PLToolbar.h"
#include "PL3dsMaxSceneExport/PLSceneEnumProc.h"


//[-------------------------------------------------------]
//[ Global variables                                      ]
//[-------------------------------------------------------]
HINSTANCE g_hInstance;
extern ClassDesc *GetPLToolbarDesc();


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class PLSceneExport : public SceneExport {


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		PLSceneExport()
		{
		}

		virtual ~PLSceneExport()
		{
		}

		int ExtCount()
		{
			return 1;
		}

		const TCHAR *Ext(int i)
		{
			return (i == 0) ? "scene" : "";
		}

		const TCHAR *LongDesc()
		{
			return "PixelLight scene export plugin http://www.pixellight.org";
		}

		const TCHAR *ShortDesc()
		{
			return "PixelLight scene export";
		}

		const TCHAR *AuthorName()
		{
			return "Copyright (C) 2002-2011 by The PixelLight Team";
		}

		const TCHAR *CopyrightMessage()
		{
			return "";
		}

		const TCHAR *OtherMessage1()
		{
			return "";
		}

		const TCHAR *OtherMessage2()
		{
			return "";
		}

		unsigned int Version()
		{
			return 84;
		}

		void ShowAbout(HWND hWnd)
		{
			MessageBox(hWnd, "PixelLight scene export plugin\nhttp://www.pixellight.org", "about", MB_OK);
		}

		BOOL SupportsOptions(int ext,DWORD options)
		{
			return (options == SCENE_EXPORT_SELECTED);
		}

		int DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts = false, DWORD options = 0)
		{
			if (name && i) {
				// We REALLY need to set the locale to a known setting... else we may get floats like "1,123" instead of "1.123"!
				char *pSaveLocale = _strdup(setlocale(LC_ALL, nullptr));	// Get the current set locale, we REALLY need to backup the locale because it "may" be changed by "setlocale"
				setlocale(LC_ALL, "C");

				// Export
				PLSceneEnumProc cExporter(name, *i, options);

				// Be polite and restore the previously set locale
				setlocale(LC_ALL, pSaveLocale);
				free(pSaveLocale);	// ... and don't forget to free the memory of our locale backup...
			}
			return 1;
		}


};

class PLSceneClassDesc : public ClassDesc {


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		int IsPublic()
		{
			return 1;
		}

		void *Create(BOOL loading = false)
		{
			return new PLSceneExport();
		}

		const TCHAR *ClassName()
		{
			return "PixelLight scene export";
		}

		SClass_ID SuperClassID()
		{
			return SCENE_EXPORT_CLASS_ID;
		}

		Class_ID ClassID()
		{
			return Class_ID(0xdeadbeff, 0x7b5a93fb);
		}

		const TCHAR *Category()
		{
			return "";
		}


};

static PLSceneClassDesc PLSceneDesc;

extern "C" {

	__declspec(dllexport) const TCHAR *LibDescription()
	{
		return "PixelLight scene export plugin";
	}

	__declspec(dllexport) int LibNumberClasses()
	{
		return 2;
	}

	__declspec(dllexport) ClassDesc *LibClassDesc(int i)
	{
		switch(i) {
			case 0:
				return &PLSceneDesc;

			case 1:
				return GetPLToolbarDesc();

			default:
				return 0;
		}
	}

	__declspec(dllexport) ULONG LibVersion()
	{
		return VERSION_3DSMAX;
	}

	__declspec(dllexport )ULONG CanAutoDefer()
	{
		return 1;
	}

}


//[-------------------------------------------------------]
//[ DllMain                                               ]
//[-------------------------------------------------------]
int bControlsInit = false;
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
	g_hInstance = hinstDLL;
	if (!bControlsInit) {
		bControlsInit = true;
		InitCommonControls();
	}
	return true;
}