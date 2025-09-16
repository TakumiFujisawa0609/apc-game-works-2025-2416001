#include "Vector2F.h"
#include "../Utility/AsoUtility.h"
#include "Vector2.h"

// デフォルトコンストラクタ
Vector2::Vector2()
{
	x = 0;
	y = 0;
}

// コンストラクタ
Vector2::Vector2(int vX, int vY)
{
	x = vX;
	y = vY;
}

Vector2::Vector2(Vector2F value)
{
	x = AsoUtility::Round(value.x);
	y = AsoUtility::Round(value.y);
}

Vector2F Vector2::ToVector2F(void)
{
	Vector2F ret;
	ret.x = static_cast<float>(x);
	ret.y = static_cast<float>(y);
	return ret;
}

void Vector2::operator*=(const int _value)
{
	x *= _value;
	y *= _value;
}

const Vector2 Vector2::operator*(const int _value)const
{
	return { x * _value, y * _value };
}
