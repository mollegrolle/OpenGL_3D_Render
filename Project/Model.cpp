#include "Model.h"



Model::Model()
{
}

Model::Model(std::string path)
{
	loadModelAssimp(path);
}

Model::Model(std::vector<glm::vec3> vertices, std::vector<glm::vec2> texCoords, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents)
{
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<Texture> mTextures;

	//function that calcs tangent and bi tangent

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		Vertex vector;

		vector.Position = vertices[i];
		vector.TexCoords = texCoords[i];
		vector.Normal = normals[i];
		vector.Tangent = tangents[i];
		vector.Bitangent = bitangents[i];

		mVertices.push_back(vector);

		mIndices.push_back(i);
	}

	std::vector<std::string> types{ "diffuse", "specular", "normal", "height" };

	std::vector<glm::vec4> colors{ 
		glm::vec4(1.0, 1.0, 1.0, 1.0), //diffuse
		glm::vec4(1.0, 1.0, 1.0, 1.0), //specular
		glm::vec4(0.5, 0.5, 1.0, 1.0), //normal
		glm::vec4(0.0, 0.0, 0.0, 1.0)  //height
	};

	for (int i = 0; i < 4; i++)
	{
		Texture texture;

		texture.id = CustomTextureColor(colors[i]);
		texture.type = types[i];
		texture.path = "noPath";
		mTextures.push_back(texture);
	}

	

	meshes.push_back(Mesh(mVertices, mIndices, mTextures));
}

Model::Model(std::string path, char a)
{
	//---OLD---
	//Obj Parser
	//Mesh(vertices, indices, textures);

	std::vector<glm::vec3> position; //pos to temp vertex
	std::vector<glm::vec2> texCoord; //tex to temp vertex
	std::vector<glm::vec3> normal; //nor to temp vertex
	std::vector<glm::vec3> tangent; //tangent to temp vertex
	std::vector<glm::vec3> bitangent; //bitangent to temp vertex

	//Texture
	std::vector<Texture> textures(4);

	//Variables for read
	std::ifstream objFile;
	std::ifstream mtlFile;

	std::string line;
	objFile.open(path);

	if (path.find("/") != std::string::npos)
		directory = path.substr(0, path.find_last_of('/')) + '/';
	else
		directory = "";


	if (!objFile.is_open())
	{
		std::cout << path << " not found" << std::endl;
	}

	else
	{
		while (std::getline(objFile, line))
		{

			std::vector<Vertex> vertices; //vertex array to Mesh

			//reverseArr
			std::vector<glm::vec2> revArr;

			//Indices
			std::vector<unsigned int> indices;


			if (line.size() <= 0)
			{
				std::getline(objFile, line);
			}
			

			switch (line.at(0))
			{

			case 'm':

				if (line.substr(0, 7) == "mtllib ")
				{
					//Material library
					int index = 7;
					int firstChar = index, lastChar = index;
					while (line[index] != ' ' && line.length() > index) {
						index++;
						lastChar = index;
					}

					std::string mtlPath = directory + line.substr(firstChar, lastChar - firstChar).c_str();
					mtlFile.open(mtlPath);
				}

				break;

			case 'v':

				//Gather vertex data
				if (line.substr(0, 2) == "v ")
				{
					//Vertices position
					float pos[3]{ 0,0,0 };

					int index = 0;
					for (int i = 0; i < 3; i++)
					{

						while (line[index] == 'v' || line[index] == ' ')
							index++;

						int firstChar = index, lastChar = index;
						while (line[index] != ' ' && line.length() - 1 > index) {
							index++;
							lastChar = index;
						}
						pos[i] = (float)(std::atof(line.substr(firstChar, lastChar - firstChar).c_str()));
					}
					position.push_back(glm::vec3(pos[0], pos[1], pos[2]));

				}

				if (line.substr(0, 3) == "vt ")
				{
					//Vertices texture
					float tex[2];

					int index = 0;
					for (int i = 0; i < 2; i++)
					{

						while (line[index] == 'v' || line[index] == 't' || line[index] == ' ')
							index++;

						int firstChar = index, lastChar = index;
						while (line[index] != ' ' && line.length() - 1 > index) {
							index++;
							lastChar = index;
						}
						tex[i] = (float)(atof(line.substr(firstChar, lastChar - firstChar).c_str()));
					}
					texCoord.push_back(glm::vec2(tex[0], tex[1]));

				}

				if (line.substr(0, 3) == "vn ")
				{
					//Vertices normal
					float norm[3];

					int index = 0;
					for (int i = 0; i < 3; i++)
					{

						while (line[index] == 'v' || line[index] == 'n' || line[index] == ' ')
							index++;

						int firstChar = index, lastChar = index;
						while (line[index] != ' ' && line.length() - 1 > index) {
							index++;
							lastChar = index;
						}
						norm[i] = (float)(atof(line.substr(firstChar, lastChar - firstChar).c_str()));
					}
					normal.push_back(glm::vec3(norm[0], norm[1], norm[2]));

				}
				
				break;

			case 'u':

				if (line.substr(0, 7) == "usemtl ")
				{
					//Use material

					std::string objMat;

					int index = 7;
					int firstChar = index, lastChar = index;
					while (line[index] != ' ' && line.length() > index) {
						index++;
						lastChar = index;
					}

					objMat = line.substr(firstChar, lastChar - firstChar).c_str();

					std::string mtlLine;
					bool complete = false;
					mtlFile.clear();
					mtlFile.seekg(0, mtlFile.beg);

					while (std::getline(mtlFile, mtlLine) && complete == false)
					{
						if (mtlLine.compare(0, 2, "# ") == 0)
						{
							//Comment

						}

						if (mtlLine.compare(0, 7, "newmtl ") == 0)
						{
							//New material

							std::string mtlMat;

							int mtlIndex = 7;
							int firstChar = mtlIndex, lastChar = mtlIndex;
							while (mtlLine[mtlIndex] != ' ' && mtlLine.length() > mtlIndex) {
								mtlIndex++;
								lastChar = mtlIndex;
							}
							mtlMat = mtlLine.substr(firstChar, lastChar - firstChar).c_str();

							if (objMat == mtlMat)
							{
								//Check if material name in mtl file is the same
								//as the material name in obj file.

								while (std::getline(mtlFile, mtlLine) && complete == false)
								{


									if (mtlLine.substr(0, 3) == "Ns ")
									{

									}

									if (mtlLine.substr(0, 3) == "Ka ")
									{

									}

									if (mtlLine.substr(0, 3) == "Kd ")
									{

									}

									if (mtlLine.substr(0, 3) == "Ks ")
									{

									}

									if (mtlLine.substr(0, 3) == "Ni ")
									{

									}

									if (mtlLine.substr(0, 2) == "d ")
									{

									}

									if (mtlLine.substr(0, 6) == "illum ")
									{

									}

									if (mtlLine.substr(0, 7) == "map_Kd ")
									{

										int mtlIndex = 7;
										int firstChar = mtlIndex, lastChar = mtlIndex;
										while (mtlLine[mtlIndex] != ' ' && mtlLine.length() > mtlIndex) {
											mtlIndex++;
											lastChar = mtlIndex;
										}

										std::string texPath;
										texPath = mtlLine.substr(firstChar, lastChar - firstChar).c_str();

										Texture texture;

										// if texture hasn't been loaded already, load it

										texture.id = TextureFromFile(texPath.c_str(), directory);
										texture.type = "diffuse";
										texture.path = texPath.c_str();
										textures_loaded.push_back(texture); // add to loaded textures (this is to check if texture has been loaded for this mesh)

										textures[0] = texture;
									}

									if (mtlLine.substr(0, 9) == "map_Bump ")
									{
										int mtlIndex = 9;
										int firstChar = mtlIndex, lastChar = mtlIndex;
										while (mtlLine[mtlIndex] != ' ' && mtlLine.length() > mtlIndex) {
											mtlIndex++;
											lastChar = mtlIndex;
										}

										std::string texPath;
										texPath = mtlLine.substr(firstChar, lastChar - firstChar).c_str();

										Texture texture;

										// if texture hasn't been loaded already, load it

										texture.id = TextureFromFile(texPath.c_str(), directory);
										texture.type = "normal";
										texture.path = texPath.c_str();
										textures_loaded.push_back(texture); // add to loaded textures (this is to check if texture has been loaded for this mesh)

										textures[2] = texture;
									}

									if (mtlLine.substr(0, 7) == "map_Ks ")
									{
										int mtlIndex = 7;
										int firstChar = mtlIndex, lastChar = mtlIndex;
										while (mtlLine[mtlIndex] != ' ' && mtlLine.length() > mtlIndex) {
											mtlIndex++;
											lastChar = mtlIndex;
										}

										std::string texPath;
										texPath = mtlLine.substr(firstChar, lastChar - firstChar).c_str();

										Texture texture;

										// if texture hasn't been loaded already, load it

										texture.id = TextureFromFile(texPath.c_str(), directory);
										texture.type = "specular";
										texture.path = texPath.c_str();
										textures_loaded.push_back(texture); // add to loaded textures (this is to check if texture has been loaded for this mesh)

										textures[1] = texture;
										
									}
									if (mtlLine.size() == 0)
									{
										complete = true;
										break;
									}
								}
							}
						}
					}

					//if mtl file don't open then procced to default texture
					if (!mtlFile.is_open())
					{
						std::vector<std::string> types{ "diffuse", "specular", "normal", "height" };

						std::vector<glm::vec4> colors{
							glm::vec4(1.0, 1.0, 1.0, 1.0), //diffuse
							glm::vec4(1.0, 1.0, 1.0, 1.0), //specular
							glm::vec4(0.5, 0.5, 1.0, 1.0), //normal
							glm::vec4(0.0, 0.0, 0.0, 1.0)  //height
						};


						for (int i = 0; i < 4; i++)
						{

							Texture texture;
							texture.id = CustomTextureColor(colors[i]);
							texture.type = types[i];
							texture.path = "noPath";
							textures[i] = texture;
							textures_loaded.push_back(texture);
						}

					}

				}
				break;

			case 'f':

				while (line.substr(0, 2) == "f " || line.substr(0, 2) == "s ")
				{
					while (line.substr(0, 1) == "s")
						std::getline(objFile, line); //skip smooth

					int v[3], vt[3], vn[3];

					const char* chh = line.c_str();
					sscanf_s(chh, "f %i/%i/%i %i/%i/%i %i/%i/%i", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2]);
					
					v[0]--; v[1]--; v[2]--;
					vt[0]--; vt[1]--; vt[2]--;
					vn[0]--; vn[1]--; vn[2]--;

					for (int i = 0; i < 3; i++)
					{
						Vertex vertex; //temp Vertex to pushback in vertex ar

						vertex.Position = position[v[i]];
						vertex.TexCoords = glm::vec2(texCoord[vt[i]].x, -texCoord[vt[i]].y);
						vertex.Normal = normal[vn[i]];

						vertices.push_back(vertex);
					}

					std::getline(objFile, line);
					//assume that all data for one vertex in complete 

				}

				

				for (int i = 0; i < vertices.size(); i++)
				{
					indices.push_back(i);
				}

				meshes.push_back(Mesh(vertices, indices, textures));
				break;

			default:
				break;
			}

			//end of .obj file
		}
	}
}

void Model::loadModelAssimp(std::string path)
{
	Assimp::Importer import;

	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	if (path.find("/") != std::string::npos)
		directory = path.substr(0, path.find_last_of('/'));
	else
		directory = "";

	processScene(scene);
}

void Model::loadModelObjParse(const char *path)
{
	//Vertices
	std::vector<Vertex> vertices;
	//Indices
	std::vector<unsigned int> indices;
	//Texture
	std::vector<Texture> textures(4);

	Vertex vertex;
	std::vector<glm::vec3> position;
	std::vector<glm::vec2> texCoord;
	std::vector<glm::vec3> normal;
	std::vector<glm::vec3> tangent;
	std::vector<glm::vec3> bitangent;

	loadOBJ(path, position, texCoord, normal, tangent, bitangent);

	for (int i = 0; i < position.size(); i++)
	{
		vertex.Position = position[i];
		vertex.TexCoords = texCoord[i];
		vertex.Normal = normal[i];
		vertex.Tangent = tangent[i];
		vertex.Tangent = bitangent[i];

		vertices.push_back(vertex);
	}

	std::vector<std::string> types{ "diffuse", "specular", "normal", "height" };
	std::vector<glm::vec4> colors{
		glm::vec4(1.0, 1.0, 1.0, 1.0), //diffuse
		glm::vec4(1.0, 1.0, 1.0, 1.0), //specular
		glm::vec4(0.5, 0.5, 1.0, 1.0), //normal
		glm::vec4(0.0, 0.0, 0.0, 1.0)  //height
	};
	for (int i = 0; i < 4; i++)
	{
		Texture texture;
		texture.id = CustomTextureColor(colors[i]);
		texture.type = types[i];
		texture.path = "noPath";
		textures[i] = texture;
		textures_loaded.push_back(texture);
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		indices.push_back(i);
	}

	meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::loadMaterial(std::string path, const std::string typeName)
{

	for (int i = 0; i < meshes.size(); i++)
	{

		Texture texture;

		// if texture hasn't been loaded already, load it

		if (path.find("/") != std::string::npos)
			directory = path.substr(0, path.find_last_of('/'));
		else
			directory = "";

		texture.id = TextureFromFile(path.c_str(), directory);
		texture.type = typeName;
		texture.path = path.c_str();
		textures_loaded.push_back(texture); // add to loaded textures (this is to check if texture has been loaded for this mesh)

		//We know what path has been loaded but we dont know how it's being used in meshes.textures
		//How is the texture being used by the meshes aka what TYPE
		//The texture can be loaded but meshes might use it by a different type

		for (int j = 0; j < meshes[i].textures.size(); j++)
		{
			if (meshes[i].textures[j].type == texture.type)
			{
				meshes[i].textures[j] = texture;
			}
		}

	}
	
}

void Model::setColor(glm::vec3 color)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].color = color;
	}
}

void Model::setDiffuse(glm::vec3 diffuse)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, meshes[i].textures[0].id);
		glDeleteTextures(1, &meshes[i].textures[0].id);
		meshes[i].textures[0].id = CustomTextureColor(glm::vec4(diffuse, 0));
	}
}

void Model::setSpecular(glm::vec3 specular)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, meshes[i].textures[1].id);
		glDeleteTextures(1, &meshes[i].textures[1].id);
		meshes[i].textures[1].id = CustomTextureColor(glm::vec4(specular, 0));
	}
}

void Model::setNormal(glm::vec3 normal)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, meshes[i].textures[2].id);
		glDeleteTextures(1, &meshes[i].textures[2].id);
		meshes[i].textures[2].id = CustomTextureColor(glm::vec4(normal, 0));
	}
}

void Model::setHeight(glm::vec3 height)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, meshes[i].textures[3].id);
		glDeleteTextures(1, &meshes[i].textures[3].id);
		meshes[i].textures[3].id = CustomTextureColor(glm::vec4(height, 0));
	}
}


void Model::Draw(Shader shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::setTranslate(glm::vec3 translate)
{
	transform.setTranslate(translate);
}

void Model::setRotate(glm::vec3 rotate)
{
	transform.setRotate(rotate);
}

void Model::setScale(glm::vec3 scale)
{
	transform.setScale(scale);
}

void Model::setDrawMode(GLenum drawMode)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].setDrawMode(drawMode);
	}
}

glm::vec3 Model::getTranslate()
{
	return transform.getTranslate();
}

glm::vec3 Model::getRotate()
{
	return transform.getRotate();
}

glm::vec3 Model::getScale()
{
	return transform.getScale();
}

glm::mat4 Model::getModel()
{
	return transform.getModel();
}

aiScene *Model::generateScene(std::vector<glm::vec3> vertices1, std::vector<glm::vec2> texCoords1, std::vector<glm::vec3> normals1)
{
	aiScene *scene = new aiScene();

	scene->mRootNode = new aiNode();
		 
	scene->mMaterials = new aiMaterial*[1];
	scene->mMaterials[0] = nullptr;
	scene->mNumMaterials = 1;
		 
	scene->mMaterials[0] = new aiMaterial();
		 
	scene->mMeshes = new aiMesh*[1];
	scene->mMeshes[0] = nullptr;
	scene->mNumMeshes = 1;
		 
	scene->mMeshes[0] = new aiMesh();
	scene->mMeshes[0]->mMaterialIndex = 0;
		 
	scene->mRootNode->mMeshes = new unsigned int[1];
	scene->mRootNode->mMeshes[0] = 0;
	scene->mRootNode->mNumMeshes = 1;

	auto pMesh = scene->mMeshes[0];

	pMesh->mVertices = new aiVector3D[vertices1.size()];
	pMesh->mNormals = new aiVector3D[vertices1.size()];
	pMesh->mNumVertices = (unsigned int)vertices1.size();

	pMesh->mTextureCoords[0] = new aiVector3D[vertices1.size()];
	pMesh->mNumUVComponents[0] = (unsigned int)vertices1.size();

	int j = 0;
	for (auto itr = vertices1.begin(); itr != vertices1.end(); ++itr)
	{
		pMesh->mVertices[itr - vertices1.begin()] = aiVector3D(vertices1[j].x, vertices1[j].y, vertices1[j].z);
		pMesh->mNormals[itr - vertices1.begin()] = aiVector3D(normals1[j].x, normals1[j].y, normals1[j].z);
		pMesh->mTextureCoords[0][itr - vertices1.begin()] = aiVector3D(texCoords1[j].x, texCoords1[j].y, 0);
		j++;
	}

	pMesh->mFaces = new aiFace[vertices1.size() / 3];
	pMesh->mNumFaces = (unsigned int)(vertices1.size() / 3);

	int k = 0;
	for (int i = 0; i < (vertices1.size() / 3); i++)
	{
		aiFace &face = pMesh->mFaces[i];
		face.mIndices = new unsigned int[3];
		face.mNumIndices = 3;

		face.mIndices[0] = k;
		face.mIndices[1] = k + 1;
		face.mIndices[2] = k + 2;
		k = k + 3;
	}

	return scene;
}

void Model::processScene(const aiScene * scene)
{
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	if (true)
	{
		vertices = processVertices(mesh);
		indices = processIndices(mesh);
		textures = processTextures(mesh, scene);
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Vertex> Model::processVertices(aiMesh * mesh)
{
	std::vector<Vertex> vertices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//...
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		if (mesh->mTangents)
		{
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;
		}

		if (mesh->mBitangents)
		{
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}

		vertices.push_back(vertex);
	}

	return vertices;
}

std::vector<unsigned int> Model::processIndices(aiMesh * mesh)
{
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return indices;
}

std::vector<Texture> Model::processTextures(aiMesh * mesh, const aiScene * scene)
{
	std::vector<Texture> textures;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		//Checks the existsing type of material by going through diffuse, specular, normal, height
		//If the material type doesn't exist, it return an empty std::vector

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = loadMaterialTextures(material,
			aiTextureType_NORMALS, "height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return textures;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures (this is to check if texture has been loaded for this mesh)
		}
	}

	//If no texture is found, then put a default texture on it

	std::vector<std::string> types{ "diffuse", "specular", "normal", "height" };

	std::vector<glm::vec4> colors{
		glm::vec4(1.0, 1.0, 1.0, 1.0), //diffuse
		glm::vec4(1.0, 1.0, 1.0, 1.0), //specular
		glm::vec4(0.5, 0.5, 1.0, 1.0), //normal
		glm::vec4(0.0, 0.0, 0.0, 1.0)  //height
	};

	if (mat->GetTextureCount(type) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (typeName == types[i])
			{
				Texture texture;
				texture.id = CustomTextureColor(colors[i]);
				texture.type = typeName;
				texture.path = "noPath";
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}
	}

	return textures;
}

unsigned int CustomTextureColor(glm::vec4 color)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	unsigned char *data = new unsigned char[4];

	// Allocate the needed space.
	int width;
	int height;
	width = 1;
	height = 1;

	data[0] = color.x * 255;	//Red
	data[1] = color.y * 255;	//Green
	data[2] = color.z * 255;	//Blue
	data[3] = color.w * 255;	//Alpha


	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);


	return textureID;
}

unsigned int TextureFromFile(const char * path, const std::string & directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);
	unsigned char* data;

	int width, height, nrComponents;
	data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);


	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//stbi_
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void savedStuff() {

	//Default Fill Location Vector    

	std::vector <glm::vec3> vertice;

	int draw_order[36] =
	{
		0,2,1,      2,3,1,
		1,3,5,      3,7,5,
		5,7,4,      7,6,4,
		4,6,0,      6,2,0,
		4,0,5,      0,1,5,
		2,6,3,      6,7,3
	};

	glm::vec3 data[8] =
	{
		glm::vec3(-1.0f / 2.0f,1.0f / 2.0f,1.0f / 2.0f),
		glm::vec3(1.0f / 2.0f,1.0f / 2.0f,1.0f / 2.0f),
		glm::vec3(-1.0f / 2.0f,-1.0f / 2.0f,1.0f / 2.0f),
		glm::vec3(1.0f / 2.0f,-1.0f / 2.0f,1.0f / 2.0f),
		glm::vec3(-1.0f / 2.0f,1.0f / 2.0f,-1.0f / 2.0f),
		glm::vec3(1.0f / 2.0f,1.0f / 2.0f,-1.0f / 2.0f),
		glm::vec3(-1.0f / 2.0f,-1.0f / 2.0f,-1.0f / 2.0f),
		glm::vec3(1.0f / 2.0f,-1.0f / 2.0f,-1.0f / 2.0f)
	};

	for (int i = 0; i < 36; i++)
	{
		vertice.push_back(data[draw_order[i]]);
	}


	//Default Fill Normal Vector

	std::vector<glm::vec3> normal;

	for (int i = 0; i < 36; i++)
	{
		if (i < 6) { normal.push_back(glm::vec3(0, 0, 1)); }
		else if (i < 12) { normal.push_back(glm::vec3(1, 0, 0)); }
		else if (i < 18) { normal.push_back(glm::vec3(0, 0, -1)); }
		else if (i < 24) { normal.push_back(glm::vec3(-1, 0, 0)); }
		else if (i < 30) { normal.push_back(glm::vec3(0, 1, 0)); }
		else if (i < 36) { normal.push_back(glm::vec3(0, -1, 0)); }
	}


	//Default Fill UV Vector

	std::vector<glm::vec2> uv;

	for (int i = 0; i < 6; i++)
	{
		uv.push_back(glm::vec2(0, 1));
		uv.push_back(glm::vec2(0, 0));
		uv.push_back(glm::vec2(1, 1));
		uv.push_back(glm::vec2(0, 0));
		uv.push_back(glm::vec2(1, 0));
		uv.push_back(glm::vec2(1, 1));
	}
}
