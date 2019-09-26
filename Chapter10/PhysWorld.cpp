// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PhysWorld.h"
#include <algorithm>
#include "BoxComponent.h"
#include <SDL/SDL.h>
#include <unordered_map>
#include <unordered_set>

PhysWorld::PhysWorld(Game* game)
	:mGame(game)
{
}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
	bool collided = false;
	// Initialize closestT to infinity, so first
	// intersection will always update closestT
	float closestT = Math::Infinity;
	Vector3 norm;
	// Test against all boxes
	for (auto box : mBoxes)
	{
		float t;
		// Does the segment intersect with the box?
		if (Intersect(l, box->GetWorldBox(), t, norm))
		{
			// Is this closer than previous intersection?
			if (t < closestT)
			{
				outColl.mPoint = l.PointOnSegment(t);
				outColl.mNormal = norm;
				outColl.mBox = box;
				outColl.mActor = box->GetOwner();
				collided = true;
			}
		}
	}
	return collided;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> f)
{
	// Naive implementation O(n^2)
	for (size_t i = 0; i < mBoxes.size(); i++)
	{
		// Don't need to test vs itself and any previous i values
		for (size_t j = i + 1; j < mBoxes.size(); j++)
		{
			BoxComponent* a = mBoxes[i];
			BoxComponent* b = mBoxes[j];
			if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				// Call supplied function to handle intersection
				f(a->GetOwner(), b->GetOwner());
			}
		}
	}
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> f)
{

	std::unordered_map<BoxComponent*, std::vector<BoxComponent*> > xMap;
	std::unordered_map<BoxComponent*, std::vector<BoxComponent*> > yMap;
	std::unordered_map<BoxComponent*, std::vector<BoxComponent*> > zMap;

	for (size_t i = 0; i < mXBoxes.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = mXBoxes[i];
		float max = a->GetWorldBox().mMax.x;
		std::vector<BoxComponent*> dest;
		for (size_t j = i + 1; j < mXBoxes.size(); j++)
		{
			BoxComponent* b = mXBoxes[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().mMin.x > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				dest.emplace_back(b);
			}
		}
		xMap.insert_or_assign(a, dest);
	}
	for (size_t i = 0; i < mYBoxes.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = mYBoxes[i];
		float max = a->GetWorldBox().mMax.y;
		std::vector<BoxComponent*> dest;
		for (size_t j = i + 1; j < mYBoxes.size(); j++)
		{
			BoxComponent* b = mYBoxes[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().mMin.y > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				dest.emplace_back(b);
			}
		}
		yMap.insert_or_assign(a, dest);
	}
	for (size_t i = 0; i < mZBoxes.size(); i++)
	{
		// Get max.x for current box
		BoxComponent* a = mZBoxes[i];
		float max = a->GetWorldBox().mMax.z;
		std::vector<BoxComponent*> dest;
		for (size_t j = i + 1; j < mZBoxes.size(); j++)
		{
			BoxComponent* b = mZBoxes[j];
			// If AABB[j] min is past the max bounds of AABB[i],
			// then there aren't any other possible intersections
			// against AABB[i]
			if (b->GetWorldBox().mMin.z > max)
			{
				break;
			}
			else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
			{
				dest.emplace_back(b);
			}
		}
		zMap.insert_or_assign(a, dest);
	}
	std::unordered_set<BoxComponent*> uniqueBoxes;
	for (auto box : mBoxes) {
		uniqueBoxes.clear();
		const std::vector<BoxComponent*>& xv = xMap.at(box);
		const std::vector<BoxComponent*>& yv = yMap.at(box);
		const std::vector<BoxComponent*>& zv = zMap.at(box);
		if (xv.empty() || yv.empty() || zv.empty()) {
			continue;
		}
		for (auto v : xv) uniqueBoxes.emplace(v);
		for (auto v : yv) uniqueBoxes.emplace(v);
		for (auto v : zv) uniqueBoxes.emplace(v);
		for (auto uniqueV : uniqueBoxes) {
			f(box->GetOwner(), uniqueV->GetOwner());
		}
	}
}

void PhysWorld::AddBox(BoxComponent* box)
{
	mBoxes.emplace_back(box);
	mXBoxes.emplace_back(box);
	mYBoxes.emplace_back(box);
	mZBoxes.emplace_back(box);
	std::sort(mXBoxes.begin(), mXBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
		return a->GetWorldBox().mMin.x <
			b->GetWorldBox().mMin.x;
	});
	std::sort(mYBoxes.begin(), mYBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
		return a->GetWorldBox().mMin.y <
			b->GetWorldBox().mMin.y;
	});
	std::sort(mZBoxes.begin(), mZBoxes.end(),
		[](BoxComponent* a, BoxComponent* b) {
		return a->GetWorldBox().mMin.z <
			b->GetWorldBox().mMin.z;
	});
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
	RemoveBox(mBoxes, box);
	RemoveBox(mXBoxes, box);
	RemoveBox(mYBoxes, box);
	RemoveBox(mZBoxes, box);
}

void PhysWorld::RemoveBox(std::vector<BoxComponent*> src, BoxComponent * box)
{
	auto iter = std::find(src.begin(), src.end(), box);
	if (iter != src.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		std::iter_swap(iter, src.end() - 1);
		src.pop_back();
	}
}