#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const GLint WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, shader, uniformModel;
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncr = 0.00005f;
//Vertex Shader
static const char* vShader = "										\n\
#version 330														\n\
																	\n\
layout (location = 0) in vec3 pos;									\n\
uniform mat4 model	;												\n\
																	\n\
void main()															\n\
{																	\n\
	gl_Position = model* vec4(0.4 * pos.x , 0.4 * pos.y, 0.4 * pos.z, 1.0);	\n\
}";

static const char* fShader = "						\n\
#version 330										\n\
													\n\
out vec4 color;										\n\
													\n\
void main()											\n\
{													\n\
	color = vec4(1.0, 0.0, 0.0, 1.0);				\n\
}";
void createTriangle()
{
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, 0.5f, 0.5f
	};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);
	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;

	GLchar elog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(elog), NULL, elog);
		printf("Error compiling the %d shader '%s' \n", shaderType, elog);
		return;
	}
	glAttachShader(theProgram, theShader);
}
void CompileShader()
{
	shader = glCreateProgram();
	if (!shader) {
		printf("Error creating program");
		return;
	}
	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);
	GLint result = 0;

	GLchar elog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
		printf("Error linking program '%s' \n", elog);
		return;
	}
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
		printf("Error validating program '%s' \n", elog);
		return;
	}
	uniformModel = glGetUniformLocation(shader, "model");
}
int main()
{
	if (!glfwInit())
	{
		printf("GLFW Init failed");
		glfwTerminate();
		return 1;
	}
	//Setup GLFW window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW Window Creation failed");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size Info
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	glfwMakeContextCurrent(mainWindow);

	//Allow Modern Extention Version

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW init failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Setup Viewport size

	glViewport(0, 0, bufferWidth, bufferHeight);
	createTriangle();
	CompileShader();

	//loop

	while (!glfwWindowShouldClose(mainWindow))
	{
		//Get + Handle user input
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncr;
		}
		else {
			triOffset -= triIncr;
		}

		if (abs(triOffset) >= triMaxOffset) direction = !direction;
		//Clear Window
		glClearColor(1.0f, 0.3f, 0.7f, 0.9f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);
	
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(triOffset, 0, 0));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}
	return 0;
}