#include "shaderClass.hpp"

#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>

// Function prototypes
bool processInput(sf::Window& window);

// Settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

int main()
{
	// SFML OpenGL Settings
	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	settings.attributeFlags = sf::ContextSettings::Core;

	// SFML Window
	sf::Window window;
	window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "Shaders", sf::Style::Default, settings);

	// GLAD Initialization
	if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE); // Exit with failure code
	}

	// Check of many vertex attributes (inputs of the vertex shader) are available to use
	// My hardware allows for 32 inputs
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// Shaders managed by our Shader class
	Shader shaderRed("upsideDown.vert", "rainbowCycle.frag");

	// Define vertex geometry of the triangle
	float vertices[] =
	{
		0.0f,  0.5f, 0.0f,	// Top
		0.5f, -0.5f, 0.0f,	// Bottom Right
	   -0.5f, -0.5f, 0.0f,	// Bottom Left
	};

	// Set up buffers and configure vertex attributes
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// Render loop
	bool running = true;
	sf::Clock clock;
	while (running)
	{
		running = processInput(window);

		// Clear screen with dark grey color
		glClearColor(60.0f/255.0f, 60.0f/255.0f, 60.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate the shader
		shaderRed.use();

		// Update the uniform color
		float timevalue = clock.getElapsedTime().asSeconds();
		//std::cout << timevalue << std::endl;
		shaderRed.setFloat("time", timevalue);

		// Render the triangle
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// End the current frame (internally swaps front and back buffers)
		window.display();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;
}

bool processInput(sf::Window& window)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			return false;

		else if (event.key.code == sf::Keyboard::Escape)
			return false;

		else if (event.type == sf::Event::Resized)
			glViewport(0, 0, event.size.width, event.size.height);
	}
	return true;
}
