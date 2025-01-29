#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <array>
#include <vector>
#include <unordered_map>


// CLASS GRAPH VERTEX
//=================================================================================================================
class GraphVertex
{
private:
	// ID
	int id;
	static int currentID;
	// Constructor attributes
	float size; // Diameter of rasterized points / pixels. Will be same value as used in gl_PointSize();
	glm::vec3 coords;
	glm::vec3 color;
	// Calculated attributes
	std::array<glm::vec3, 4> bbox; // If point resized with resize(), calculate BoundingBox again

	void generateBbox(); // DEF BELOW

public:
	GraphVertex(const glm::vec3& coords, glm::vec3 color = glm::vec3(0.7843137f), float size = 30.0f)
		: id(currentID++)
		, coords(coords)
		, color(color)
		, size(size) { // Diameter in rasterized points (pixels). This will be used for gl_PointSize(size)
		generateBbox();
	}

	// Getters
	int getID() const { return id; };
	float getSize() const { return size; };
	glm::vec3 getCoords() const { return coords; }
	glm::vec3 getColor() const { return color; }
	std::array<glm::vec3, 4> getBbox() const { return bbox; }

	// Class methods
	bool isMouseWithinBounds(float mouseX, float mouseY) {
		return mouseX >= bbox[0].x && mouseX <= bbox[1].x && mouseY >= bbox[2].y && mouseY <= bbox[0].y;
	}

	void resize(float newSize) const { /* TODO */ }
};
//----------------------------------------------------------------------------------------------------------------

int GraphVertex::currentID = 1;

void GraphVertex::generateBbox() {
	float radius = size / 2.0f;
	bbox[0] = glm::vec3(coords.x - radius, coords.y + radius, coords.z); // Top Left
	bbox[1] = glm::vec3(coords.x + radius, coords.y + radius, coords.z); // Top Right
	bbox[2] = glm::vec3(coords.x + radius, coords.y - radius, coords.z); // Bottom Right
	bbox[3] = glm::vec3(coords.x - radius, coords.y - radius, coords.z); // Bottom Left
}
//=================================================================================================================


// CLASS GRAPH
//=================================================================================================================
class Graph
{
private:
	static int currentID;
	int id;
	std::unordered_map<int, GraphVertex> vertices;

public:
	Graph() : id(currentID++) {}

	// Getters
	int getID() const { return id; }
	const std::unordered_map<int, GraphVertex>& getVertices() const { return vertices; }
	const std::vector<std::array<float, 7>> getVerticesForVBO(); // DEF BELOW. First 3 floats: coords. Next 3 floats: color. Next float: size.

	// Class methods
	void addVertex(GraphVertex& vertex) { vertices.insert({ vertex.getID(), vertex }); }
	bool deleteVertexbyID(int vertexID) { return vertices.erase(vertexID); }
	GraphVertex* findClickedPoint(float mouseX, float mouseY); // DEF BELOW
	void clear() { vertices.clear(); }
};
//----------------------------------------------------------------------------------------------------------------

int Graph::currentID = 1;

GraphVertex* Graph::findClickedPoint(float mouseX, float mouseY)
{
	GraphVertex* lastMatchVertex = nullptr;

	// Iterate over all vertices
	for (std::pair<const int, GraphVertex>& pair : vertices)
	{
		GraphVertex& vertex = pair.second;
		if (vertex.isMouseWithinBounds(mouseX, mouseY))
		{
			lastMatchVertex = &vertex;
		}
	}
	return lastMatchVertex;
}

const std::vector<std::array<float, 7>> Graph::getVerticesForVBO()
{
	std::vector<std::array<float, 7>> verticesVBO;
	for (std::pair<const int, GraphVertex>& pair : vertices)
	{
		GraphVertex& vertex = pair.second;
		glm::vec3 vCoords = vertex.getCoords();
		glm::vec3 vColor = vertex.getColor();
		std::array<float, 7> vertexVBO = { vCoords.x, vCoords.y, vCoords.z, vColor.x, vColor.y, vColor.z, vertex.getSize() };
		verticesVBO.push_back(vertexVBO);
	}

	return verticesVBO;
}
//=================================================================================================================
