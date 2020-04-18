// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.9f, 0.9f, 0.5f, 0.0f);

	// Включить тест глубины
	glEnable(GL_DEPTH_TEST);
	// Фрагмент будет выводиться только в том, случае, если он находится ближе к камере, чем предыдущий
	glDepthFunc(GL_LESS);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	GLuint programIDB = LoadShaders("SimpleTransform.vertexshader", "SimpleFragmentShaderB.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	// Get a handle for our "MVP" uniform
	GLuint MatrixIDB = glGetUniformLocation(programIDB, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f,  0.0f,  0.5f,
		 0.5f,  0.0f,  0.5f,
		 0.0f,  1.0f,  0.0f,

		-0.5f,  0.0f, -0.5f,
		 0.5f,  0.0f, -0.5f,
		 0.0f,  1.0f,  0.0f,

		-0.5f,  0.0f, -0.5f,
		-0.5f,  0.0f,  0.5f,
		 0.0f,  1.0f,  0.0f,

		 0.5f,  0.0f, -0.5f,
		 0.5f,  0.0f,  0.5f,
		 0.0f,  1.0f,  0.0f,

		 0.5f,  0.0f, -0.5f,
		 0.5f,  0.0f,  0.5f,
		-0.5f,  0.0f, -0.5f,

		 0.5f,  0.0f,  0.5f,
	    -0.5f,  0.0f, -0.5f,
		-0.5f,  0.0f,  0.5f,

		-0.2f, -0.2f,  1.0f,
		 0.2f, -0.2f,  1.0f,
		 0.25f, 0.4f, -0.8f
	};

	static const GLfloat g_color_buffer_data[] = {
		1.0f,  1.0f,  1.0f, 0.1f,
		1.0f,  1.0f,  1.0f, 0.1f,
		1.0f,  1.0f,  1.0f, 0.1f,

		0.822f,  0.569f,  0.201f, 0.7f,
		0.435f,  0.602f,  0.223f, 0.7f,
		0.310f,  0.747f,  0.185f, 0.7f,

		0.597f,  0.770f,  0.761f, 0.7f,
		0.559f,  0.436f,  0.730f, 0.7f,
		0.359f,  0.583f,  0.152f, 0.7f,

		0.483f,  0.596f,  0.789f, 0.7f,
		0.559f,  0.861f,  0.639f, 0.7f,
		0.195f,  0.548f,  0.859f, 0.7f,

		0.014f,  0.184f,  0.576f, 0.7f,
		0.771f,  0.328f,  0.970f, 0.7f,
		0.406f,  0.615f,  0.116f, 0.7f,

		0.676f,  0.977f,  0.133f, 0.7f,
		0.971f,  0.572f,  0.833f, 0.7f,
		0.140f,  0.616f,  0.489f, 0.7f,

		0.997f,  0.513f,  0.064f, 0.7f,
		0.945f,  0.719f,  0.592f, 0.7f,
		0.543f,  0.021f,  0.978f, 0.7f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat radius = 5.0f;
		GLfloat camX = sin(glfwGetTime()) * radius;
		GLfloat camY = sin(glfwGetTime()) * radius;
		GLfloat camZ = cos(glfwGetTime()) * radius;
		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(camX, camY, camZ), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 18); // 3 indices starting at 0 -> 1 triangle

		// Use our shader
		glUseProgram(programIDB);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixIDB, 1, GL_FALSE, &MVP[0][0]);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 18, 3); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);
	glDeleteProgram(programIDB);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

