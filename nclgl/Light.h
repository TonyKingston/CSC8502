#pragma once
#include "Vector3.h"
#include "Vector4.h"

class Light
{
public:

	enum LightType { POINT, SPOT, DIRECTIONAL, NONE};

	Light() {} // Default constructor , we ’ll be needing this later !

	Light(const Vector3& position, const Vector4& colour, const Vector3& direction, float radius, float maxAngle) {
		this->colour = colour;
		this->position = position;
		this->direction = direction;
		this->radius = radius;
		this->maxAngle = maxAngle;
	}

	Light(const Vector3& direction, const Vector4& colour) {
		this->colour = colour;
		this->direction = direction;
	}

	Light(const Vector3& position, const Vector4& colour, float radius) {
		this->colour = colour;
		this->position = position;
		this->radius = radius;
	}
	
	~Light(void) {};

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4 & val) { colour = val; }

	Vector3 GetDirection() const { return direction; }
	void SetDirection(const Vector3& val) { direction = val; }

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	float GetAngle() const { return maxAngle; }
	void SetAngle(float ang) { maxAngle = ang; }

	float GetLinearCoefficient() { return linear; }
	void SetLinearCoefficient(float val) { linear = val; }

	float GetQuadraticCoefficient() { return quadratic; }
	void SetQuadraticCoefficient(float val) { quadratic = val; }
	
protected:
	Vector4 colour;
	Vector3 direction;
	Vector3 position;
	float radius;
	float maxAngle;
	float linear;
	float quadratic;
};

