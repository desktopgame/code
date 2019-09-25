// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem.h"
#include <SDL/SDL.h>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "InputDetector.h"

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
	return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
	if (mPrevState[keyCode] == 0)
	{
		if (mCurrState[keyCode] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrState[keyCode] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool MouseState::GetButtonValue(int button) const
{
	return (SDL_BUTTON(button) & mCurrButtons) == 1;
}

ButtonState MouseState::GetButtonState(int button) const
{
	int mask = SDL_BUTTON(button);
	if ((mask & mPrevButtons) == 0)
	{
		if ((mask & mCurrButtons) == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else
	{
		if ((mask & mCurrButtons) == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
	return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
	if (mPrevButtons[button] == 0)
	{
		if (mCurrButtons[button] == 0)
		{
			return ENone;
		}
		else
		{
			return EPressed;
		}
	}
	else // Prev state must be 1
	{
		if (mCurrButtons[button] == 0)
		{
			return EReleased;
		}
		else
		{
			return EHeld;
		}
	}
}
InputState::InputState()
{
	cButtons[""] = SDL_CONTROLLER_BUTTON_INVALID;
	cButtons["A"] = SDL_CONTROLLER_BUTTON_A;
	cButtons["B"] = SDL_CONTROLLER_BUTTON_B;
	cButtons["X"] = SDL_CONTROLLER_BUTTON_X;
	cButtons["Y"] = SDL_CONTROLLER_BUTTON_Y;
	cButtons["Back"] = SDL_CONTROLLER_BUTTON_BACK;
	cButtons["Guide"] = SDL_CONTROLLER_BUTTON_GUIDE;
	cButtons["Start"] = SDL_CONTROLLER_BUTTON_START;
	cButtons["LeftStick"] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
	cButtons["RightStick"] = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
	cButtons["LeftShoulder"] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
	cButtons["RightShoulder"] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
	cButtons["DPadUp"] = SDL_CONTROLLER_BUTTON_DPAD_UP;
	cButtons["DPadDown"] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
	cButtons["DPadLeft"] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	cButtons["DPadRight"] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
	cButtons[""] = SDL_CONTROLLER_BUTTON_MAX;
}
InputState::~InputState()
{
	for (auto kv : bindings) {
		delete kv.second;
	}
	ClearBinding();
}
// InputState
bool InputState::GetBoolValue(const std::string & name) const
{
	return bindings.at(name)->GetBoolValue(Self());
}

ButtonState InputState::GetButtonValue(const std::string & name) const
{
	return bindings.at(name)->GetButtonValue(Self());
}

float InputState::GetFloatValue(const std::string & name) const
{
	return bindings.at(name)->GetFloatValue(Self());
}

Vector2 InputState::GetAxisValue(const std::string & name) const
{
	return bindings.at(name)->GetAxisValue(Self());
}

void InputState::ParseBindingFromFile(const std::string & path)
{
	std::ifstream ifs(path);
	std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ParseBindingFromString(str);
}

void InputState::ParseBindingFromString(const std::string & source)
{
	ClearBinding();
	const char* sentence = source.c_str();
	std::stringstream ss(sentence);
	std::string to;

	if (sentence != NULL)
	{
		while (std::getline(ss, to, '\n')) {
			AddBinding(to);
		}
	}
}

void InputState::AddBinding(const std::string & oneline)
{
	std::vector<std::string> words;
	split(oneline, words, ' ');
	std::string name = words[0];
	std::string type = words[1];
	if (type == "Key") {
		PutBinding(name, new KeyDetector((SDL_Scancode)words[2].at(0)));
	}
	else if (type == "Mouse") {
		if (words[2] == "Left") {
			PutBinding(name, new MouseDetector(SDL_BUTTON_LEFT));
		}
		else if (words[2] == "Right") {
			PutBinding(name, new MouseDetector(SDL_BUTTON_RIGHT));
		}
		else {
			throw std::logic_error("unknown mouse type: " + words[2]);
		}
	}
	else if (type == "ControllerButton") {
		PutBinding(name, new ControllerButtonDetector(cButtons[words[2]]));
	}
	else if (type == "ControllerStick") {
		if (words[2] == "Left") {
			PutBinding(name, new ControllerStickDetector(ControllerDirection::Left));
		}
		else if (words[2] == "Right") {
			PutBinding(name, new ControllerStickDetector(ControllerDirection::Right));
		}
		else {
			throw std::logic_error("unknown stick type: " + words[2]);
		}
	}
	else if (type == "ControllerTrigger") {
		if (words[2] == "Left") {
			PutBinding(name, new ControllerTriggerDetector(ControllerDirection::Left));
		}
		else if (words[2] == "Right") {
			PutBinding(name, new ControllerTriggerDetector(ControllerDirection::Right));
		}
		else {
			throw std::logic_error("unknown stick type: " + words[2]);
		}
	}
	else {
		throw std::logic_error("unsupported input type: " + type);
	}
}

void InputState::PutBinding(const std::string & key, InputDetector * detector)
{
	bindings[key] = detector;
}

void InputState::RemoveBinding(const std::string & key)
{
	bindings.erase(key);
}

bool InputState::HasBinding(const std::string & key) const
{
	return bindings.count(key) > 0;
}

void InputState::ClearBinding()
{
	bindings.clear();
}

const InputState & InputState::Self() const
{
	return *this;
}

size_t InputState::split(const std::string & txt, std::vector<std::string>& strs, char ch)
{
	//https://stackoverflow.com/questions/5888022/split-string-by-single-spaces
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	return strs.size();
}


// InputSystem
bool InputSystem::Initialize(const std::string& bindingFile)
{
	// Keyboard
	// Assign current state pointer
	mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
	// Clear previous state memory
	memset(mState.Keyboard.mPrevState, 0,
		SDL_NUM_SCANCODES);

	// Mouse (just set everything to 0)
	mState.Mouse.mCurrButtons = 0;
	mState.Mouse.mPrevButtons = 0;

	// Get the connected controller, if it exists
	mController = SDL_GameControllerOpen(0);
	// Initialize controller state
	mState.Controller.mIsConnected = (mController != nullptr);
	memset(mState.Controller.mCurrButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
	memset(mState.Controller.mPrevButtons, 0,
		SDL_CONTROLLER_BUTTON_MAX);
	mState.ParseBindingFromFile(bindingFile);
	return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
	// Copy current state to previous
	// Keyboard
	memcpy(mState.Keyboard.mPrevState,
		mState.Keyboard.mCurrState,
		SDL_NUM_SCANCODES);

	// Mouse
	mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
	mState.Mouse.mIsRelative = false;
	mState.Mouse.mScrollWheel = Vector2::Zero;

	// Controller
	memcpy(mState.Controller.mPrevButtons,
		mState.Controller.mCurrButtons,
		SDL_CONTROLLER_BUTTON_MAX);
}

void InputSystem::Update()
{
	// Mouse
	int x = 0, y = 0;
	if (mState.Mouse.mIsRelative)
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		mState.Mouse.mCurrButtons = 
			SDL_GetMouseState(&x, &y);
	}

	mState.Mouse.mMousePos.x = static_cast<float>(x);
	mState.Mouse.mMousePos.y = static_cast<float>(y);

	// Controller
	// Buttons
	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	{
		mState.Controller.mCurrButtons[i] =
			SDL_GameControllerGetButton(mController, 
				SDL_GameControllerButton(i));
	}

	// Triggers
	mState.Controller.mLeftTrigger =
		Filter1D(SDL_GameControllerGetAxis(mController,
			SDL_CONTROLLER_AXIS_TRIGGERLEFT));
	mState.Controller.mRightTrigger =
		Filter1D(SDL_GameControllerGetAxis(mController,
			SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

	// Sticks
	x = SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_LEFTX);
	y = -SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_LEFTY);
	mState.Controller.mLeftStick = Filter2D(x, y);

	x = SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_RIGHTX);
	y = -SDL_GameControllerGetAxis(mController,
		SDL_CONTROLLER_AXIS_RIGHTY);
	mState.Controller.mRightStick = Filter2D(x, y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
	switch (event.type)
	{
	case SDL_MOUSEWHEEL:
		mState.Mouse.mScrollWheel = Vector2(
			static_cast<float>(event.wheel.x),
			static_cast<float>(event.wheel.y));
		break;
	default:
		break;
	}
}

void InputSystem::SetRelativeMouseMode(bool value)
{
	SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
	SDL_SetRelativeMouseMode(set);

	mState.Mouse.mIsRelative = value;
}

float InputSystem::Filter1D(int input)
{
	// A value < dead zone is interpreted as 0%
	const int deadZone = 250;
	// A value > max value is interpreted as 100%
	const int maxValue = 30000;

	float retVal = 0.0f;

	// Take absolute value of input
	int absValue = input > 0 ? input : -input;
	// Ignore input within dead zone
	if (absValue > deadZone)
	{
		// Compute fractional value between dead zone and max value
		retVal = static_cast<float>(absValue - deadZone) /
		(maxValue - deadZone);
		// Make sure sign matches original value
		retVal = input > 0 ? retVal : -1.0f * retVal;
		// Clamp between -1.0f and 1.0f
		retVal = Math::Clamp(retVal, -1.0f, 1.0f);
	}

	return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
	const float deadZone = 8000.0f;
	const float maxValue = 30000.0f;

	// Make into 2D vector
	Vector2 dir;
	dir.x = static_cast<float>(inputX);
	dir.y = static_cast<float>(inputY);

	float length = dir.Length();

	// If length < deadZone, should be no input
	if (length < deadZone)
	{
		dir = Vector2::Zero;
	}
	else
	{
		// Calculate fractional value between
		// dead zone and max value circles
		float f = (length - deadZone) / (maxValue - deadZone);
		// Clamp f between 0.0f and 1.0f
		f = Math::Clamp(f, 0.0f, 1.0f);
		// Normalize the vector, and then scale it to the
		// fractional value
		dir *= f / length;
	}

	return dir;
}
