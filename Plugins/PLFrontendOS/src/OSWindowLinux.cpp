/*********************************************************\
 *  File: OSWindowLinux.cpp                              *
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


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <string.h>
#include <signal.h>
#include "../pl_icon.h"
#include <PLCore/Tools/Timing.h>
#include <PLCore/Frontend/Frontend.h>
#include <PLCore/Frontend/FrontendContext.h>
#include "PLFrontendOS/Frontend.h"
#include "PLFrontendOS/Linux_XDnDFileDropHelper.h"
#include "PLFrontendOS/OSWindowLinux.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
namespace PLFrontendOS {


//[-------------------------------------------------------]
//[ Ugly global variables                                 ]
//[-------------------------------------------------------]
bool g_bSignalSystemQuit = false;	/**< Does the OS asks us to shut down? */


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
OSWindowLinux::OSWindowLinux(Frontend &cFrontendOS) :
	m_pFrontendOS(&cFrontendOS),
	m_pDisplay(XOpenDisplay(nullptr)),
	m_nNativeWindowHandle(NULL_HANDLE),
	m_bInitialized(false),
	m_bVisible(false),
	m_bIsMouseOver(false),
	m_bMouseVisible(true),
	m_bPause(true),
	m_nInvisibleCursor(0),
	// Atoms
	WM_DELETE_WINDOW	(XInternAtom(m_pDisplay, "WM_DELETE_WINDOW",	 True)),
	UTF8_STRING			(XInternAtom(m_pDisplay, "UTF8_STRING",			 False)),
	WM_NAME				(XInternAtom(m_pDisplay, "WM_NAME",				 False)),
	_NET_WM_NAME		(XInternAtom(m_pDisplay, "_NET_WM_NAME",		 False)),
	_NET_WM_VISIBLE_NAME(XInternAtom(m_pDisplay, "_NET_WM_VISIBLE_NAME", False)),
	m_pDropHelper(nullptr)
{
	// Tell the frontend about this instance at once because it may already be required during frontend life cycle initialization
	m_pFrontendOS->m_pOSWindow = this;

	// Connect Linux signals
	signal(SIGINT,  OSWindowLinux::SignalHandler);
	signal(SIGTERM, OSWindowLinux::SignalHandler);

	{ // Create the native OS window
		const unsigned int  nWidth  = 640;
		const unsigned int  nHeight = 480;
		const int           nScreen = DefaultScreen(m_pDisplay);
		Visual             *pVisual = DefaultVisual(m_pDisplay, nScreen);
		const int           nDepth  = DefaultDepth(m_pDisplay, nScreen);

		// Create the native OS window instance with a black background (else we will see trash if nothing has been drawn, yet)
		XSetWindowAttributes sXSetWindowAttributes;
		sXSetWindowAttributes.background_pixel = 0;
		sXSetWindowAttributes.event_mask = ExposureMask | StructureNotifyMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask | VisibilityChangeMask | KeyPressMask | MotionNotify;
		m_nNativeWindowHandle = XCreateWindow(m_pDisplay, XRootWindow(m_pDisplay, nScreen), 0, 0, nWidth, nHeight, 0, nDepth, InputOutput, pVisual, CWBackPixel | CWEventMask, &sXSetWindowAttributes);
		XSetWMProtocols(m_pDisplay, m_nNativeWindowHandle, &WM_DELETE_WINDOW, 1);

		// Set icon
		Atom wmIcon = XInternAtom(m_pDisplay, "_NET_WM_ICON", False);
		Atom wmCardinal = XInternAtom(m_pDisplay, "CARDINAL", False);
		XChangeProperty(m_pDisplay, m_nNativeWindowHandle, wmIcon, wmCardinal, 32,
						PropModeReplace, reinterpret_cast<const unsigned char*>(pl_icon), pl_icon_length);

		// Set window title
		SetTitle(m_pFrontendOS->GetFrontend() ? m_pFrontendOS->GetFrontend()->GetContext().GetName() : "");

		// Create the drag'n'drop helper instance
		m_pDropHelper = new Linux_XDnDFileDropHelper(*this);

		// Do it!
		XSync(m_pDisplay, False);

		// Create the invisible cursor instance
		CreateInvisibleCursor();
	}

	// Do the frontend life cycle thing - start
	m_pFrontendOS->OnStart();

	// If the window is not visible yet, make it visible right now
	MakeVisible();

	// Initialization is done
	m_bInitialized = true;
}

/**
*  @brief
*    Destructor
*/
OSWindowLinux::~OSWindowLinux()
{
	// Do the frontend life cycle thing - pause
	// -> "OnPause()" is already handled inside "OSWindowLinux::Ping()"
	// -> Life cycle methods should only be called from a single point in the program flow in order to ensure correctness
	// -> We could put the event processing inside "OSWindowLinux::Ping()" into a separate method and then calling it in
	//    here, but this would not bring many benefits. So, this comment block and a direct method call has to do the job
	//    to perform a clean life cycle.
	// -> Before calling "OnPause()", ensure that we're currently not within the pause-state, else we would call "OnPause()" twice
	if (!m_bPause)
		m_pFrontendOS->OnPause();

	// Do the frontend life cycle thing - stop
	m_pFrontendOS->OnStop();

	// Destroy the drag'n'drop helper instance
	if (m_pDropHelper)
		delete m_pDropHelper;

	// Check if the window has already been destroyed
	if (m_nNativeWindowHandle) {
		// Send destroy message to window
		XDestroyWindow(m_pDisplay, m_nNativeWindowHandle);
	}

	// Destroy the invisible cursor instance
	if (m_nInvisibleCursor)
		XFreeCursor(m_pDisplay, m_nInvisibleCursor);

	// Close the X11 display connection
	XCloseDisplay(m_pDisplay);
}

/**
*  @brief
*    If the window is not visible yet, make it visible right now
*/
void OSWindowLinux::MakeVisible()
{
	if (!m_bVisible && m_nNativeWindowHandle) {
		// The window is now considered to be visible
		m_bVisible = true;

		// Show window
		XMapRaised(m_pDisplay, m_nNativeWindowHandle);

		// Do it!
		XSync(m_pDisplay, False);
	}
}

/**
*  @brief
*    Creates an invisible cursor
*/
void OSWindowLinux::CreateInvisibleCursor()
{
	// Data of the "invisible" cursor
	XColor sXColorBlack;
	sXColorBlack.red = sXColorBlack.green = sXColorBlack.blue = 0;
	static char nNoData[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	// Create the bitmap (it's our responsibility to free the bitmap using "XFreePixmap()" when finished)
	Pixmap sPixmapNoData = XCreateBitmapFromData(m_pDisplay, m_nNativeWindowHandle, nNoData, 8, 8);

	// Create the pixmap cursor
	m_nInvisibleCursor = XCreatePixmapCursor(m_pDisplay, sPixmapNoData, sPixmapNoData, &sXColorBlack, &sXColorBlack, 0, 0);

	// The "XCreatePixmapCursor"-documentation (http://tronche.com/gui/x/xlib/pixmap-and-cursor/XCreatePixmapCursor.html) states:
	// "The pixmaps can be freed immediately if no further explicit references to them are to be made"
	XFreePixmap(m_pDisplay, sPixmapNoData);
}

void OSWindowLinux::OnDrop(const Container<String> &lstFiles) const
{
	m_pFrontendOS->OnDrop(lstFiles);
}


//[-------------------------------------------------------]
//[ Private virtual OSWindow functions                    ]
//[-------------------------------------------------------]
handle OSWindowLinux::GetNativeWindowHandle() const
{
	return static_cast<handle>(m_nNativeWindowHandle);
}

void OSWindowLinux::Redraw()
{
	if (m_nNativeWindowHandle) {
		// If the window is not visible yet, make it visible right now
		MakeVisible();

		// Send expose event
		XEvent sEvent;
		sEvent.type			 = Expose;
		sEvent.xany.window	 = m_nNativeWindowHandle;
		sEvent.xexpose.count = 0;
		XSendEvent(m_pDisplay, m_nNativeWindowHandle, False, 0, &sEvent);

		// Do it!
		XSync(m_pDisplay, False);
	}
}

bool OSWindowLinux::Ping()
{
	bool bQuit = g_bSignalSystemQuit ? true : false;

	// Check if we're allowed to perform an update right now, please note that an update is only allowed when the frontend is fully initialized
	if (m_bInitialized && Timing::GetInstance()->Update()) {
		// Let the frontend update it's states (do this before drawing else, e.g. the first frame may have an unwanted content)
		m_pFrontendOS->OnUpdate();
	}

	// Look if messages are waiting (non-blocking)
	while (XPending(m_pDisplay) > 0) {
		// Get the waiting message
		XEvent sXEvent;
		XNextEvent(m_pDisplay, &sXEvent);

		// Process message
		switch (sXEvent.type) {
			case Expose:
				// There could be more then one Expose event currently in the event loop.
				// To avoid too many redraw calls, call OnDraw only when the current processed Expose event is the last one.
				if (!sXEvent.xexpose.count)
					m_pFrontendOS->OnDraw();
				break;

			case DestroyNotify:
				// Mark window destroyed
				m_nNativeWindowHandle = NULL_HANDLE;
				bQuit = true;
				break;

			// Window configuration changed
			case ConfigureNotify:
				// Inform that the window size has been changed
				m_pFrontendOS->OnSize();
				break;

			case UnmapNotify:
			case FocusOut:
				m_bPause = true;
				m_pFrontendOS->OnPause();
				break;

			case MapNotify:
			case FocusIn:
				m_pFrontendOS->OnResume();
				m_bPause = false;
				break;

			case ClientMessage:
				// Check if the client message is a XDnD message, otherwise check for WM_DELETE_WINDOW
				if (!m_pDropHelper || !m_pDropHelper->HandleClientMessage(sXEvent.xclient)) {
					// When the "WM_DELETE_WINDOW" client message is send, no "DestroyNotify"-message is generated because the
					// application itself should destroy/close the window to which the "WM_DELETE_WINDOW" client message was send to.
					// In this case, we will leave the event loop after this message was processed and no other messages are in the queue.
					// -> No "DestroyNotify"-message can be received
					if (sXEvent.xclient.data.l[0] == WM_DELETE_WINDOW)
						bQuit = true;
				}
				break;

			case SelectionNotify:
				if (m_pDropHelper)
					m_pDropHelper->HandleXdndSelection(sXEvent.xselection);
				break;

			case KeyPress:
				// Is it allowed to toggle the fullscreen mode using hotkeys? (Alt-Return or AltGr-Return)
				if (m_pFrontendOS->GetToggleFullscreenMode()) {
					// It's allowed, toggle fullscreen right now?
					const unsigned int nKey = XLookupKeysym(&sXEvent.xkey, 0);
					if (nKey == XK_Return && ((sXEvent.xkey.state & Mod1Mask) || (sXEvent.xkey.state & Mod2Mask))) {
						// Toggle fullscreen mode
						m_pFrontendOS->SetFullscreen(!m_pFrontendOS->IsFullscreen());
					}
				}
				break;

			// Mouse entered the window area
			case EnterNotify:
				m_bIsMouseOver = true;
				break;

			// Mouse left the window area
			case LeaveNotify:
				m_bIsMouseOver = false;
				break;
		}
	}

	// Done
	return bQuit;
}

String OSWindowLinux::GetTitle() const
{
	if (m_nNativeWindowHandle) {
		// Request the window title from the OS
		Atom		   sPropertyType;
		int			   nDataUnit;
		unsigned long  nBytesLeft;
		unsigned long  nNumberOfUnits;
		unsigned char *pszName;

		// Try first getting as utf-8 string
		if (XGetWindowProperty(m_pDisplay, m_nNativeWindowHandle, WM_NAME, 0, 65536/sizeof(long), False, UTF8_STRING,
							   &sPropertyType, &nDataUnit, &nNumberOfUnits, &nBytesLeft, &pszName) == XLib::Success) {
			// Return the window title
			return String::FromUTF8(reinterpret_cast<const char*>(pszName));

		// Try getting as plain text
		} else if (XGetWindowProperty(m_pDisplay, m_nNativeWindowHandle, WM_NAME, 0, 65536/sizeof(long), False, XA_STRING,
							   &sPropertyType, &nDataUnit, &nNumberOfUnits, &nBytesLeft, &pszName) == XLib::Success) {
			// Return the window title
			return String::FromUTF8(reinterpret_cast<const char*>(pszName));
		}
	}

	// Error!
	return "";
}

void OSWindowLinux::SetTitle(const String &sTitle)
{
	if (m_nNativeWindowHandle) {
		// We need here the number of bytes of the string because the number of characters (returned by String::GetLength) and the used number of bytes can differ in an utf-8 string
		// String::GetNumOfBytes can't be used here because the String class doesn't support UTF-8 as an internal format.
		// Even if the String class uses UNICODE as internal format this wouldn't work either.
		// Because the UNICODE String format uses wchar_t as datatype and the size of this type is greater than 1 byte.
		// Each character in a string needs the same amount of bytes.
		// In an utf-8 encoded string the byte-count per character can differ between 1 byte (ASCII char) and 4 bytes (UNICODE char in Unicode Code point range 	U+010000 to U+10FFFF)
		const unsigned char *pszWindowTitle = reinterpret_cast<const unsigned char*>(sTitle.GetUTF8());
		const int nNumOfElements = strlen(reinterpret_cast<const char*>(pszWindowTitle));
		XChangeProperty(m_pDisplay, m_nNativeWindowHandle, WM_NAME,				 UTF8_STRING, 8, PropModeReplace, pszWindowTitle, nNumOfElements);
		XChangeProperty(m_pDisplay, m_nNativeWindowHandle, _NET_WM_NAME,		 UTF8_STRING, 8, PropModeReplace, pszWindowTitle, nNumOfElements);
		XChangeProperty(m_pDisplay, m_nNativeWindowHandle, _NET_WM_VISIBLE_NAME, UTF8_STRING, 8, PropModeReplace, pszWindowTitle, nNumOfElements);
	}
}

int OSWindowLinux::GetX() const
{
	if (m_nNativeWindowHandle) {
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(m_pDisplay, m_nNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Lookout! We need the position relative to the root window! (window decoration topic, don't do the horror below and you just get wrong position data)
		::Window nChildWindow = 0;
		XTranslateCoordinates(m_pDisplay, m_nNativeWindowHandle, nRootWindow, nPositionX, nPositionY, &nPositionX, &nPositionY, &nChildWindow);
		XGetGeometry(m_pDisplay, nChildWindow, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Return the window x position (in screen coordinates)
		return nPositionX;
	} else {
		// Error!
		return 0;
	}
}

int OSWindowLinux::GetY() const
{
	if (m_nNativeWindowHandle) {
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(m_pDisplay, m_nNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Lookout! We need the position relative to the root window! (window decoration topic, don't do the horror below and you just get wrong position data)
		::Window nChildWindow = 0;
		XTranslateCoordinates(m_pDisplay, m_nNativeWindowHandle, nRootWindow, nPositionX, nPositionY, &nPositionX, &nPositionY, &nChildWindow);
		XGetGeometry(m_pDisplay, nChildWindow, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Return the window y position (in screen coordinates)
		return nPositionY;
	} else {
		// Error!
		return 0;
	}
}

uint32 OSWindowLinux::GetWidth() const
{
	if (m_nNativeWindowHandle) {
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(m_pDisplay, m_nNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Return the window width
		return nWidth;
	} else {
		// Error!
		return 0;
	}
}

uint32 OSWindowLinux::GetHeight() const
{
	if (m_nNativeWindowHandle) {
		// Get X window geometry information
		::Window nRootWindow = 0;
		int nPositionX = 0, nPositionY = 0;
		unsigned int nWidth = 0, nHeight = 0, nBorder = 0, nDepth = 0;
		XGetGeometry(m_pDisplay, m_nNativeWindowHandle, &nRootWindow, &nPositionX, &nPositionY, &nWidth, &nHeight, &nBorder, &nDepth);

		// Return the window height
		return nHeight;
	} else {
		// Error!
		return 0;
	}
}

void OSWindowLinux::SetWindowPositionSize(int nX, int nY, uint32 nWidth, uint32 nHeight)
{
	if (m_nNativeWindowHandle) {
		{ // Correct frontend position and size settings
			// Get screen info (the documentation doesn't state whether or not we need to free this screen when we're done, so we don't free it)
			Screen *pScreenInfo = XScreenOfDisplay(m_pDisplay, 0);

			// Correct frontend position and size settings
			Frontend::CorrectPositionSize(nX, nY, nWidth, nHeight, 0, 0, XWidthOfScreen(pScreenInfo), XHeightOfScreen(pScreenInfo));
		}

		// If the window is not visible yet, make it visible right now
		MakeVisible();

		{ // Set OS window position and size
			// Set position and size (only works correctly when the window is already visible, that's why the "MakeVisible()" call must be done first)
			XWindowChanges sChanges;
			sChanges.x		= nX;
			sChanges.y		= nY;
			sChanges.width	= nWidth;
			sChanges.height	= nHeight;
			XConfigureWindow(m_pDisplay, m_nNativeWindowHandle, CWX | CWY | CWWidth | CWHeight, &sChanges);

			// Do it!
			XSync(m_pDisplay, False);
		}
	}
}

void OSWindowLinux::SetFullscreenAltTab(bool bAllowed)
{
	// Nothing to do in here
}

void OSWindowLinux::SetFullscreen(bool bFullscreen)
{
	// Set/remove _NET_WM_STATE_FULLSCREEN to toggle fullscreen mode.
	// The window manger is responsible to restore the original position and size of the window when the fullscreen mode should be left.
	// This works only on window manger which are EWMH (v1.3 or greater) compatible (http://standards.freedesktop.org/wm-spec/wm-spec-1.3.html)
	Atom wmFullScreen = XInternAtom(m_pDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	Atom wmState = XInternAtom(m_pDisplay, "_NET_WM_STATE", False);

	// Setup the X-event
	XEvent sXEvent;
	memset(&sXEvent, 0, sizeof(sXEvent));
	sXEvent.type                 = ClientMessage;
	sXEvent.xclient.window       = m_nNativeWindowHandle;
	sXEvent.xclient.message_type = wmState;
	sXEvent.xclient.format       = 32;
	sXEvent.xclient.data.l[0]    = bFullscreen ? 1 : 0;	// The atom should be added (= 1) or removed (= 0)
	sXEvent.xclient.data.l[1]    = wmFullScreen;

	// Send message to the root window to inform the window manager about the change
	XSendEvent(m_pDisplay, DefaultRootWindow(m_pDisplay), False, SubstructureNotifyMask, &sXEvent);

	// Do it!
	XSync(m_pDisplay, False);

	// Inform that the fullscreen mode was changed (in here, usually e.g. the display resolution is updated and so on)
	// -> This has really be done "after" the OS window has been set to fullscreen, else we don't see the window content
	// (looks like the OS takes care of situations like changing the display resolution and updating the window dimension automatically)
	m_pFrontendOS->OnFullscreenMode();
}

void OSWindowLinux::RefreshFullscreen()
{
	// This information is only interesting if we're currently in fullscreen mode, if not, just ignore this method call
	if (m_pFrontendOS->IsFullscreen()) {
		// Set/remove _NET_WM_STATE_FULLSCREEN to toggle fullscreen mode.
		// The window manger is responsible to restore the original position and size of the window when the fullscreen mode should be left.
		// This works only on window manger which are EWMH (v1.3 or greater) compatible (http://standards.freedesktop.org/wm-spec/wm-spec-1.3.html)
		Atom wmFullScreen = XInternAtom(m_pDisplay, "_NET_WM_STATE_FULLSCREEN", False);
		Atom wmState = XInternAtom(m_pDisplay, "_NET_WM_STATE", False);

		// Setup the X-event
		XEvent sXEvent;
		memset(&sXEvent, 0, sizeof(sXEvent));
		sXEvent.type                 = ClientMessage;
		sXEvent.xclient.window       = m_nNativeWindowHandle;
		sXEvent.xclient.message_type = wmState;
		sXEvent.xclient.format       = 32;
		sXEvent.xclient.data.l[1]    = wmFullScreen;

		// Leave fullscreen - Send message to the root window to inform the window manager about the change
		sXEvent.xclient.data.l[0] = 0;	// The atom should be added (= 1) or removed (= 0)
		XSendEvent(m_pDisplay, DefaultRootWindow(m_pDisplay), False, SubstructureNotifyMask, &sXEvent);
		XSync(m_pDisplay, False); // Do it!

		// Inform that the fullscreen mode was changed (in here, usually e.g. the display resolution is updated and so on)
		m_pFrontendOS->OnFullscreenMode();

		// Reenter fullscreen - Send message to the root window to inform the window manager about the change
		sXEvent.xclient.data.l[0] = 1;	// The atom should be added (= 1) or removed (= 0)
		XSendEvent(m_pDisplay, DefaultRootWindow(m_pDisplay), False, SubstructureNotifyMask, &sXEvent);
		XSync(m_pDisplay, False); // Do it!
	}
}

bool OSWindowLinux::IsMouseOver() const
{
	return m_bIsMouseOver;
}

int OSWindowLinux::GetMousePositionX() const
{
	// Get the absolute mouse cursor position on the screen
	XEvent sXEvent;

	// Get the window directly below the current mouse cursor position - do only continue if this is our window
	if (XQueryPointer(m_pDisplay, m_nNativeWindowHandle, &sXEvent.xbutton.root, &sXEvent.xbutton.window,
		&sXEvent.xbutton.x_root, &sXEvent.xbutton.y_root, &sXEvent.xbutton.x, &sXEvent.xbutton.y, &sXEvent.xbutton.state) == True) {
		// Done
		return sXEvent.xbutton.x;
	} else {
		// Error, the mouse cursor is currently not over this window
		return -1;
	}
}

int OSWindowLinux::GetMousePositionY() const
{
	// Get the absolute mouse cursor position on the screen
	XEvent sXEvent;

	// Get the window directly below the current mouse cursor position - do only continue if this is our window
	if (XQueryPointer(m_pDisplay, m_nNativeWindowHandle, &sXEvent.xbutton.root, &sXEvent.xbutton.window,
		&sXEvent.xbutton.x_root, &sXEvent.xbutton.y_root, &sXEvent.xbutton.x, &sXEvent.xbutton.y, &sXEvent.xbutton.state) == True) {
		// Done
		return sXEvent.xbutton.y;
	} else {
		// Error, the mouse cursor is currently not over this window
		return -1;
	}
}

bool OSWindowLinux::IsMouseVisible() const
{
	return m_bMouseVisible;
}

void OSWindowLinux::SetMouseVisible(bool bVisible)
{
	// Backup the state
	m_bMouseVisible = bVisible;

	// Set the mouse cursor state
	if (bVisible)
		XUndefineCursor(m_pDisplay, m_nNativeWindowHandle);
	else
		XDefineCursor(m_pDisplay, m_nNativeWindowHandle, m_nInvisibleCursor);
}

void OSWindowLinux::SetTrapMouse(bool bTrap)
{
	// Check if the window has already been destroyed
	if (m_nNativeWindowHandle) {
		// Set or release mouse capture?
		if (bTrap) {
			// Grab mouse
			XGrabPointer(m_pDisplay, m_nNativeWindowHandle, True, 0, GrabModeAsync, GrabModeAsync, m_nNativeWindowHandle, XLib::None, CurrentTime);
		} else {
			// Release mouse grab
			XUngrabPointer(m_pDisplay, CurrentTime);
		}

		// Do it!
		XSync(m_pDisplay, False);
	}
}


//[-------------------------------------------------------]
//[ Private static functions                              ]
//[-------------------------------------------------------]
/**
*  @brief
*    Signal handler callback
*/
void OSWindowLinux::SignalHandler(int nSignal)
{
	// Catch signal
	switch (nSignal) {
		// Interrupt (exit application by ctrl-c)
		case SIGINT:
			// The OS asks us to shut down
			g_bSignalSystemQuit = true;

			// Signal handler has done it's job, re-raise signal
			signal(nSignal, SIG_DFL);
			raise(nSignal);
			break;

		// Terminate (exit application)
		case SIGTERM:
			// The OS asks us to shut down
			g_bSignalSystemQuit = true;

			// Signal handler has done it's job, re-raise signal
			signal(nSignal, SIG_DFL);
			raise(nSignal);
			break;
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLFrontendOS
