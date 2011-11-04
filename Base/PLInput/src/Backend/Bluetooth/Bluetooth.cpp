/*********************************************************\
 *  File: Bluetooth.cpp                                  *
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
#include "PLInput/Backend/Bluetooth/Bluetooth.h"
#include "PLInput/Backend/Bluetooth/BTImpl.h"
#if defined(LINUX) && !defined(ANDROID) && !defined(APPLE)
	#include "PLInput/Backend/Bluetooth/BTLinux.h"
#endif


//[-------------------------------------------------------]
//[ Template instance                                     ]
//[-------------------------------------------------------]
namespace PLCore {
	template class Singleton<PLInput::Bluetooth>;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
namespace PLInput {


//[-------------------------------------------------------]
//[ Public static PLCore::Singleton functions             ]
//[-------------------------------------------------------]
Bluetooth *Bluetooth::GetInstance()
{
	// The compiler should be able to optimize this extra call (inlining)
	return Singleton<Bluetooth>::GetInstance();
}

bool Bluetooth::HasInstance()
{
	// The compiler should be able to optimize this extra call (inlining)
	return Singleton<Bluetooth>::HasInstance();
}


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Detect available Bluetooth devices
*/
void Bluetooth::DetectDevices()
{
	// Clear list
	Clear();

	// Check Bluetooth implementation
	if (m_pBTImpl) {
		// Enumerate devices
		m_pBTImpl->EnumerateDevices(m_lstDevices);
	}
}

/**
*  @brief
*    Get list of available Bluetooth devices
*/
const List<BTDevice*> &Bluetooth::GetDevices() const
{
	// Return list
	return m_lstDevices;
}


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
Bluetooth::Bluetooth() :
	// Create Bluetooth implementation
	#if defined(LINUX) && !defined(ANDROID) && !defined(APPLE)
		m_pBTImpl(new BTLinux())
	#else
		m_pBTImpl(nullptr)
	#endif
{
	// Detect devices
	DetectDevices();
}

/**
*  @brief
*    Destructor
*/
Bluetooth::~Bluetooth()
{
	// Clear devices
	Clear();

	// Delete Bluetooth implementation
	if (m_pBTImpl)
		delete m_pBTImpl;
}

/**
*  @brief
*    Remove all devices
*/
void Bluetooth::Clear()
{
	// Destroy devices
	m_lstDevices.Clear();
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLInput
