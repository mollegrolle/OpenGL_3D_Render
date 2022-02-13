#include "Math.h"
#include "SimpleMath.h"

float bounceSineWave(float minValue, float maxValue, float pace)
{
	return (float)((sin(glfwGetTime() * pace) * 0.5 + 0.5) * (maxValue - minValue) + minValue);
}

float bounceTriangleWave(float minValue, float maxValue, float pace)
{
	return (float)(fabs(remainder(glfwGetTime() * pace, (maxValue - minValue) * 2)) + minValue);
}

float bounceSawWave(float minValue, float maxValue, float pace)
{
	return (float)(fmodf(((glfwGetTime() * pace) * 0.5 + 0.5), 1) * (maxValue - minValue) + minValue);
}

float linearGrowth(float startingNumber, float rate)
{
	return (float)(glfwGetTime() * rate + startingNumber);
}

float exponetialGrowth(float startingNumber, float rate)
{
	return (float)(pow(2, (glfwGetTime() * rate)) + startingNumber);
}

float cubicGrowth(float startingNumber, float rate)
{
	return (float)(pow((glfwGetTime() * rate), 3) + startingNumber);
}
