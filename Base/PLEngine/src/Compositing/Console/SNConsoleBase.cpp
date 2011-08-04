/*********************************************************\
 *  File: SNConsoleBase.cpp                              *
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
#include <PLCore/Log/Log.h>
#include <PLGui/Gui/Gui.h>
#include <PLInput/Input/InputManager.h>
#include <PLInput/Input/Devices/Keyboard.h>
#include <PLScene/Scene/SceneContext.h>
#include "PLEngine/Application/EngineApplication.h"
#include "PLEngine/Compositing/Console/ConsoleDefaultCommands.h"
#include "PLEngine/Compositing/Console/ConsoleCommand.h"
#include "PLEngine/Compositing/Console/SNConsoleBase.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLGui;
using namespace PLInput;
namespace PLEngine {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SNConsoleBase)


//[-------------------------------------------------------]
//[ Public functions                                      ]
//[-------------------------------------------------------]
/**
*  @brief
*    Returns whether the console is active or not
*/
bool SNConsoleBase::IsActive() const
{
	return (m_nState == Active || m_nState == Activating);
}

/**
*  @brief
*    Activates the console
*/
void SNConsoleBase::Activate()
{
	m_nState = Activating;
}

/**
*  @brief
*    Deactivates the console
*/
void SNConsoleBase::Deactivate()
{
	m_nState = Deactivating;
}

/**
*  @brief
*    Returns the current console state
*/
SNConsoleBase::EState SNConsoleBase::GetState() const
{
	return m_nState;
}

/**
*  @brief
*    Clear the command history
*/
void SNConsoleBase::ClearCommandHistory()
{
	m_lstCommandHistory.Clear();
}

/**
*  @brief
*    Returns the console description
*/
String SNConsoleBase::GetDescription() const
{
	static const String sString = "Unknown console v1.0";
	return sString;
}

/**
*  @brief
*    Registers a new command
*/
bool SNConsoleBase::RegisterCommand(bool bDebug, const String &sCommand, const String &sParameters,
									const String &sHelp, const Functor<void, ConsoleCommand &> &cFunctor)
{
	ConsoleCommand cCommand(bDebug, sCommand, sParameters, sHelp, cFunctor);
	return RegisterCommand(cCommand);
}

/**
*  @brief
*    Register a new command
*/
bool SNConsoleBase::RegisterCommand(const ConsoleCommand &cCommand)
{
	// Check the command parameters
	if (cCommand.HasValidParams()) {
		// Is there already such a command?
		if (!m_mapCommands.Get(cCommand.GetCommand())) {
			// Register the new command
			ConsoleCommand *pCommand = new ConsoleCommand(cCommand);
			pCommand->m_pConsole = this;
			m_lstCommands.Add(pCommand); 
			m_mapCommands.Add(pCommand->GetCommand(), pCommand);

			// Done
			return true;
		} else {
			PL_LOG(Error, "Command '" + cCommand.GetCommand() + "' is already registered!")
		}
	} else {
		PL_LOG(Error, "Command '" + cCommand.GetCommand() + "' has invalid parameter types ('" + cCommand.GetParameters() + "') [command not registered!]")
	}

	// Error!
	return false;
}

/**
*  @brief
*    Unregisters a command
*/
bool SNConsoleBase::UnRegisterCommand(const String &sCommand)
{
	// Get the command
	ConsoleCommand *pCommand = m_mapCommands.Get(sCommand);
	if (!pCommand)
		return false; // Error!

	// Remove the command
	m_mapCommands.Remove(sCommand);
	m_lstCommands.Remove(pCommand);
	delete pCommand;

	// Done
	return true;
}

/**
*  @brief
*    Unregisters all commands
*/
void SNConsoleBase::UnRegisterAllCommands()
{
	for (uint32 i=0; i<m_lstCommands.GetNumOfElements(); i++)
		delete m_lstCommands[i];
	m_lstCommands.Clear();
	m_mapCommands.Clear();
}

/**
*  @brief
*    Returns the number of registered commands
*/
uint32 SNConsoleBase::GetNumOfCommands() const
{
	// Allow debug commands?
	if (GetFlags() & NoDebugCommands) {
		uint32 nCommands = 0;

		// Filter out debug commands
		for (uint32 i=0; i<m_lstCommands.GetNumOfElements(); i++) {
			if (!m_lstCommands[i]->IsDebug())
				nCommands++;
		}

		// Return the number of commands
		return nCommands;
	} else {
		return m_lstCommands.GetNumOfElements();
	}
}

/**
*  @brief
*    Returns the number of parameters in a given string
*/
uint32 SNConsoleBase::GetNumOfParamsInString(const String &sString) const
{
	// Remove all whitespace (tabs and spaces) at the beginning and the end of the given string
	String sCopy = sString;
	sCopy.Trim();

	// Count the number of spaces
	uint32 nNumOfSpaces = 0;
	uint32 nLength      = sCopy.GetLength();
	for (uint32 i=0; i<nLength; i++) {
		// Is this a tab or space?
		if (sCopy[i] == '	' || sCopy[i] == ' ')
			nNumOfSpaces++;
	}

	// Return the found number of spaces
	return nNumOfSpaces;
}

/**
*  @brief
*    Process a keyboard message
*/
void SNConsoleBase::ProcessKeyMessage()
{
	// Check if input is active
	// [TODO] Don't use devices directly, use a virtual controller instead
	ConsoleApplication *pConsoleApplication = ConsoleApplication::GetApplication();
	if (!pConsoleApplication || !pConsoleApplication->IsInstanceOf("PLEngine::EngineApplication"))
		return;	// Get us out of here right now!
	Controller *pController = reinterpret_cast<Controller*>(static_cast<EngineApplication*>(pConsoleApplication)->GetInputController());
	if ((pController && pController->GetActive()) || !pController) {
		// Get keyboard input device
		Keyboard *pKeyboard = InputManager::GetInstance()->GetKeyboard();
		if (pKeyboard) {
			// Check if console can be activated at all
			// [TODO] Don't use an external config here, rather have a deactivated-flag *inside* this class
			//        and set it from *outside* using the config! This is spaghetti logic...
			bool bActivated = true;
			ConsoleApplication *pApplication = ConsoleApplication::GetApplication();
			if (pApplication) {
				bActivated = pApplication->GetConfig().GetVar("PLEngine::EngineGeneralConfig", "ConsoleActivated").GetBool();
			}

			// Console toggle
			if (pKeyboard->KeyCircumflex.IsHit()) {
				if (IsActive() || !bActivated)
					Deactivate();
				else
					Activate();

				// Done
				return;
			}

			// Only process keyboard if the console is active
			if (m_nState == Inactive || m_nState == Deactivating)
				return;

			// Clear command
			if (pKeyboard->KeyEscape.IsHit()) {
				m_sCommand      = "";
				m_sFoundCommand = "";
				m_nCursor = 0;
				m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}

			// Set cursor
			if (pKeyboard->KeyLeft.IsHit()) {
				// Check selection start
				if (pKeyboard->KeyShift.IsPressed() && m_nSelStart < 0)
					m_nSelStart = m_nCursor; // Selection start

				// Update current cursor position
				if (m_nCursor > 0)
					m_nCursor--;

				// Check selection end
				if (pKeyboard->KeyShift.IsPressed())
					m_nSelEnd   = m_nCursor;
				else
					m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}
			if (pKeyboard->KeyRight.IsHit()) {
				// Check selection start
				if (pKeyboard->KeyShift.IsPressed() && m_nSelStart < 0)
					m_nSelStart = m_nCursor; // Selection start

				// Update current cursor position
				m_nCursor++;
				if (m_nCursor > m_sCommand.GetLength())
					m_nCursor = m_sCommand.GetLength();

				// Check selection end
				if (pKeyboard->KeyShift.IsPressed())
					m_nSelEnd   = m_nCursor;
				else
					m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}
			if (pKeyboard->KeyHome.IsHit()) {
				// Check selection start
				if (pKeyboard->KeyShift.IsPressed() && m_nSelStart < 0)
					m_nSelStart = m_nCursor; // Selection start

				// Update current cursor position
				m_nCursor = 0;

				// Check selection end
				if (pKeyboard->KeyShift.IsPressed())
					m_nSelEnd   = m_nCursor;
				else
					m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}
			if (pKeyboard->KeyEnd.IsHit()) {
				// Check selection start
				if (pKeyboard->KeyShift.IsPressed() && m_nSelStart < 0)
					m_nSelStart = m_nCursor; // Selection start

				// Update current cursor position
				m_nCursor = m_sCommand.GetLength();

				// Check selection end
				if (pKeyboard->KeyShift.IsPressed())
					m_nSelEnd   = m_nCursor;
				else
					m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}

			// Remove the selected text
			if (pKeyboard->KeyBack.IsHit()) {
				if (m_nSelStart < 0 || m_nSelEnd < 0) {
					if (m_nCursor > 0) {
						m_sCommand.Delete(m_nCursor-1, 1);
						m_nCursor--;

						// Complete the command automatically...
						CompleteCommand();
					}
				} else {
					if (m_nSelStart > m_nSelEnd) {
						m_sCommand.Delete(m_nSelEnd, m_nSelStart-m_nSelEnd);
						m_nCursor = m_nSelEnd;
					} else {
						m_sCommand.Delete(m_nSelStart, m_nSelEnd-m_nSelStart);
						m_nCursor = m_nSelStart;
					}
					m_nSelStart = m_nSelEnd = -1;

					// Complete the command automatically...
					CompleteCommand();
				}

				// Done
				return;
			}

			// Complete command
			if (pKeyboard->KeyTab.IsHit()) {
				if (m_sFoundCommand.GetLength()) {
					m_sCommand = m_sFoundCommand;
					m_nCursor  = m_sCommand.GetLength();
					m_nSelStart = m_nSelEnd = -1;
				}

				// Done
				return;
			}

			// Get text from clipboard
			if ( (pKeyboard->KeyControl.IsPressed() && pKeyboard->KeyV.IsHit()) ||
				 (pKeyboard->KeyShift  .IsPressed() && pKeyboard->KeyInsert.IsHit()) )
			{
				// Remove the selected text
				if (m_nSelStart > -1 && m_nSelEnd > -1) {
					if (m_nSelStart > m_nSelEnd) {
						m_sCommand.Delete(m_nSelEnd, m_nSelStart-m_nSelEnd);
						m_nCursor = m_nSelEnd;
					} else {
						m_sCommand.Delete(m_nSelStart, m_nSelEnd-m_nSelStart);
						m_nCursor = m_nSelStart;
					}
					m_nSelStart = m_nSelEnd = -1;
				}

				// Get text from clipboard
				String sClipboardText = Gui::GetSystemGui()->GetClipBoard().GetText();
				if (sClipboardText.GetLength()) {
					// Add text from clipboard
					m_sCommand.Insert(sClipboardText, m_nCursor);
					m_nCursor += sClipboardText.GetLength();
				}

				// Done
				return;
			}

			// Copy text to clipboard
			if ( (pKeyboard->KeyControl.IsPressed() && pKeyboard->KeyC.IsHit()) ||
				 (pKeyboard->KeyControl.IsPressed() && pKeyboard->KeyX.IsHit()) ||
				 (pKeyboard->KeyShift  .IsPressed() && pKeyboard->KeyDelete.IsHit()) )
			{
				// Copy the selected text to clipboard
				if (m_nSelStart > m_nSelEnd)
					Gui::GetSystemGui()->GetClipBoard().SetText(m_sCommand.GetSubstring(m_nSelEnd, m_nSelStart-m_nSelEnd));
				else
					Gui::GetSystemGui()->GetClipBoard().SetText(m_sCommand.GetSubstring(m_nSelStart, m_nSelEnd-m_nSelStart));

				// Remove selected text?
				if ( ( (pKeyboard->KeyControl.IsPressed() && pKeyboard->KeyX.IsHit()) ||
					   (pKeyboard->KeyShift  .IsPressed() && pKeyboard->KeyDelete.IsHit()) ) &&
						m_nSelStart > -1 && m_nSelEnd > -1 )
				{
					if (m_nSelStart > m_nSelEnd) {
						m_sCommand.Delete(m_nSelEnd, m_nSelStart-m_nSelEnd);
						m_nCursor = m_nSelEnd;
					} else {
						m_sCommand.Delete(m_nSelStart, m_nSelEnd-m_nSelStart);
						m_nCursor = m_nSelStart;
					}
					m_nSelStart = m_nSelEnd = -1;
				}

				// Done
				return;
			}

			// Select a command from the command history
			if (!m_lstCommandHistory.IsEmpty()) {
				// Go upwards
				if (pKeyboard->KeyUp.IsHit()) {
					if (m_nCurrentCommand > 0)
						m_nCurrentCommand--;
					m_sCommand = m_lstCommandHistory[m_nCurrentCommand];
					m_nCursor = 0;
					m_nSelStart = m_nSelEnd = -1;

					// Complete the command automatically...
					CompleteCommand();

					// Done
					return;
				}

				// Go downstairs
				if (pKeyboard->KeyDown.IsHit() && m_nCurrentCommand < m_lstCommandHistory.GetNumOfElements()) {
					m_nCurrentCommand++;
					m_sCommand = m_lstCommandHistory[m_nCurrentCommand];
					m_nCursor = 0;
					m_nSelStart = m_nSelEnd = -1;

					// Complete the command automatically...
					CompleteCommand();

					// Done
					return;
				}

				// Remove a command history entry
				if (pKeyboard->KeyDelete.IsHit()) {
					m_sCommand      = "";
					m_sFoundCommand = "";
					if (m_nCurrentCommand < m_lstCommandHistory.GetNumOfElements()) {
						m_lstCommandHistory.RemoveAtIndex(m_nCurrentCommand);
						if (m_nCurrentCommand > 0) {
							m_nCurrentCommand--;
							m_sCommand = m_lstCommandHistory[m_nCurrentCommand];
						}
					}
					m_nCursor = 0;
					m_nSelStart = m_nSelEnd = -1;

					// Done
					return;
				}
			} else {
				if (pKeyboard->KeyDelete.IsHit()) {
					// Clear command line
					m_sCommand      = "";
					m_sFoundCommand = "";
					m_nCursor = 0;
					m_nSelStart = m_nSelEnd = -1;

					// Done
					return;
				}
			}

			// Execute command
			if (pKeyboard->KeyReturn.IsHit()) {
				// Add command line to the history
				m_lstCommandHistory.Add(m_sCommand);
				m_nCurrentCommand = m_lstCommandHistory.GetNumOfElements();

				// Execute command itself
				ProcessCommand();

				// Clear command line
				m_sCommand      = "";
				m_sFoundCommand = "";
				m_nCursor = 0;
				m_nSelStart = m_nSelEnd = -1;

				// Done
				return;
			}

			// Loop through all keyboard buttons
			for (uint32 i=0; i<pKeyboard->GetButtons().GetNumOfElements(); i++) {
				// Get the button
				Button *pButton = pKeyboard->GetButtons()[i];

				// Was this button hit?
				if (pButton->IsHit()) {
					// Get the character of the button
					String sButtonName = pButton->GetCharacter();

					// The character must have at least a length of 1
					if (sButtonName.GetLength() == 1) {
						// Ensure the button name is lower or upper case depending on whether or not the shift key is currently pressed
						if (pKeyboard->KeyShift.IsPressed())
							sButtonName.ToUpper();
						else
							sButtonName.ToLower();

						// Remove the selected text
						if (m_nSelStart > -1 && m_nSelEnd > -1) {
							if (m_nSelStart > m_nSelEnd) {
								m_sCommand.Delete(m_nSelEnd, m_nSelStart-m_nSelEnd);
								m_nCursor = m_nSelEnd;
							} else {
								m_sCommand.Delete(m_nSelStart, m_nSelEnd-m_nSelStart);
								m_nCursor = m_nSelStart;
							}
							m_nSelStart = m_nSelEnd = -1;
						}

						// Add character to command line
						m_sCommand.Insert(sButtonName, m_nCursor);
						m_nCursor++;

						// Complete the command automatically...
						CompleteCommand();
					}
				}
			}
		}
	}
}

/**
*  @brief
*    Process a command
*/
void SNConsoleBase::ProcessCommand(const String &sCommand)
{
	// Get the command to process
	String sProcessCommand = sCommand.GetLength() ? sCommand : m_sCommand;

	// Remove all whitespace (tabs and spaces) at the beginning and the end of the given command
	sProcessCommand.Trim();

	// Print command
	PL_LOG(Always, sProcessCommand)

	// Find a empty space within the command
	String sCommandName;
	int nIndex = sProcessCommand.IndexOf(' ');
	if (nIndex < 0)
		nIndex = sProcessCommand.IndexOf('	');
	if (nIndex < 0) {
		// No space found, so no parameters
		sCommandName = sProcessCommand;
	} else {
		// Get command name
		sCommandName = sProcessCommand.GetSubstring(0, nIndex);
	}

	// Check the command
	ConsoleCommand *pCommand = m_mapCommands.Get(sCommandName);
	if (pCommand) {
		// Check debug command
		if ((GetFlags() & NoDebugCommands) && pCommand->IsDebug())
			return;	// Avoid cheating, please! ;-)

		// Get parameters
		String sParameters;
		if (nIndex > 0) {
			// Get the command parameters
			sParameters = sProcessCommand.GetSubstring(nIndex+1);

			// Create command variables
			pCommand->CreateVarsFromString(sParameters);
		}

		// Check the number of parameters
		if (pCommand->GetNumOfParams()) {
			uint32 nNumOfParameters = GetNumOfParamsInString(sProcessCommand);
			if (nNumOfParameters < pCommand->GetNumOfParams()) {
				PL_LOG(Error, String("This command requires ") + pCommand->GetNumOfParams() + " parameter(s), but only " + nNumOfParameters + " given!")

				// Done
				return;
			}
		}

		// Process the command
		ProcessCommand(*pCommand);
	} else {
		PL_LOG(Error, "Unknown command: " + sCommandName)
	}
}

/**
*  @brief
*    Prints a list of all available console commands into the log
*/
void SNConsoleBase::List(bool bDetailed)
{
	for (uint32 i=0; i<GetNumOfCommands(); i++) {
		ConsoleCommand *pCommand = m_lstCommands[i];

		// Is this a debug command?
		if (!(GetFlags() & NoDebugCommands) || !pCommand->IsDebug()) {
			// Is there is help available?
			if (bDetailed && pCommand->GetHelp().GetLength()) {
				// Display the usage
				if (pCommand->IsDebug())
					PL_LOG(Info, "Usage: " + pCommand->GetCommand() + ' ' + pCommand->GetHelp() + " (debug command)")
				else
					PL_LOG(Info, "Usage: " + pCommand->GetCommand() + ' ' + pCommand->GetHelp())

				// Display the syntax
				String sSyntax = "Syntax: " + pCommand->GetCommand();
				for (uint32 p=0; p<pCommand->GetNumOfParams(); p++) {
					switch (pCommand->GetParamType(p)) {
						case ConsoleCommand::ParamFloat:
							sSyntax += " <float>";
							break;

						case ConsoleCommand::ParamInt:
							sSyntax += " <int>";
							break;

						case ConsoleCommand::ParamString:
							sSyntax += " <string>";
							break;

						case ConsoleCommand::ParamBool:
							sSyntax += " <bool>";
							break;

						default:
							sSyntax += " <unknown type>";
							break;
					}
				}
				PL_LOG(Info, sSyntax)
			} else {
				PL_LOG(Info, pCommand->GetCommand())
			}
		}
	}
	PL_LOG(Info, String::Format("Total: %d commands", GetNumOfCommands()))
	Log::GetInstance()->Flush();
}


//[-------------------------------------------------------]
//[ Protected functions                                   ]
//[-------------------------------------------------------]
/**
*  @brief
*    Default constructor
*/
SNConsoleBase::SNConsoleBase() :
	Flags(this),
	m_nState(Inactive),
	m_nCursor(0),
	m_nSelStart(-1),
	m_nSelEnd(-1),
	m_nCurrentCommand(0)
{
	// Overwrite the default setting of the flags
	SetFlags(GetFlags()|NoCulling);

	// Register default commands
	//				debug	prefix				parameters	help string		execution functor
	RegisterCommand(0,		"help",				"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandHelp));
	RegisterCommand(0,		"list",				"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandList));
	RegisterCommand(0,		"bulkylist",		"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandBulkyList));
	RegisterCommand(0,		"clear",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandClear));
	RegisterCommand(0,		"about",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandAbout));
	RegisterCommand(0,		"version",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandVersion));
	RegisterCommand(0,		"fps",				"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandFPS));
	RegisterCommand(0,		"fpslimit",			"F",		"<FPS>",		Functor<void, ConsoleCommand &>(PLConsoleCommandFPSLimit));
	RegisterCommand(0,		"profiling",		"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandProfiling));
	RegisterCommand(0,		"nextprofile",		"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandNextProfile));
	RegisterCommand(0,		"preprofile",		"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandPreviousProfile));

	// Debug commands
	RegisterCommand(1,		"coordinateaxis",	"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandCoordinateAxis));
	RegisterCommand(1,		"xzplane",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandXZPlane));
	RegisterCommand(1,		"xyplane",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandXYPlane));
	RegisterCommand(1,		"yzplane",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandYZPlane));
	RegisterCommand(1,		"setloglevel",		"S",		"<log level>",	Functor<void, ConsoleCommand &>(PLConsoleCommandSetLogLevel));
	RegisterCommand(1,		"pause",			"",			"",				Functor<void, ConsoleCommand &>(PLConsoleCommandPause));
	RegisterCommand(1,		"timescale",		"F",		"<factor>",		Functor<void, ConsoleCommand &>(PLConsoleCommandTimeScale));
}

/**
*  @brief
*    Destructor
*/
SNConsoleBase::~SNConsoleBase()
{
	ClearCommandHistory();
	UnRegisterAllCommands();
}

/**
*  @brief
*    Process the syntactical correct command
*/
void SNConsoleBase::ProcessCommand(ConsoleCommand &cCommand)
{
	// Execute it! :)
	cCommand.GetFunctor()(cCommand);
}

/**
*  @brief
*    Completes the command
*/
void SNConsoleBase::CompleteCommand()
{
	// Reset the found command
	m_sFoundCommand = "";

	// Loop through all registered commands
	uint32 nEqualCharacters = 0;
	for (uint32 nCommand=0; nCommand<m_lstCommands.GetNumOfElements(); nCommand++) {
		ConsoleCommand *pCommand = m_lstCommands[nCommand];

		// Is this a debug command? (Avoid cheating, please! ;-)
		if (!pCommand->IsDebug() || !(GetFlags() & NoDebugCommands)) {
			// Get the number of characters to compare
			uint32 nLength = m_sCommand.GetLength();
			String sCommand = pCommand->GetCommand();
			if (nLength > sCommand.GetLength())
				nLength = sCommand.GetLength();

			// Get the number of equal characers
			uint32 nCharacers = 0;
			for (; nCharacers<nLength; nCharacers++) {
				if (m_sCommand.GetSubstring(nCharacers, 1) != sCommand.GetSubstring(nCharacers, 1))
					break;
			}

			// Did this command fit in better?
			if (nEqualCharacters < nCharacers) {
				nEqualCharacters = nCharacers;
				m_sFoundCommand = sCommand;
			}
		}
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLEngine
