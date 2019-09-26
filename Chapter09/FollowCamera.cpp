// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FollowCamera.h"
#include "Actor.h"
#include "Game.h"

FollowCamera::FollowCamera(Actor* owner)
	:CameraComponent(owner)
	,mHorzDist(350.0f)
	,mVertDist(150.0f)
	,mTargetDist(100.0f)
	,mSpringConstant(64.0f)
	,mOffset(-400.0f, 0.0f, 0.0f)
	,mUp(Vector3::UnitZ)
	,mPitchSpeed(0.0f)
	,mYawSpeed(0.0f)
{
}

void FollowCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);
	// Compute dampening from spring constant
	float dampening = 2.0f * Math::Sqrt(mSpringConstant);
	// Compute ideal position
	Vector3 idealPos = ComputeCameraPos();
	// Compute difference between actual and ideal
	Vector3 diff = mActualPos - idealPos;
	// Compute acceleration of spring
	Vector3 acel = -mSpringConstant * diff -
		dampening * mVelocity;
	// Update velocity
	mVelocity += acel * deltaTime;
	// Update actual camera position
	mActualPos += mVelocity * deltaTime;
	// Target is target dist in front of owning actor
	Vector3 target = mOwner->GetPosition() +
		mOwner->GetForward() * mTargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		mUp);

	int x, y;
	Uint32 buttons = SDL_GetMouseState(&x, &y);
	bool leftIsDown = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
	if (leftIsDown) {
		this->mYawSpeed = 1.0f;
		this->mPitchSpeed = 1.0f;
		// Create a quaternion for yaw about world up
		Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
		// Transform offset and up by yaw
		mOffset = Vector3::Transform(mOffset, yaw);
		mUp = Vector3::Transform(mUp, yaw);

		// Compute camera forward/right from these vectors
		// Forward owner.position - (owner.position + offset)
		// = -offset
		Vector3 forward = -1.0f * mOffset;
		forward.Normalize();
		Vector3 right = Vector3::Cross(mUp, forward);
		right.Normalize();

		// Create quaternion for pitch about camera right
		Quaternion pitch(right, mPitchSpeed * deltaTime);
		// Transform camera offset and up by pitch
		mOffset = Vector3::Transform(mOffset, pitch);
		mUp = Vector3::Transform(mUp, pitch);

		Matrix4 pm = Matrix4::CreateFromQuaternion(pitch);
		Matrix4 ym = Matrix4::CreateFromQuaternion(yaw);
		SetViewMatrix(view * pm * ym);
	} else {
		mOffset = Vector3(-400.0f, 0.0f, 0.0f);
		mUp = (Vector3::UnitZ);
		mPitchSpeed = (0.0f);
		SetViewMatrix(view);
	}
}

void FollowCamera::SnapToIdeal()
{
	// Set actual position to ideal
	mActualPos = ComputeCameraPos();
	// Zero velocity
	mVelocity = Vector3::Zero;
	// Compute target and view
	Vector3 target = mOwner->GetPosition() +
		mOwner->GetForward() * mTargetDist;
	// Use actual position here, not ideal
	Matrix4 view = Matrix4::CreateLookAt(mActualPos, target,
		Vector3::UnitZ);
	SetViewMatrix(view);
}

Vector3 FollowCamera::ComputeCameraPos() const
{
	// Set camera position behind and above owner
	Vector3 cameraPos = mOwner->GetPosition();
	cameraPos -= mOwner->GetForward() * mHorzDist;
	cameraPos += Vector3::UnitZ * mVertDist;
	return cameraPos;
}
