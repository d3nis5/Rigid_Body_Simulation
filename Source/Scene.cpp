/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Scene.cpp
 *
 */

#include "Scene.h"

Scene::Scene()
{
	camera = Camera(glm::vec3(0.0f, 2.0f, 20.0f));
	modelManager = ModelManager();
}


Scene::~Scene()
{
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		delete *it;
	}
	objects.clear();
}

bool Scene::loadScene(fs::path filename)
{
	bool rtrnVal = true;

	std::ifstream file;
	file.open(filename);

	auto fillVector = [](glm::vec3& vector, std::string& line)
	{
		std::stringstream stream;
		stream << line;
		stream >> vector.x; stream >> vector.y; stream >> vector.z;
	};

	if (file.is_open())
	{
		// line read from file
		std::string line;

		// read file
		while (file)
		{
			try
			{
				std::getline(file, line);

				if (line.length() < 2)
					continue;

				std::string lineHeader = line.substr(0, 2);
				std::stringstream stream;

				stream << line.substr(2);

				if (lineHeader == "m ")
				{
					// model to be loaded from file
					char modelType;
					float radius = 0.0f;
					std::string modelName;
					std::string modelPath;
					ShapeType type = none;

					stream >> modelType;
					stream >> modelName;
					stream >> modelPath;

					switch (modelType)
					{
					case 's':
						type = sphere;
						stream >> radius;
						break;
					case 'h':
						type = hull;
						break;
					case 'p':
						type = plane;
						break;
					}

					if (type == none)
					{
						rtrnVal = false;
						break;
					}

					fs::path pathToModel = std::experimental::filesystem::u8path(ROOT_DIR);
					fs::path model = std::experimental::filesystem::path(modelPath);
					pathToModel += model;

					if (!modelManager.loadModel(pathToModel, modelName, type, radius))
					{
						rtrnVal = false;
						break;
					}
				}
				else if (lineHeader == "o ")
				{
					// object to be created in scene

					Object::ObjectInit object;
					std::string modelName;
					std::string objectDensity;
					stream >> object.objectName; stream >> modelName; stream >> objectDensity;

					if (objectDensity == "INFINITY")
						object.density = INFINITY;
					else
					{
						object.density = std::stof(objectDensity);
					}

					if (object.density <= 0.0f)
					{
						std::cout << "Density of an object must be greater than 0" << std::endl;
						rtrnVal = false;
						break;
					}

					// object color
					std::getline(file, line);
					fillVector(object.color, line);

					// object position
					std::getline(file, line);
					fillVector(object.position, line);

					// object rotation
					std::getline(file, line);
					fillVector(object.rotation, line);

					// velocity vector of an object
					std::getline(file, line);
					fillVector(object.initialVelocity, line);

					object.model = modelManager.getModel(modelName);

					if (object.model == NULL)
					{
						std::cout << "Model requested for object " << object.objectName << " not found" << std::endl;
						rtrnVal = false;
						break;
					}
					Object *obj;
					try
					{
						obj = new Object(object);
					}
					catch (const std::bad_alloc &ba)
					{
						std::cout << "Couldn't allocate memory: " << ba.what() << std::endl;
						rtrnVal = false;
						break;
					}
					objects.push_back(obj);
				}
			}
			catch (...)
			{
				std::cout << "Wrong format of scene file" << std::endl;
				rtrnVal = false;
				break;
			}
		}
		file.close();
	}
	else
	{
		std::cout << "Couldn't open file " << filename << std::endl;
		rtrnVal = false;
	}
	return rtrnVal;
}

Camera* Scene::getCamera()
{
	return &camera;
}
