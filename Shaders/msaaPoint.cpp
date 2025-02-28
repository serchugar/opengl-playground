#include "shaderClass.hpp"

#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>
#include <glm/vec3.hpp>

namespace
{
	const unsigned int SCR_WIDTH = 900;
	const unsigned int SCR_HEIGHT = 900;

	// Function prototypes
	bool closeCondition(sf::Event& event);
	void changePointSize(sf::Event& event, float& pointSize, glm::vec3& pointColor);
}

// To achiece a MSAA-like effect using GL_POINTS primitives
void msaaPoint()
{
	// SFML OpenGL Settings
	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	//settings.antialiasingLevel = 4; // This has no effect when using GL_POINTS primitives. Only for polygon primitives
	settings.attributeFlags = sf::ContextSettings::Core;

	// SFML Window
	sf::Window window;
	window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "MSAA using GL_POINTS primitives", sf::Style::Default, settings);

	// GLAD Initialization
	if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE); // Exit with failure code
	}

	// Shaders managed by the Shader class
	Shader shader("msaaPoint.vert", "msaaPoint.frag");
	Shader shader2("msaaPoint.vert", "msaaPoint.frag");

	// Define vertex variables
	float vertices[]  = { -0.5,0,0 };
	float vertices2[] = { +0.5,0,0 };

	// Set up buffers and configure vertex attributes
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Vertex attributes. How should OpenGL interpret the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // Enable the vertex attribute with location=0

	// Enable MSAA. OpenGL does this automatically with the amount of subdivisions specified by the antialiasinglevel
	//glEnable(GL_MULTISAMPLE); // This has no effect when using GL_POINTS primitives

	// Enable point size editing in shader program and enable blending with alpha.
	// Important to achieve MSAA-like effect done in the fragment shader
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Variables for render loop
	sf::Vector2u windowSize = window.getSize();

	// Hiperparameters for render loop
	glm::vec3 backgroundColor(255.0f/255.0f);

	// Render loop
	bool running = true;
	while (running)
	{
		// Clear the screen with background color
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate the shader
		shader.setVec2("uWindowSize", windowSize.x, windowSize.y); // For point with MSAA
		shader2.setVec2("uWindowSize", windowSize.x, windowSize.y);// For point without MSAA
		
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (running)
				running = closeCondition(event);

			if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
				windowSize = window.getSize();
				shader.setVec2("uWindowSize", windowSize.x, windowSize.y);
				shader2.setVec2("uWindowSize", windowSize.x, windowSize.y);
			}
		}

		glBindVertexArray(VAO);

		// Draw circle 1, with MSAA
		shader.use();
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, 1);

		// Draw circle 2, without MSAA
		shader2.use();
		shader2.setBool("uMSAA", false);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, 1);

		// Swap buffers
		window.display();
	}

	// De-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader.ID);
}

namespace
{
	bool closeCondition(sf::Event& event)
	{
		if (event.type == sf::Event::Closed)
			return false;

		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			return false;
		return true;
	}
}
