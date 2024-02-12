#pragma once

#include "Vec2.h"
#include <SFML\Graphics.hpp>

class CTransform
{
public:
	Vec2 pos = { 0.0 , 0.0 };
	Vec2 velocity = { 0.0 , 0.0 };
	float angle;

	CTransform(const Vec2& p, const Vec2& b, float u): pos(p), velocity(b), angle(u) {}
};

class CShape
{
public:
	sf::CircleShape circle;
	CShape(float r,int points, const sf::Color & fill, const sf::Color & outline, float thickness): circle(r,points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(r, r);
	}
};

class CCollision
{
public:
	float radius = 0;
	CCollision(float r):radius(r){}
};

class CScore
{
public:
	int score;
	CScore(int s) : score(s){}
};

class CLifespan
{
public:
	int remaining = 0;
	int total = 0;
	CLifespan(int a): remaining(a), total(a){}
};

class CInput
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	CInput(){}
};


