#pragma once

#include <iostream>
#include <string>
#include <..\header\GL\glew.h>
#include <..\header\SDL\SDL.h>

class Display
{
public:
	Display(GLint width, GLint height, const std::string& title, bool dyncol);
	~Display();

	void Clear();
	void SwapBuffers();
	bool IsClosed();

	float GetWidth();
	float GetHeight();

	void DynamicColor();
	static void NumBounce(float &number, float min, float max, float interval, bool &colSwitch);

private:
	SDL_Window * m_window;
	SDL_GLContext m_glContext;

	bool m_isClosed;

	GLint m_width;
	GLint m_height;

	float r, g, b, a;
	bool rBool, gBool, bBool;
	bool dyncol;
};