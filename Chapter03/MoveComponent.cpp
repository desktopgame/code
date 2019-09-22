// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(class Actor* owner, int updateOrder)
:Component(owner, updateOrder)
,mAngularSpeed(0.0f)
,mForwardSpeed(0.0f)
,mForce(0.0f, 0.0f)
,mMass(0.1f)
,mVelocity(0.0f, 0.0f)
{
	
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		float rot = mOwner->GetRotation();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotation(rot);
	}
	
	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 pos = mOwner->GetPosition();
		// compute accel from sum of forces
		Vector2 accel(mForce.x / mMass, mForce.y / mMass);
		// compute velocity from accel
		this->mVelocity = accel * deltaTime;
		// compute position from velocity
		pos += mVelocity * deltaTime;
		this->mForce = Vector2(0.0f, 0.0f);
		/*
		Oridinal Code:
		Vector2 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		*/

		// (Screen wrapping code only for asteroids)
		if (pos.x < 0.0f) { pos.x = 1022.0f; }
		else if (pos.x > 1024.0f) { pos.x = 2.0f; }

		if (pos.y < 0.0f) { pos.y = 766.0f; }
		else if (pos.y > 768.0f) { pos.y = 2.0f; }

		mOwner->SetPosition(pos);
	}
}

void MoveComponent::AddForce(const Vector2 force) {
	this->mForce += force;
}

Vector2 MoveComponent::GetForce() const {
	return mForce;
}

void MoveComponent::SetMass(const float mass) {
	this->mMass = mass;
}

float MoveComponent::GetMass() const {
	return mMass;
}

Vector2 MoveComponent::GetVelocity() const {
	return mVelocity;
}
