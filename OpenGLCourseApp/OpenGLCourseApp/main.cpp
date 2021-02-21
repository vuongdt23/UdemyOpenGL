#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const GLint WIDTH = 800, HEIGHT = 800;
GLuint VAO, VBO, IBO, shader, uniformModel, uniformProjection;
const float toRad = 3.14159265f / 180.0f;
bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncr = 0.0005f;
float curAngle = 0.0f;

bool sizeIncr = true;
float curSize = 0.4;
float maxSize = 0.8;
float minSize = 0.1;
//Vertex Shader
static const char* vShader = "										\n\
#version 330														\n\
																	\n\
layout (location = 0) in vec3 pos;									\n\
uniform mat4 model;													\n\
uniform mat4 projection;											\n\
out vec4 vCol;														\n\
void main()															\n\
{																	\n\
	gl_Position = projection * model* vec4(pos, 1.0);				\n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);						\n\
}"; 

static const char* fShader = "						\n\
#version 330										\n\
in vec4 vCol;										\n\
out vec4 color;										\n\
													\n\
void main()											\n\
{													\n\
													\n\
	color = vCol;									\n\
}";
void createTriangle()
{

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	}
	;
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	 
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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
	uniformProjection = glGetUniformLocation(shader, "projection");

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

	glEnable(GL_DEPTH_TEST);
	//Setup Viewport size

	glViewport(0, 0, bufferWidth, bufferHeight);
	createTriangle();
	CompileShader();


	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);
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
		glClearColor(0,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
		if (sizeIncr) curSize += 0.001;
		else
		{
			curSize -= 0.001;
		}

		if (curSize >= maxSize or curSize< minSize) sizeIncr = !sizeIncr;

		glUseProgram(shader);
		curAngle += 0.01f;
		if (curAngle >= 360) curAngle = 0.0f;
		glm::mat4 model;

		model = glm::translate(model, glm::vec3(0, triOffset, -2.50f));
		model = glm::rotate(model, curAngle * toRad , glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(0.4, 0.4, 1));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniform1f(uniformXMove, triOffset);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}
	return 0;
}