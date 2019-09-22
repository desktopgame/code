// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Laser.h"
#include "CircleComponent.h"
#include "Asteroid.h"

Ship::Ship(Game* game)
	:Actor(game)
	,mLaserCooldown(0.0f)
	,mElapsed(0.0f)
	,mIsDead(false)
	,mDeadTime(0.0f)
{
	// Create a sprite component
	this->mSprite = new SpriteComponent(this, 150);
	mSprite->SetTexture(game->GetTexture("Assets/Ship.png"));

	// Create an input component and set keys/speed
	InputComponent* ic = new InputComponent(this);
	ic->SetForwardKey(SDL_SCANCODE_W);
	ic->SetBackKey(SDL_SCANCODE_S);
	ic->SetClockwiseKey(SDL_SCANCODE_A);
	ic->SetCounterClockwiseKey(SDL_SCANCODE_D);
	ic->SetMaxForwardSpeed(300.0f);
	ic->SetMaxAngularSpeed(Math::TwoPi);

	this->mCircle = new CircleComponent(this);
	mCircle->SetRadius(32.0f);
}

void Ship::UpdateActor(float deltaTime)
{
	if (mIsDead) {
		mDeadTime += deltaTime;
		if (mDeadTime > 1.0f) {
			mSprite->SetTexture(GetGame()->GetTexture("Assets/Ship.png"));
			SetPosition(GetRespawnPosition());
			SetRotation(GetRespawnRotation());
			this->mIsDead = false;
		}
		return;
	}
	mLaserCooldown -= deltaTime;
	if (mElapsed < 1.0f) {
		this->mElapsed += deltaTime;
		return;
	}
	// Do we intersect with an asteroid?
	for (auto ast : GetGame()->GetAsteroids())
	{
		if (Intersect(*mCircle, *(ast->GetCircle())))
		{
			// The first asteroid we intersect with,
			// set ourselves and the asteroid to dead
			mSprite->SetTexture(nullptr);
			ast->SetState(EDead);
			this->mElapsed = 0.0f;
			this->mDeadTime = 0.0f;
			this->mIsDead = true;
			break;
		}
	}
}

void Ship::ActorInput(const uint8_t* keyState)
{
	if (mIsDead) {
		return;
	}
	if (keyState[SDL_SCANCODE_SPACE] && mLaserCooldown <= 0.0f)
	{
		// Create a laser and set its position/rotation to mine
		Laser* laser = new Laser(GetGame());
		laser->SetPosition(GetPosition());
		laser->SetRotation(GetRotation());

		// Reset laser cooldown (half second)
		mLaserCooldown = 0.5f;
	}
}

void Ship::SetRespawnPosition(const Vector2 respawnPosition) {
	this->mRespawnPosition = respawnPosition;
}

Vector2 Ship::GetRespawnPosition() const {
	return mRespawnPosition;
}

void Ship::SetRespawnRotation(const float rotation) {
	this->mRespawnRotation = rotation;
}

float Ship::GetRespawnRotation() const {
	return mRespawnRotation;
}
