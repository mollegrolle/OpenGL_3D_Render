#pragma once

#include <GLFW\glfw3.h>


float bounceSineWave(float minValue, float maxValue, float pace);
float bounceTriangleWave(float minValue, float maxValue, float pace);
float bounceSawWave(float minValue, float maxValue, float pace);
float linearGrowth(float startingNumber, float rate);
float exponetialGrowth(float startingNumber, float rate);
float cubicGrowth(float startingNumber, float rate);