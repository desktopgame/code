// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Actor.h"
class Ship : public Actor
{
public:
	Ship(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keyState) override;

	void SetRespawnPosition(const Vector2 respawnPosition);
	Vector2 GetRespawnPosition() const;

	void SetRespawnRotation(const float rotation);
	float GetRespawnRotation() const;
private:
	float mElapsed;
	float mLaserCooldown;
	bool mIsDead;
	float mDeadTime;
	Vector2 mRespawnPosition;
	float mRespawnRotation;
	class SpriteComponent* mSprite;
	class CircleComponent* mCircle;
};