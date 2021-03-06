#include "Model.h"

Model::Model()
{
	vertexBuffer = uvBuffer = normalBuffer = 0;
	renderUVs = renderNormals = true;
	renderMode = GL_TRIANGLES;
}

Model::~Model()
{
	if (vertexBuffer != 0)	glDeleteBuffers(1, &vertexBuffer);
	if (uvBuffer != 0)		glDeleteBuffers(1, &uvBuffer);
	if (normalBuffer != 0)	glDeleteBuffers(1, &normalBuffer);
}

Model * Model::loadModel(string path)
{
	if (path == "Models\\quad.obj") return loadQuadModel();

	FILE * file = 0;
	file = fopen(path.c_str(), "rb");
	if (file == NULL) return loadErrorModel(); //File not found, generate error Model

	if (strcmp(path.c_str() + path.size()-4, ".obj") == 0 || strcmp(path.c_str() + path.size()-4, ".OBJ") == 0)
		return loadOBJ(file);

	fclose(file); return loadErrorModel(); //not an accepted filetype
}

Model * Model::loadHeightFieldModel(float * heightData, int width, int length)
{
	Model * m = new Model();
	m->setRenderMode(GL_QUADS);
	m->setRenderNormals(true);
	m->setRenderUVs(false);

	vector< glm::vec3 > tempVertices;
	vector< glm::vec3 > tempNormals;
	int numVertices = 0;

	for (int x = 0; x < width-1; x ++) {
		for (int z = 0; z < length-1; z ++) {
			tempVertices.push_back(glm::vec3(x - width / 2 + 1, heightData[width * (z + 0) + (x + 1)], z - length / 2 + 0));
			tempVertices.push_back(glm::vec3(x - width / 2 + 0, heightData[width * (z + 0) + (x + 0)], z - length / 2 + 0));
			tempVertices.push_back(glm::vec3(x - width / 2 + 0, heightData[width * (z + 1) + (x + 0)], z - length / 2 + 1));
			tempVertices.push_back(glm::vec3(x - width / 2 + 1, heightData[width * (z + 1) + (x + 1)], z - length / 2 + 1));
			
			if (x == 0 || x == width - 2 || z == 0 || z == length - 2) {
				tempNormals.push_back(glm::vec3(0, 1, 0));
				tempNormals.push_back(glm::vec3(0, 1, 0));
				tempNormals.push_back(glm::vec3(0, 1, 0));
				tempNormals.push_back(glm::vec3(0, 1, 0));
			}
			else {
				tempNormals.push_back(glm::normalize(glm::vec3(heightData[width * (z + 0) + (x + 0)] - heightData[width * (z + 0) + (x + 2)], 1.0f, heightData[width * (z - 1) + (x + 1)] - heightData[width * (z + 1) + (x + 1)])));
				tempNormals.push_back(glm::normalize(glm::vec3(heightData[width * (z + 0) + (x - 1)] - heightData[width * (z + 0) + (x + 1)], 1.0f, heightData[width * (z - 1) + (x + 0)] - heightData[width * (z + 1) + (x + 0)])));
				tempNormals.push_back(glm::normalize(glm::vec3(heightData[width * (z + 1) + (x - 1)] - heightData[width * (z + 1) + (x + 1)], 1.0f, heightData[width * (z + 0) + (x + 0)] - heightData[width * (z + 2) + (x + 0)])));
				tempNormals.push_back(glm::normalize(glm::vec3(heightData[width * (z + 1) + (x + 0)] - heightData[width * (z + 1) + (x + 2)], 1.0f, heightData[width * (z + 0) + (x + 1)] - heightData[width * (z + 2) + (x + 1)])));
			}
			numVertices += 4;
		}
	}

	m->numVertices = numVertices;

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, tempVertices.size() * sizeof(glm::vec3), &tempVertices[0], GL_STATIC_DRAW);
	tempVertices.clear();

	glGenBuffers(1, &m->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, tempNormals.size() * sizeof(glm::vec3), &tempNormals[0], GL_STATIC_DRAW);
	tempNormals.clear();

	return m;
}

Model * Model::loadOBJ(FILE * file)
{
	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector< glm::vec3 > temp_vertices;
	vector< glm::vec2 > temp_uvs;
	vector< glm::vec3 > temp_normals;

	while (true)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.
 
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9) { return loadErrorModel(); /*Couldn't Parse File*/ }
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}

	Model * m = new Model();
	if (m == NULL) return loadErrorModel(); /* Failed to allocate memory */

	GLfloat * vertexBufferData = new GLfloat[vertexIndices.size()*3]; 
	GLfloat * uvBufferData = new GLfloat[uvIndices.size()*2];
	GLfloat * normalBufferData = new GLfloat[normalIndices.size()*3]; 

	vector < glm::vec3 > out_vertices;
    vector < glm::vec2 > out_uvs;
    vector < glm::vec3 > out_normals;

	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		out_vertices.push_back(vertex);
	}

	for( unsigned int i=0; i<uvIndices.size(); i++ ){
		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		out_uvs.push_back(uv);
	}

	for( unsigned int i=0; i<normalIndices.size(); i++ ){
		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		out_normals.push_back(normal);
	}

	m->numVertices = vertexIndices.size();

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);
	delete [] vertexBufferData;

	glGenBuffers(1, &m->uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(glm::vec2), &out_uvs[0], GL_STATIC_DRAW);
	delete [] uvBufferData;

	glGenBuffers(1, &m->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, out_normals.size() * sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);
	delete [] normalBufferData;

	fclose(file);

	return m;
}

int Model::render()
{
	if (activeShader == NULL) return -1;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	int vertexID = glGetAttribLocation(activeShader->getShaderHandle(), "vertex");
	glEnableVertexAttribArray(vertexID);
	glVertexAttribPointer(
	   vertexID,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   (void*)0            // array buffer offset
	);

	if (renderUVs) {
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		int uvID = glGetAttribLocation(activeShader->getShaderHandle(), "uv");
		glEnableVertexAttribArray(uvID);
		glVertexAttribPointer(
			uvID,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
	}
	
	if (renderNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		int normalID = glGetAttribLocation(activeShader->getShaderHandle(), "normal");
		glEnableVertexAttribArray(normalID);
		glVertexAttribPointer(
		   normalID,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		   3,                  // size
		   GL_FLOAT,           // type
		   GL_FALSE,           // normalized?
		   0,                  // stride
		   (void*)0            // array buffer offset
		);
	}

	// Draw the triangle !
	glDrawArrays(renderMode, 0, numVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
 
	glDisableVertexAttribArray(0);

	return 0;
}

Model * Model::loadErrorModel()
{
	Model * m = new Model();
	
	static const GLfloat vertexBufferData[] = {
		
		 0.5f,  0.5f,  0.5f,
	    -0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,

		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
	};

	m->numVertices = 36;

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	static const GLfloat uvBufferData[] = {
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
	};

	glGenBuffers(1, &m->uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);

	//sqr(1) = 3*sqr(0.55375)
	static const GLfloat normalBufferData[] = {
		 0.0f,  0.0f,  1.0f,
	     0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,

		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,

		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,

		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,

		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
	};

	glGenBuffers(1, &m->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferData), normalBufferData, GL_STATIC_DRAW);

	return m;
}

float mult(float x) {
	return 0.001f*(x * abs(x));
}

glm::vec3 expRad(float x, float y, float z)
{
	float rsq = x*x + z*z;

	float newX = x * 0.0001f * rsq;
	float newZ = z * 0.0001f * rsq;

	return glm::vec3(newX, 0, newZ);
}

Model * Model::loadQuadModel()
{
	Model * m = new Model();

	#pragma region TRIANGLE TEST
	static const GLfloat vertexBufferData[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
	     0.5f,  0.5f, 0.0f,
	     0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
	};

	m->numVertices = 6;

	glGenBuffers(1, &m->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

	static const GLfloat uvBufferData[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
	};

	glGenBuffers(1, &m->uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvBufferData), uvBufferData, GL_STATIC_DRAW);
	#pragma endregion

	static const GLfloat normalBufferData[] = {
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	   0.0f, 0.0f, -1.0f,
	};

	glGenBuffers(1, &m->normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalBufferData), normalBufferData, GL_STATIC_DRAW);

	return m;
}