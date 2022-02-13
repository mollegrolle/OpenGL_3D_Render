#pragma once

//Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

//GLEW includes
#include <GL/glew.h>

//GLFW includes
#include <GLFW/glfw3.h>

//GLM includes
#include <GLM/glm.hpp>
#include <GLM/gtx/transform.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <GLM/gtc/matrix_transform.hpp>

//Extra includes
#include <vector>
#include <cstdio>
#include <random>
#include <algorithm>
#include <cmath>

#define DISPLAY_WIDTH 1024
#define DISPLAY_HEIGHT 768

extern GLFWwindow* window;