#include "../../Shaders/shaderClass.hpp"
#include "Graph.hpp"

#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>

namespace
{
	const unsigned int SCR_WIDTH = 900;
	const unsigned int SCR_HEIGHT = 900;

	// Function prototypes
	bool closeCondition(sf::Event& event);
	void changePointSize(sf::Event& event, float& pointSize, glm::vec3& pointColor);
}

void clickDelete()
{
	// SFML OpenGL Settings
	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	settings.attributeFlags = sf::ContextSettings::Core;

	// SFML Window
	sf::Window window;
	window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "ClickDelete", sf::Style::Default, settings);

	// GLAD Initialization
	if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE); // Exit with failure code
	}

	// Shaders managed by the Shader class
	Shader shader("click-Delete/pointAsCircle.vert", "click-Delete/pointAsCircle.frag");
	// To create a shader that draws points as circles:
	//https://mmmovania.blogspot.com/2010/12/circular-point-sprites-in-opengl-33.html

	// Initialize Graph
	Graph graph;

	// Define vertex variables
	std::vector<std::array<float, 7>> vertices;

	// Set up buffers and configure vertex attributes
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// Vertex attributes. How should OpenGL interpret the vertex data
	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // Enable the vertex attribute with location=0

	// Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Size
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Variables for render loop
	glm::vec3 pointColor(0.78);
	float pointSize = 60.0f;
	sf::Vector2u windowSize = window.getSize();

	// Hiperparameters for render loop
	glEnable(GL_PROGRAM_POINT_SIZE); // So that gl_PointSize can be changed within the vert shader
	glm::vec3 backgroundColor(30.0f/255.0f);

	// Render loop
	bool running = true;
	while (running)
	{

		// Clear the screen with dark grey color
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate the shader
		shader.use();
		shader.setVec2("uWindowSize", windowSize.x, windowSize.y);
		shader.setVec3("backgroundColor", backgroundColor.x, backgroundColor.y, backgroundColor.z);
		
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
			}

			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Left)
			{

				// Update the vertices variable here
				GraphVertex newVertex({event.mouseButton.x, event.mouseButton.y, 0.0f}, pointColor, pointSize);
				graph.addVertex(newVertex);
				vertices = graph.getVerticesForVBO();
			}

			if (event.type == sf::Event::MouseButtonPressed &&
				event.mouseButton.button == sf::Mouse::Right)
			{

				GraphVertex* vertexToDelete = graph.findClickedPoint(event.mouseButton.x, event.mouseButton.y);

				if (vertexToDelete != nullptr)
				{
					graph.deleteVertexbyID(vertexToDelete->getID());
					vertices = graph.getVerticesForVBO();
				}
			}

			if (event.type == sf::Event::KeyPressed)
			{
				changePointSize(event, pointSize, pointColor);

				if (event.key.code == sf::Keyboard::R)
				{
					vertices.clear();
					graph.clear();
				}
			}
		}

		// Pass the vertices geometry to the array buffer linked to the VBO
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

		// Draw the points
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, vertices.size());

		// Swap buffers
		window.display();
	}

	// De-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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

	void changePointSize(sf::Event& event, float& pointSize, glm::vec3& pointColor)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Num1:
			pointSize = 30.0f;
			pointColor = glm::vec3(255/255.0f, 75/255.0f, 75/255.0f);
			break;
		case sf::Keyboard::Num2:
			pointSize = 60.0f;
			pointColor = glm::vec3(0.78);
			break;
		case sf::Keyboard::Num3:
			pointSize = 100.0f;
			pointColor = glm::vec3(165/255.0f, 255/255.0f, 74/255.0f);
			break;
		}
	}
}
