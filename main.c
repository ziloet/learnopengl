#include<windows.h>
#include<stdint.h>

#include"main.h"
#include"khrplatform.h"
#include"opengl.h"

#include"opengl.c"

#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "opengl32")

static LRESULT CALLBACK
WindowProc(HWND Window, UINT Message, WPARAM wParam, LPARAM lParam)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_SIZE:
		{
			break_if(glViewport == 0);
			RECT WindowRect = {0};
			GetClientRect(Window, &WindowRect);
			int WindowWidth = WindowRect.right - WindowRect.left;
			int WindowHeight = WindowRect.bottom - WindowRect.top;
			glViewport(0, 0, WindowWidth, WindowHeight);
		} break;

		case WM_CLOSE:
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;

		default:
		{
			Result = DefWindowProcW(Window, Message, wParam, lParam);
		} break;
	}
	return Result;
}

static HWND
CreateOpenGLWindow(void)
{
	HINSTANCE Instance = GetModuleHandleW(0);
	WNDCLASSEXW WindowClass = {0};
	WindowClass.cbSize = sizeof(WindowClass);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	WindowClass.hInstance = Instance;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.lpszClassName = L"wndclass_name";
	RegisterClassExW(&WindowClass);
	DWORD WindowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	HWND Result = CreateWindowExW(0, WindowClass.lpszClassName, L"Window", WindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, Instance, 0);
	Assert(IsWindow(Result));
	return Result;
}

char* VertexShaderSource =
{
	"#version 330 core\n"
	"layout(location = 0) in vec3 Position;\n"
	"void main()\n"
	"{\n"
	"\tgl_Position = vec4(Position.x, Position.y, Position.z, 1.0f);\n"
	"}\n"
};

char* FragmentShaderSource =
{
	"#version 330 core\n"
	"out vec4 FragmentColor;\n"
	"void main()\n"
	"{\n"
	"\tFragmentColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n"
};

char* FragmentShaderSource2 =
{
	"#version 330 core\n"
	"out vec4 FragmentColor;\n"
	"void main()\n"
	"{\n"
	"\tFragmentColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
	"}\n"	
};

static void
DisableDPIScaling(void)
{
	HMODULE Library = LoadLibraryW(L"user32.dll");
	void* Function = GetProcAddress(Library, "SetProcessDpiAwarenessContext");
	if(Function)
	{
		typedef BOOL new_function(DPI_AWARENESS_CONTEXT);
		new_function* SetProcessDpiAwarenessContext = Function;
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
		return;
	}
	Function = GetProcAddress(Library, "SetProcessDpiAware");
	if(Function)
	{
		typedef BOOL old_function(void);
		old_function* SetProcessDpiAware = Function;
		SetProcessDpiAware();
	}
}

float Triangle1[] =
{
	-0.5f, -0.5f, 0.0f,
	-0.25f, 0.5f, 0.0f,
	0.0f, -0.5f, 0.0f,
};

float Triangle2[] =
{
	0.0f, -0.5f, 0.0f,
	0.25f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
};

void WinMainCRTStartup()
{
	DisableDPIScaling();
	HWND Window = CreateOpenGLWindow();
	HDC WindowDC = GetDC(Window);
	SetOpenGLContext(Window);
	LoadOpenGLFunctions();

	GLuint VertexArrays[2];
	GLuint VertexBuffers[2];
	glGenVertexArrays(2, VertexArrays);
	glGenBuffers(2, VertexBuffers);

	glBindVertexArray(VertexArrays[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle1), Triangle1, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(VertexArrays[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle1), Triangle2, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, 0);
	glCompileShader(VertexShader);
	GLint Status = 0;
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Status);
	char ErrorLog[512];
	if(Status == GL_FALSE)
	{
		glGetShaderInfoLog(VertexShader, sizeof(ErrorLog), 0, ErrorLog);
	}

	GLuint FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderSource, 0);
	glCompileShader(FragmentShader);
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Status);
	if (Status == GL_FALSE)
	{
		glGetShaderInfoLog(FragmentShader, sizeof(ErrorLog), 0, ErrorLog);
	}

	GLuint FragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader2, 1, &FragmentShaderSource2, 0);
	glCompileShader(FragmentShader2);
	glGetShaderiv(FragmentShader2, GL_COMPILE_STATUS, &Status);
	if(Status == GL_FALSE)
	{
		glGetShaderInfoLog(FragmentShader2, sizeof(ErrorLog), 0, ErrorLog);
	}

	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Status);
	if(Status == GL_FALSE)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), 0, ErrorLog);
	}
	glDeleteShader(FragmentShader);

	GLuint ShaderProgram2 = glCreateProgram();
	glAttachShader(ShaderProgram2, VertexShader);
	glAttachShader(ShaderProgram2, FragmentShader2);
	glLinkProgram(ShaderProgram2);
	glGetProgramiv(ShaderProgram2, GL_LINK_STATUS, &Status);
	if(Status == GL_FALSE)
	{
		glGetProgramInfoLog(ShaderProgram2, sizeof(ErrorLog), 0, ErrorLog);
	}
	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader2);
	
	MSG Message = {0};
	for(;;)
	{
		GetMessageW(&Message, 0, 0, 0);
		do
		{
			if(Message.message == WM_QUIT)
			{
				ExitProcess(0);
			}
			DispatchMessageW(&Message);
		} while(PeekMessageW(&Message, 0, 0, 0, PM_REMOVE));
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(ShaderProgram);
		glBindVertexArray(VertexArrays[0]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glUseProgram(ShaderProgram2);
		glBindVertexArray(VertexArrays[1]);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		SwapBuffers(WindowDC);
	}
}