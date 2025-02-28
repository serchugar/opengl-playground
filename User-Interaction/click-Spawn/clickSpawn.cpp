#include "../../Shaders/shaderClass.hpp"

#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>

// TODO: Keep aspect ratio to 1:1 no matter the size of the window -> Will be done in the future in other file
namespace
{
	// Useful structs
	struct Vec2f { float x, y; };
	struct Vec3f { float x, y, z; };

	// Function prototypes
	bool closeCondition(sf::Event& event);
	void createNGon(float x, float y, float z, int n_vertices, float radius, std::vector<float>& vertices, std::vector<int>& indices);
	Vec2f convertMousePosToNDC(sf::Event& event, sf::Window& window);
	void shapeSelector(sf::Event& event, int& num_vertices, float& polygon_size);

	const float PI = 3.14159265359f;

	// Settings
	const unsigned int SCR_WIDTH = 900;
	const unsigned int SCR_HEIGHT = 900;
}


void clickSpawn()
{
	// Show controls at the start of the program
	std::cout << "ClickSpawnv3 Controls:" << std::endl;
	std::cout << "-----------------------------------------------------------------" << std::endl;
	std::cout << "Press left click to draw a polygon" << std::endl;
	std::cout << "Press R to reset the drawing" << std::endl;
	std::cout << "Press W to toggle wireframe mode" << std::endl;
	std::cout << "Press 1 to decrease the polygon size" << std::endl;
	std::cout << "Press 2 to increase the polygon size" << std::endl;
	std::cout << "Press 3-0 to control the number of vertices of the polygons" << std::endl;
	std::cout << "-----------------------------------------------------------------\n" << std::endl;

	// SFML OpenGL Settings
	sf::ContextSettings settings;
	settings.majorVersion = 4;
	settings.minorVersion = 6;
	settings.attributeFlags = sf::ContextSettings::Core;

	// SFML Window
	sf::Window window;
	window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "ClickSpawnv3", sf::Style::Default, settings);

	// GLAD Initialization
	if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE); // Exit with failure code
	}

	// Check of many vertex attributes (inputs of the vertex shader) are available to use
	int nrAttributes; // My hardware allows for 32 inputs
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	//std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// Shaders managed by our Shader class
	Shader shader1("click-Spawn/basicVert.vert", "../Shaders/rgb-Cycle/rainbowCycle.frag");

	// Define vertex geometry of the pentagon
	std::vector<float> vertices;
	std::vector<int> indices;

	// Set up buffers and configure vertex attributes
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO); // Create the vertex buffer object VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Bind the VBO to the GL_ARRAY_BUFFER. From now on, any buffer calls to GL_ARRAY_BUFFER will affect the bound buffer VBO

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	// Position attribute. How should OpenGL interpret the vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // Enable the vertex attribute with location=0

	// Wireframe mode
	bool wireframeMode = false;
	wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Render loop
	bool running = true;
	Vec2f NDC;
	sf::Clock clock;
	int num_vertices = 3;
	float polygon_size = 0.05f;
	while (running)
	{
		// Clear screen with dark grey color
		glClearColor(60.0f/255.0f, 60.0f/255.0f, 60.0f/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Activate the shader
		shader1.use();

		// Update the uniform color
		float timevalue = clock.getElapsedTime().asSeconds();
		shader1.setFloat("time", timevalue);

		// Process all events in a single loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Condition needed so that no other event resets running to true
			if (running)
				running = closeCondition(event);

			NDC = convertMousePosToNDC(event, window);

			if (event.type == sf::Event::MouseButtonPressed && 
				event.mouseButton.button == sf::Mouse::Left)
				createNGon(NDC.x, NDC.y, 0.0f, num_vertices, polygon_size, vertices, indices);

			// Change number of vertices in runtime so user can decide the shape of the polygon
			if (event.type == sf::Event::KeyPressed)
				shapeSelector(event, num_vertices, polygon_size);

			// Press R to reset the drawing
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
			{
				vertices.clear();
				indices.clear();
			}

			// Press W to toggle wireframe mode
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
			{
				wireframeMode = !wireframeMode;
				wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				//wireframeMode ? std::cout << "Wireframe mode ON" << std::endl : std::cout << "Wireframe mode OFF" << std::endl;
			}
		}

		// Draw using the vertices and indices updated by createNGon();
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

		// Render the triangle
		glBindVertexArray(VAO);
		glPointSize(20.0f);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		// End the current frame (internally swaps front and back buffers)
		window.display();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shader1.ID);
}

namespace
{
	bool closeCondition(sf::Event& event)
	{
		if (event.type == sf::Event::Closed)
			return false;

		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			return false;

		else if (event.type == sf::Event::Resized)
		{
			glViewport(0, 0, event.size.width, event.size.height);
			
		}

		return true;
	}

	Vec2f convertMousePosToNDC(sf::Event& event, sf::Window& window)
	{
		sf::Vector2u windowSize = window.getSize();

		// Convert the mouse position to NDC
		float ndcx = 2.0f * event.mouseButton.x / windowSize.x - 1.0f;
		float ndcy = 1.0f - 2.0f * event.mouseButton.y / windowSize.y;

		return { ndcx, ndcy };
	}

	void createNGon(float x, float y, float z, int n_vertices, float radius, std::vector<float>& vertices, std::vector<int>& indices)
	{
		Vec3f center = { x, y, z };

		n_vertices = n_vertices < 3 ? 3 : n_vertices;

		// Update vertices
		float angleStep = 2.0f * PI / n_vertices;
		float startAngle = PI / 2.0f;

		if (n_vertices % 2 == 0) // This is so any even vertex polygon has the flat side on top
			startAngle += 180.f / n_vertices * PI / 180.0f;

		for (int i = 0; i < n_vertices; i++)
		{
			float angle = startAngle + i * angleStep;
			float x = center.x + radius * cos(angle);
			float y = center.y + radius * sin(angle);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}

		// Update indices
		int last_index = indices.size() == 0 ? 0 : indices.back() + 1;
		for (int i = last_index; i < last_index + n_vertices - 2; i++)
		{
			indices.push_back(last_index);
			indices.push_back(i + 1);
			indices.push_back(i + 2 );
		}
	}

	void shapeSelector(sf::Event& event, int& num_vertices, float& polygon_size)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Num1:
			polygon_size -= 0.01f;
			polygon_size = polygon_size < 0.01f ? 0.01f : polygon_size;
			std::cout << "Polygon Size: " << polygon_size << std::endl;
			break;
		case sf::Keyboard::Num2:
			polygon_size += 0.01f;
			std::cout << "Polygon Size: " << polygon_size << std::endl;
			break;
		case sf::Keyboard::Num3:
			num_vertices = 3;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num4:
			num_vertices = 4;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num5:
			num_vertices = 5;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num6:
			num_vertices = 6;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num7:
			num_vertices = 7;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num8:
			num_vertices = 8;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num9:
			num_vertices = 9;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		case sf::Keyboard::Num0:
			num_vertices = 20;
			std::cout << "Number of vertices: " << num_vertices << std::endl;
			break;
		}
	}
}
