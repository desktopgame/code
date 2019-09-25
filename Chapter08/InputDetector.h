#pragma once
#include <SDL/SDL_scancode.h>
#include <SDL/SDL_gamecontroller.h>
#include <SDL/SDL_mouse.h>
#include <stdexcept>

class InputDetector {
public:
	explicit InputDetector() = default;
	virtual ~InputDetector() = default;
	virtual bool GetBoolValue(class InputState& state) const = 0;
	virtual enum ButtonState GetButtonValue(class InputState& state) const = 0;
	virtual float GetFloatValue(class InputState& state) const = 0;
	virtual struct Vector2 GetAxisValue(class InputState& state) const = 0;
private:
};

class KeyDetector : public InputDetector {
public:
	explicit KeyDetector(SDL_Scancode code);
	bool GetBoolValue(class InputState& state) const override;
	enum ButtonState GetButtonValue(class InputState& state) const override;
	float GetFloatValue(class InputState& state) const override;
	struct Vector2 GetAxisValue(class InputState& state) const override;
private:
	SDL_Scancode code;
};

class MouseDetector : public InputDetector {
public:
	explicit MouseDetector(int button);
	bool GetBoolValue(class InputState& state) const override;
	enum ButtonState GetButtonValue(class InputState& state) const override;
	float GetFloatValue(class InputState& state) const override;
	struct Vector2 GetAxisValue(class InputState& state) const override;
private:
	int button;
};

enum class ControllerDirection : int {
	Left,
	Right
};

class ControllerButtonDetector : public InputDetector {
public:
	explicit ControllerButtonDetector(SDL_GameControllerButton button);
	bool GetBoolValue(class InputState& state) const override;
	enum ButtonState GetButtonValue(class InputState& state) const override;
	float GetFloatValue(class InputState& state) const override;
	struct Vector2 GetAxisValue(class InputState& state) const override;
private:
	SDL_GameControllerButton button;
};

class ControllerStickDetector : public InputDetector {
public:
	explicit ControllerStickDetector(ControllerDirection direction);
	bool GetBoolValue(class InputState& state) const override;
	enum ButtonState GetButtonValue(class InputState& state) const override;
	float GetFloatValue(class InputState& state) const override;
	struct Vector2 GetAxisValue(class InputState& state) const override;
private:
	ControllerDirection direction;
};

class ControllerTriggerDetector : public InputDetector {
public:
	explicit ControllerTriggerDetector(ControllerDirection direction);
	bool GetBoolValue(class InputState& state) const override;
	enum ButtonState GetButtonValue(class InputState& state) const override;
	float GetFloatValue(class InputState& state) const override;
	struct Vector2 GetAxisValue(class InputState& state) const override;
private:
	ControllerDirection direction;

};