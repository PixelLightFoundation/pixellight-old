/*********************************************************\
 *  File: Application.h                                  *
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


#ifndef __PLSAMPLE_64_APPLICATION_H__
#define __PLSAMPLE_64_APPLICATION_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLEngine/Application/EngineApplication.h>


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace PLInput {
	class Control;
}
class MyPicking;


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    Application class
*/
class Application : public PLEngine::EngineApplication {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(pl_rtti_export, Application, "", PLEngine::EngineApplication, "Application class")
		// Constructors
		pl_constructor_0(DefaultConstructor,	"Default constructor",	"")
		// Slots
		pl_slot_1(OnControl,	PLInput::Control&,	"Called when a control event has occurred, occurred control as first parameter",	"")
	pl_class_end


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Constructor
		*/
		Application();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~Application();


	//[-------------------------------------------------------]
	//[ Public virtual PLEngine::EngineApplication functions  ]
	//[-------------------------------------------------------]
	public:
		virtual void SetCamera(PLScene::SNCamera *pCamera) override;


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Called when a control event has occurred
		*
		*  @param[in] cControl
		*    Occurred control
		*/
		void OnControl(PLInput::Control &cControl);


	//[-------------------------------------------------------]
	//[ Private virtual PLRenderer::RenderApplication functions ]
	//[-------------------------------------------------------]
	private:
		virtual bool OnUpdate() override;


	//[-------------------------------------------------------]
	//[ Private virtual PLEngine::EngineApplication functions ]
	//[-------------------------------------------------------]
	private:
		virtual void OnCreateScene(PLScene::SceneContainer &cContainer) override;
		virtual void OnCreateInputController() override;


	//[-------------------------------------------------------]
	//[ Private data                                          ]
	//[-------------------------------------------------------]
	private:
		MyPicking *m_pMyPicking;	/**< Picking application component, can be a null pointer */


};


#endif // __PLSAMPLE_64_APPLICATION_H__
