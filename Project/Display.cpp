#include "Display.h"

Display::Display(GLint width, GLint height, const std::string& title, bool dyncol)
{
	m_width = width;
	m_height = height;

	r = 0.05f; g = 0.05f; b = 0.2f;
	rBool = true, gBool = true, bBool = true;
	this->dyncol = dyncol;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL);
	m_glContext = SDL_GL_CreateContext(m_window);

	GLenum status = glewInit();

	if (status != GLEW_OK)
	{
		std::cerr << "Glew failed to initialize!" << std::endl;
	}
}

Display::~Display()
{
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Display::Clear()
{
	DynamicColor();
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::SwapBuffers()
{

	SDL_GL_SwapWindow(m_window);

	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			m_isClosed = true;
		}
	}
}

bool Display::IsClosed()
{
	return m_isClosed;
}

float Display::GetWidth()
{
	return m_width;
}

float Display::GetHeight()
{
	return m_height;
}

void Display::DynamicColor()
{
	if (dyncol)
	{
		NumBounce(r, 0.1f, 0.6f, 0.0001f, rBool);
		NumBounce(g, 0.2f, 0.5f, 0.00025f, gBool);
		NumBounce(b, 0.2f, 0.4f, 0.0005f, bBool);
	}
}

void Display::NumBounce(float &number, float min, float max, float interval, bool &colSwitch) {

	if (number < max && colSwitch == true)
	{
		number = number + interval;
		if (number >= max)
		{
			colSwitch = false;
		}
	}


	if (number > min && colSwitch == false)
	{
		number = number - interval;
		if (number <= min)
		{
			colSwitch = true;
		}
	}

}
