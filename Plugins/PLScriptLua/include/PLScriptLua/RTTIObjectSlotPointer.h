/*********************************************************\
 *  File: RTTIObjectSlotPointer.h                        *
 *
 *  Copyright (C) 2002-2012 The PixelLight Team (http://www.pixellight.org/)
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


#ifndef __PLSCRIPTLUA_RTTIOBJECTSLOTPOINTER_H__
#define __PLSCRIPTLUA_RTTIOBJECTSLOTPOINTER_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/Container/FastPool.h>
#include "PLScriptLua/RTTIObjectPointerBase.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace PLCore {
	class DynEventHandler;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLScriptLua {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
/**
*  @brief
*    RTTI object slot pointer
*/
class RTTIObjectSlotPointer : public RTTIObjectPointerBase, public PLCore::FastPoolElement<RTTIObjectSlotPointer> {


	//[-------------------------------------------------------]
	//[ Friends                                               ]
	//[-------------------------------------------------------]
	friend class LuaContext;


	//[-------------------------------------------------------]
	//[ Public functions                                      ]
	//[-------------------------------------------------------]
	public:
		/**
		*  @brief
		*    Default constructor
		*/
		RTTIObjectSlotPointer();

		/**
		*  @brief
		*    Destructor
		*/
		virtual ~RTTIObjectSlotPointer();

		/**
		*  @brief
		*    Returns the pointer to the RTTI object slot to wrap
		*
		*  @return
		*    Pointer to the RTTI object slot to wrap, can be a null pointer
		*/
		PLCore::DynEventHandler *GetDynEventHandler() const;


	//[-------------------------------------------------------]
	//[ Protected virtual LuaUserData functions               ]
	//[-------------------------------------------------------]
	protected:
		virtual int IndexMetamethod(lua_State *pLuaState) override;
		virtual int NewIndexMetamethod(lua_State *pLuaState) override;
		virtual void CGMetamethod(lua_State *pLuaState) override;
		virtual void CallMetamethod(lua_State *pLuaState) override;


	//[-------------------------------------------------------]
	//[ Protected data                                        ]
	//[-------------------------------------------------------]
	protected:
		PLCore::DynEventHandler *m_pDynEventHandler;	/**< Pointer to the RTTI object slot to wrap, can be a null pointer */


	//[-------------------------------------------------------]
	//[ Private functions                                     ]
	//[-------------------------------------------------------]
	private:
		/**
		*  @brief
		*    Initializes this instance
		*
		*  @param[in] cScript
		*    The owner script instance
		*  @param[in] pRTTIObject
		*    Pointer to the RTTI object to wrap, can be a null pointer
		*  @param[in] pDynEventHandler
		*    Pointer to the RTTI object slot to wrap, can be a null pointer
		*/
		void InitializeInstance(Script &cScript, PLCore::Object *pRTTIObject, PLCore::DynEventHandler *pDynEventHandler);


};


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLScriptLua


#endif // __PLSCRIPTLUA_RTTIOBJECTSLOTPOINTER_H__
