#include "InputDetector.h"
#include "InputSystem.h"
// KeyDetector
KeyDetector::KeyDetector(SDL_Scancode code) : code(code)
{
}

bool KeyDetector::GetBoolValue(const InputState & state) const
{
	return state.Keyboard.GetKeyValue(code);
}

ButtonState KeyDetector::GetButtonValue(const InputState & state) const
{
	return state.Keyboard.GetKeyState(code);
}

float KeyDetector::GetFloatValue(const InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 KeyDetector::GetAxisValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// MouseDetector
MouseDetector::MouseDetector(int button) : button(button)
{
}

bool MouseDetector::GetBoolValue(const InputState & state) const
{
	return state.Mouse.GetButtonValue(button);
}

ButtonState MouseDetector::GetButtonValue(const InputState & state) const
{
	return state.Mouse.GetButtonState(button);
}

float MouseDetector::GetFloatValue(const InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 MouseDetector::GetAxisValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// ControllerButtonDetector
ControllerButtonDetector::ControllerButtonDetector(SDL_GameControllerButton button) : button(button)
{
}

bool ControllerButtonDetector::GetBoolValue(const InputState & state) const
{
	return state.Controller.GetButtonValue(button);
}

ButtonState ControllerButtonDetector::GetButtonValue(const InputState & state) const
{
	return state.Controller.GetButtonState(button);
}

float ControllerButtonDetector::GetFloatValue(const InputState & state) const
{
	return GetBoolValue(state) ? 1 : 0;
}

Vector2 ControllerButtonDetector::GetAxisValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
// ControllerStickDetector
ControllerStickDetector::ControllerStickDetector(ControllerDirection direction) : direction(direction)
{
}

bool ControllerStickDetector::GetBoolValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

ButtonState ControllerStickDetector::GetButtonValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

float ControllerStickDetector::GetFloatValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

Vector2 ControllerStickDetector::GetAxisValue(const InputState & state) const
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

bool ControllerTriggerDetector::GetBoolValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

ButtonState ControllerTriggerDetector::GetButtonValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}

float ControllerTriggerDetector::GetFloatValue(const InputState & state) const
{
	if (direction == ControllerDirection::Left) {
		return state.Controller.GetLeftTrigger();
	} else {
		return state.Controller.GetRightTrigger();
	}
}

Vector2 ControllerTriggerDetector::GetAxisValue(const InputState & state) const
{
	throw std::logic_error("unsupported operation");
}
