#include "InputDetector.h"
#include "InputSystem.h"
// KeyDetector
KeyDetector::KeyDetector(SDL_Scancode code) : code(code)
{
}

bool KeyDetector::GetBoolValue(InputState & state) const
{
	return state.Keyboard.GetKeyValue(code);
}

ButtonState KeyDetector::GetButtonValue(InputState & state) const
{
	return state.Keyboard.GetKeyState(code);
}

float KeyDetector::GetFloatValue(InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 KeyDetector::GetAxisValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// MouseDetector
MouseDetector::MouseDetector(int button) : button(button)
{
}

bool MouseDetector::GetBoolValue(InputState & state) const
{
	return state.Mouse.GetButtonValue(button);
}

ButtonState MouseDetector::GetButtonValue(InputState & state) const
{
	return state.Mouse.GetButtonState(button);
}

float MouseDetector::GetFloatValue(InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 MouseDetector::GetAxisValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// ControllerButtonDetector
ControllerButtonDetector::ControllerButtonDetector(SDL_GameControllerButton button) : button(button)
{
}

bool ControllerButtonDetector::GetBoolValue(InputState & state) const
{
	return state.Controller.GetButtonValue(button);
}

ButtonState ControllerButtonDetector::GetButtonValue(InputState & state) const
{
	return state.Controller.GetButtonState(button);
}

float ControllerButtonDetector::GetFloatValue(InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 ControllerButtonDetector::GetAxisValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// ControllerStickDetector
ControllerStickDetector::ControllerStickDetector(ControllerDirection direction) : direction(direction)
{
}

bool ControllerStickDetector::GetBoolValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

ButtonState ControllerStickDetector::GetButtonValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

float ControllerStickDetector::GetFloatValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

Vector2 ControllerStickDetector::GetAxisValue(InputState & state) const
{
	if (direction == ControllerDirection::Left) {
		return state.Controller.GetLeftStick();
	} else {
		return state.Controller.GetRightStick();
	}
}

ControllerTriggerDetector::ControllerTriggerDetector(ControllerDirection direction) : direction(direction)
{
}

bool ControllerTriggerDetector::GetBoolValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

ButtonState ControllerTriggerDetector::GetButtonValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

float ControllerTriggerDetector::GetFloatValue(InputState & state) const
{
	if (direction == ControllerDirection::Left) {
		return state.Controller.GetLeftTrigger();
	} else {
		return state.Controller.GetRightTrigger();
	}
}

Vector2 ControllerTriggerDetector::GetAxisValue(InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
