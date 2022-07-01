/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	ModelManager.cpp
 *
 */

#include "ModelManager.h"

ModelManager::ModelManager() {}

ModelManager::~ModelManager()
{
	for (auto it = models.begin(); it != models.end(); it++)
	{
		delete *it;
	}
	models.clear();
}

bool ModelManager::loadModel(fs::path modelFile, std::string name, ShapeType modelType, float radius)
{
	bool result = true;

	std::ifstream file; 
	
	file.open(modelFile);

	if (file.is_open())
	{
		Model *model;

		try
		{
			model = new Model(name);

			switch (modelType)
			{
			case sphere:
				model->shape = new Sphere(radius);
				break;
			case hull:
				model->shape = new Hull();
				break;
			case plane:
				model->shape = new PlaneShape();
				break;
			}
		}
		catch (...)
		{
			printAllocErrorMsg("for geometry model.");
			return false;
		}

		// line read from file
		std::string line;

		unsigned int index = 0;

		// Associative array for Vertex and its index
		std::map <Vertex, unsigned int> verticesMap;
		std::pair<std::map <Vertex, unsigned int>::iterator, bool> p;

		std::cout << "Loading " << modelFile << std::endl;

		while (file)
		{
			std::getline(file, line);
			std::string lineHeader = line.substr(0, 2);
			std::stringstream stream;

			if (lineHeader == "v ")
			{
				// line represents vertex position
				stream << line.substr(2);

				glm::vec3 vertexPosition;
				stream >> vertexPosition.x; stream >> vertexPosition.y; stream >> vertexPosition.z;
				model->vertices.push_back(vertexPosition);

				if (modelType == hull || modelType == plane)
				{
					Hull* hull = dynamic_cast<Hull*>(model->shape);
					if (!hull->addVertex(vertexPosition))
					{
						printAllocErrorMsg("for hull vertex.");
						return false;
					}
				}
			}
			else if (lineHeader == "vn")
			{
				// line represents vertex normal
				stream << line.substr(2);
				
				glm::vec3 normal;
				stream >> normal.x; stream >> normal.y; stream >> normal.z;
				normal = glm::normalize(normal);
				model->normals.push_back(normal);
			}
			else if (lineHeader == "vt")
			{
				// line represents texture coordinate, not implemented
			}
			else if (lineHeader == "f ")
			{
				// line represents face

				std::string indices[3];
				
				int vertexIndices[3];
				int normalIndices[3];

				stream << line.substr(2);
				stream >> indices[0]; stream >> indices[1]; stream >> indices[2];

				const char *ind[3] = {
					indices[0].c_str(),
					indices[1].c_str(),
					indices[2].c_str() };

				for (int i = 0; i < 3; i++)
				{
					sscanf(ind[i], "%d//%d", &(vertexIndices[i]), &(normalIndices[i]));

					// decrease value of index, because .obj file starts indexing from 1
					vertexIndices[i]--;
					normalIndices[i]--;
				}

				glm::vec3 normal = model->normals[normalIndices[0]];

				if (modelType == hull || modelType == plane)
				{
					Hull* hull = dynamic_cast<Hull*>(model->shape);
					if (!hull->addFace(vertexIndices[0], vertexIndices[1], vertexIndices[2], normal))
					{
						printAllocErrorMsg("for hull face.");
						return false;
					}
				}

				for (int i = 0; i < 3; i++)
				{
					Vertex v;
					v.position = model->vertices[vertexIndices[i]];
					v.normal = model->normals[normalIndices[i]];

					p = verticesMap.insert(std::make_pair(v, index));

					if (p.second == false)
					{
						// same vertex is already in vertex buffer, insert its index into the index buffer
						model->indexBuffer.push_back((*(p.first)).second);
					}
					else
					{
						// vertex is not yet in vertex buffer
						model->vertexBuffer.push_back(v);
						model->indexBuffer.push_back(index);
						index++;
					}
				}
			}
		}

		if (modelType == hull || modelType == plane)
		{
			Hull* hull = dynamic_cast<Hull*>(model->shape);
			hull->finalizeBuild();
		}

		// store pointer to the loaded model
		models.push_back(model);
		std::cout << "Loaded" << std::endl;
		file.close();
	}
	else
	{
		std::cout << "Loading failed! Could not open file " << modelFile << std::endl;
		result = false;
	}
	return result;
}

void ModelManager::deleteModel(std::string name)
{
	for (unsigned int i = 0; i < models.size(); i++)
	{
		if (models[i]->modelName == name)
		{
			delete models[i];
			models.erase(models.begin() + i);
		}
	}
}

Model* ModelManager::getModel(std::string name)
{
	std::vector <Model*>::iterator it;

	for (it = models.begin(); it != models.end(); it++)
	{
		if ((*it)->modelName == name)
			return *it;
	}
	return NULL;
}

void ModelManager::createVAOs()
{
	std::vector <Model*>::iterator it;

	//for (unsigned int i = 0; i < models.size(); i++)
	for (it = models.begin(); it != models.end(); it++)
	{
		Model *model = *it;
		model->verticesCount = model->indexBuffer.size();

		unsigned int vbo, ebo;

		// generate buffers
		glGenVertexArrays(1, &(model->vao));
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		
		glBindVertexArray(model->vao);

		// buffer for vertex data
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, model->vertexBuffer.size() * sizeof(Vertex), &(model->vertexBuffer[0]), GL_STATIC_DRAW);

		// buffer for indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->verticesCount * sizeof(unsigned int), &(model->indexBuffer[0]), GL_STATIC_DRAW);

		// vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		// vertex normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
		glEnableVertexAttribArray(1);

		// data are stored in GPU so they can be deleted from memory
		model->vertexBuffer.clear();
		model->indexBuffer.clear();
		model->vertices.clear();
		model->normals.clear();
	}
}

void ModelManager::insertEdge(unsigned int vertex0, unsigned int vertex1, unsigned int normal, std::map<std::pair<unsigned int, unsigned int>, unsigned int>* edges)
{
	// find if same edge with opposite direction is already in map
	auto iterator = edges->find(std::make_pair(vertex1, vertex0));

	// edge with opposite direction is not there yet
	if (iterator == edges->end())
	{
		std::pair<unsigned int, unsigned int> edge = std::make_pair(vertex0, vertex1);
		edges->insert(std::make_pair(edge, normal));
	}
	else
	{
		// it has same normal so it is not reallly an edge of the object 
		if ((*iterator).second == normal)
		{
			edges->erase(iterator);
		}
	}
}

void ModelManager::printAllocErrorMsg(std::string what)
{
	std::cout << "Couldn't allocate memory for " << what << std::endl;
}
