#include "Texture.h"

Texture::Texture()
{
	imageUByteData	= 0;
	imageFloatData	= 0;
	imageDataType	= GL_UNSIGNED_BYTE;
	textureID		= 0;
	type			= -1;
	width = height	= -1;
	bitdepth		= -1;
}

Texture::~Texture()
{
	//if (imageUByteData != 0) delete [] imageUByteData;
	//if (imageFloatData != 0) delete [] imageFloatData;
	if (textureID != 0) glDeleteTextures (1, &textureID);
}

glm::vec4 Texture::getPixel(float x, float y)
{
	if (imageDataType == GL_UNSIGNED_BYTE) {
		if (type == GL_RGB) return glm::vec4(
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+0], 
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+1], 
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+2], 
			1.0f); 
		else if (type == GL_RGBA) return glm::vec4(
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+0], 
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+1], 
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+2], 
			imageUByteData[((int)(y*height)*width+(int)(x*width))*4+3]);
		else if (type == GL_DEPTH_COMPONENT) return glm::vec4(
			imageUByteData[((int)(y*height)*width+(int)(x*width))],
			1.0f, 1.0f, 1.0f);
		else return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (imageDataType == GL_FLOAT) {
		//return floats
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

Texture * Texture::loadTexture(string path)
{
	FILE * file = 0;
	file = fopen(path.c_str(), "rb");
	if (file == NULL) 
		return loadErrorTexture();	//File not found, generate error texture
	
	if (strcmp(path.c_str() + path.size()-4, ".tga") == 0 || strcmp(path.c_str() + path.size()-4, ".TGA") == 0)
		return loadTGA(file);

	return loadErrorTexture(); //not an accepted filetype
}

Texture * Texture::loadBlankTexture(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageUByteData = new GLubyte[imageSize];

	for (int x = 0; x < t->width; x++)
	{
		for (int y = 0; y < t->height; y++)
		{
			t->imageUByteData[(y*t->width+x)*4+0] = 0;
			t->imageUByteData[(y*t->width+x)*4+1] = 0;
			t->imageUByteData[(y*t->width+x)*4+2] = 0;
			t->imageUByteData[(y*t->width+x)*4+3] = 255;
		}
	}

	generateTexture(t, t->type);

	return t;
}

Texture * Texture::loadRandomizedTexture(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageUByteData = new GLubyte[imageSize];

	for (int x = 0; x < t->width; x++)
	{
		for (int y = 0; y < t->height; y++)
		{
			t->imageUByteData[(y*t->width+x)*4+0] = rand() % 256;
			t->imageUByteData[(y*t->width+x)*4+1] = rand() % 256;
			t->imageUByteData[(y*t->width+x)*4+2] = rand() % 256;
			t->imageUByteData[(y*t->width+x)*4+3] = rand() % 256;
		}
	}

	generateTexture(t, GL_RGBA);

	return t;
}

Texture * Texture::loadBlankFloatTexture(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA32F;
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageFloatData = new GLfloat[imageSize];

	for (int x = 0; x < t->width; x++)
	{
		for (int y = 0; y < t->height; y++)
		{
			t->imageFloatData[(y*t->width+x)*4+0] = 0.0f;
			t->imageFloatData[(y*t->width+x)*4+1] = 0.0f;
			t->imageFloatData[(y*t->width+x)*4+2] = 0.0f;
			t->imageFloatData[(y*t->width+x)*4+3] = 1.0f;
		}
	}

	//generateTexture(t, GL_RGBA);

	glGenTextures(1, &t->textureID);
    glBindTexture(GL_TEXTURE_2D, t->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, t->type, t->width, t->height, 0, GL_RGBA, GL_FLOAT, t->imageFloatData);
	delete [] t->imageFloatData;
	
	return t;
}

Texture * Texture::loadDepthTexture(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_DEPTH_COMPONENT32F;
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageFloatData = new GLfloat[imageSize];

	for (int i = 0; i < imageSize; i++) t->imageFloatData[i] = 0.0f;

 	generateTexture(t, GL_DEPTH_COMPONENT);
	t->type = GL_DEPTH_COMPONENT;
	return t;
}

Texture * Texture::loadComplexGaussianNumbersTexture(GLint width, GLint height) //Random Gaussian complex number stored in two colours
{
	Texture * t = new Texture();

	t->type = GL_RG32F; //GL_RG32F; //GL_RGBA32F; //GL_RGBA
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 16;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageFloatData = new GLfloat[imageSize];

	for (int i = 0; i < imageSize; i++) {
		t->imageFloatData[i] = grand();
	}

	//generateTexture(t, GL_RG);

	glGenTextures(1, &t->textureID);
    glBindTexture(GL_TEXTURE_2D, t->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, t->type, t->width, t->height, 0, GL_RG, GL_FLOAT, t->imageFloatData);
	delete [] t->imageFloatData;

	return t;
}

Texture * Texture::loadFFTTest(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA32F; //GL_RG32F; //GL_RGBA32F; //GL_RGBA
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth / 8;
	t->imageFloatData = new GLfloat[imageSize];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			t->imageFloatData[(y*width + x) * 4 + 0] = sin(x+y);
			t->imageFloatData[(y*width + x) * 4 + 1] = sin(x+y);
			t->imageFloatData[(y*width + x) * 4 + 2] = 0.0f;
			t->imageFloatData[(y*width + x) * 4 + 3] = 1.0f;
		}
	}

	//generateTexture(t, GL_RG);

	glGenTextures(1, &t->textureID);
	glBindTexture(GL_TEXTURE_2D, t->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, t->type, t->width, t->height, 0, GL_RGBA, GL_FLOAT, t->imageFloatData);
	delete[] t->imageFloatData;

	return t;
}

Texture * Texture::loadiFFTTest(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA32F; //GL_RG32F; //GL_RGBA32F; //GL_RGBA
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth / 8;
	t->imageFloatData = new GLfloat[imageSize];

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			t->imageFloatData[(y*width + x) * 4 + 0] = 0.0f;
			t->imageFloatData[(y*width + x) * 4 + 1] = 0.0f;
			t->imageFloatData[(y*width + x) * 4 + 2] = 0.0f;
			t->imageFloatData[(y*width + x) * 4 + 3] = 1.0f;
		}
	}

	t->imageFloatData[((height / 2)*width + width / 2) * 4 + 0] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2) * 4 + 1] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2) * 4 + 2] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2) * 4 + 3] = 1.0f;

	t->imageFloatData[((height / 2)*width + width / 2 - width/8) * 4 + 0] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 - width / 8) * 4 + 1] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 - width / 8) * 4 + 2] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 - width / 8) * 4 + 3] = 1.0f;

	t->imageFloatData[((height / 2)*width + width / 2 + width / 8) * 4 + 0] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 + width / 8) * 4 + 1] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 + width / 8) * 4 + 2] = 1.0f;
	t->imageFloatData[((height / 2)*width + width / 2 + width / 8) * 4 + 3] = 1.0f;

	//generateTexture(t, GL_RG);

	glGenTextures(1, &t->textureID);
	glBindTexture(GL_TEXTURE_2D, t->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, t->type, t->width, t->height, 0, GL_RGBA, GL_FLOAT, t->imageFloatData);
	delete[] t->imageFloatData;

	return t;
}

void BitReverse(float *Indices, int N, int n)
{
	unsigned int mask = 0x1;
	for (int j = 0; j < N; j++)
	{
		unsigned int val = 0x0;
		int temp = int(Indices[j]);
		for (int i = 0; i < n; i++)
		{
			unsigned int t = (mask & temp);
			val = (val << 1) | t;
			temp = temp >> 1;
		}
		Indices[j] = val;
	}
}

void GenerateIndices(float **Indices, int NumPoints, int NumButterflies)
{
	int numIters = 1;
	int offset, step;
	offset = NumPoints;
	for (int b = 0; b < NumButterflies; b++)
	{
		offset = offset / 2;
		step = 2 * offset;
		int p = 0;
		int start = 0;
		int end = step;
		for (int i = 0; i < numIters; i++)
		{
			for (int j = start, k = p, l = 0; j < end; j += 2, l += 2, k++)
			{
				Indices[b][j] = k;
				Indices[b][j + 1] = k + offset;
				Indices[b][l + end] = k;
				Indices[b][l + end + 1] = k + offset;
			}
			start += 2 * step;
			end += 2 * step;
			p += step;
		}

		numIters = numIters << 1;
	}

	BitReverse(Indices[NumButterflies-1], 2 * NumPoints, NumButterflies);
}

void GetWeights(int NumPoints, int K, float &Wr, float &Wi)
{
	Wr = float(cosl(2.0*M_PI*K / (float)NumPoints));
	Wi = float(-sinl(2.0*M_PI*K / (float)NumPoints));
}

void GenerateWeights(int NumPoints, int NumButterflies, float **Wr, float **Wi)
{
	int groups = NumPoints / 2;
	int numKs = 1;
	for (int i = 0; i < NumButterflies; i++)
	{
		int start = 0;
		int end = numKs;

		for (int b = 0; b < groups; b++)
		{
			for (int k = start, K = 0; k < end; k++, K++)
			{
				float wr, wi;
				GetWeights(NumPoints, K*groups, wr, wi);
				Wr[i][k] = wr;
				Wi[i][k] = wi;
				Wr[i][k + numKs] = -wr;
				Wi[i][k + numKs] = -wi;
			}
			start += 2 * numKs;
			end = start + numKs;
		}
		groups = groups >> 1;
		numKs = numKs << 1;
	}
}

GLfloat * Texture::computeButterflyLookupData(int FFTSize, int numPasses)
{
	GLfloat * pixelData = new GLfloat[FFTSize * numPasses * 4];

	float **Indices = new float*[numPasses];
	float **Wr = new float*[numPasses];
	float **Wi = new float*[numPasses];

	for (int i = 0; i < (int)numPasses; i++) {
		Indices[i] = new float[2 * FFTSize];
		Wr[i] = new float[FFTSize];
		Wi[i] = new float[FFTSize];
	}
	GenerateIndices(Indices, FFTSize, numPasses);
	GenerateWeights(FFTSize, numPasses, Wr, Wi);

	for (int pass = 0; pass < numPasses; pass++) {
		for (int x = 0; x < FFTSize; x++) {
			pixelData[(pass*FFTSize + x) * 4 + 0] = (float)(Indices[numPasses - pass - 1][2 * x] + 0.5) / float(FFTSize); // R
			pixelData[(pass*FFTSize + x) * 4 + 1] = (float)(Indices[numPasses - pass - 1][2 * x + 1] + 0.5) / float(FFTSize); // G
			pixelData[(pass*FFTSize + x) * 4 + 2] = Wr[pass][x]; // B
			pixelData[(pass*FFTSize + x) * 4 + 3] = Wi[pass][x]; // A
		}
	}

	for (int i = 0; i < (int)numPasses; i++)
	{
		delete Indices[i];
		delete Wr[i];
		delete Wi[i];
	}
	delete[] Indices;
	delete[] Wr;
	delete[] Wi;
	
	return pixelData;
}

Texture * Texture::loadButterflyLookupTexture(GLint width, GLint height)
{
	Texture * t = new Texture();

	t->type = GL_RGBA32F;
	t->imageDataType = GL_FLOAT;
	t->width = width;
	t->height = height;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;

	t->imageFloatData = computeButterflyLookupData(width, height);
	//generateTexture(t, GL_RGBA);

	glGenTextures(1, &t->textureID);
    glBindTexture(GL_TEXTURE_2D, t->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, t->type, t->width, t->height, 0, GL_RGBA, GL_FLOAT, t->imageFloatData);
	delete [] t->imageFloatData;
	
	return t;
}

Texture * Texture::loadErrorTexture()
{
	Texture * t = new Texture();

	t->type = GL_RGBA;
	t->width = t->height = 512;
	t->bitdepth = 32;

	GLint imageSize = t->width * t->height * t->bitdepth/8;
	t->imageUByteData = new GLubyte[imageSize];

	for (int x = 0; x < t->width; x++)
	{
		for (int y = 0; y < t->height; y++)
		{
			if ((int)(x/64+y/64) % 2 == 1)
			{
				t->imageUByteData[(y*t->width+x)*4+0] = 255;
				t->imageUByteData[(y*t->width+x)*4+1] = 255;
				t->imageUByteData[(y*t->width+x)*4+2] = 255;
				t->imageUByteData[(y*t->width+x)*4+3] = 255;
			}
			else 
			{
				t->imageUByteData[(y*t->width+x)*4+0] = 226;
				t->imageUByteData[(y*t->width+x)*4+1] = 156;
				t->imageUByteData[(y*t->width+x)*4+2] = 210;
				t->imageUByteData[(y*t->width+x)*4+3] = 255;
			}
		}
	}

	generateTexture(t, t->type);

	return t;
}

Texture * Texture::loadTGA(FILE * file)
{
	GLbyte header[12];

	// Uncompressed TGA Header
	GLubyte uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
	// Compressed TGA Header
	GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

	// Attempt To Read The File Header
	if (fread(&header, sizeof(GLbyte)*12, 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header */ } 

	// If The File Header Matches The Uncompressed Header
	if(memcmp(uTGAcompare, &header, sizeof(header)) == 0)
		return loadUncompressedTGA(file);

	// If The File Header Matches The Compressed Header
	else if(memcmp(cTGAcompare, &header, sizeof(header)) == 0)
		return loadCompressedTGA(file);

	return loadErrorTexture(); //error checking the type of TGA
}

Texture * Texture::loadUncompressedTGA(FILE * file)
{
	GLbyte header[12];

	// Attempt To Read Next 6 Bytes
	fseek(file, 12, 0);
	if (fread(header, sizeof(header), 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header*/ }

	Texture * t = new Texture();
	if (t == NULL) { return loadErrorTexture(); /* Not properly allocated */ }

	t->width  = header[1] * 256 + header[0];
	t->height = header[3] * 256 + header[2];
	t->bitdepth = header[4];

	// Make Sure All Information Is Valid
	if ((t->width <= 0) || (t->height <= 0) || ((t->bitdepth != 24) && (t->bitdepth !=32))) { delete t; return loadErrorTexture(); /* Bad Data*/ }

	if (t->bitdepth == 24)	t->type = GL_RGB;
	else					t->type = GL_RGBA;

	GLint bytesPerPixel = (t->bitdepth / 8);				// Calculate The BYTES Per Pixel
	GLint imageSize = bytesPerPixel * t->width * t->height;	// Calculate Memory Needed To Store Image

	// Allocate Memory
	t->imageUByteData = new GLubyte[imageSize];
	if (t->imageUByteData == NULL) { delete t; return loadErrorTexture(); /* Not Properly Allocated */ }

	// Attempt To Read All The Image Data
	fseek(file, 18, 0);
	if (fread(t->imageUByteData, 1, imageSize, file) != imageSize) { delete t; return false; /* Can't read the file*/	}

	if (t->type == GL_RGBA)
	{
		//swap 1st and 3rd bytes (R and B)
	    for(GLuint cswap = 0; cswap < (int)imageSize; cswap += bytesPerPixel)
	    {
	        // 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
	        t->imageUByteData[cswap] ^= t->imageUByteData[cswap+2] ^=
	        t->imageUByteData[cswap] ^= t->imageUByteData[cswap+2];
	    }
	}
 
    fclose(file);
	if (t->type == GL_RGB) generateTexture(t, GL_BGR);
	else if (t->type == GL_RGBA) generateTexture(t, GL_RGBA);
	return t;
}

Texture * Texture::loadCompressedTGA(FILE * file)
{
	GLbyte header[12];

	// Attempt To Read Next 6 Bytes
	fseek(file, 12, 0);
	if (fread(header, sizeof(header), 1, file) == 0) { return loadErrorTexture(); /* Couldn't read the header*/ }

	Texture * t = new Texture();
	if (t == NULL) { return loadErrorTexture(); /* Not properly allocated */ }

	t->width  = header[1] * 256 + header[0];
    t->height = header[3] * 256 + header[2];
    t->bitdepth = header[4];
	if ((t->width <= 0) || (t->height <= 0) || ((t->bitdepth != 24) && (t->bitdepth !=32))) { delete t; return loadErrorTexture(); }  

	if (t->bitdepth == 24)	t->type = GL_RGB;
	else					t->type = GL_RGBA;

	GLint bytesPerPixel = (t->bitdepth / 8);				// Calculate The BYTES Per Pixel
	GLint imageSize = bytesPerPixel * t->width * t->height;	// Calculate Memory Needed To Store Image

	t->imageUByteData = new GLubyte[imageSize];
	if (t->imageUByteData == NULL) { delete t; return loadErrorTexture(); /* Not Properly Allocated */ }

	GLuint pixelcount = t->height * t->width; // Number Of Pixels In The Image
	GLuint currentpixel = 0;            // Current Pixel We Are Reading From Data
	GLuint currentbyte  = 0;            // Current Byte We Are Writing Into Imagedata
	// Storage For 1 Pixel
	GLubyte * colorbuffer = new GLubyte[bytesPerPixel];

	fseek(file, 18, 0);

	do                      // Start Loop
	{
		GLubyte chunkheader = 0;            // Variable To Store The Value Of The Id Chunk
		if (fread(&chunkheader, sizeof(GLubyte), 1, file) == 0) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed to load chunk header */ }
	
		if(chunkheader < 128)                // If The Chunk Is A 'RAW' Chunk
		{                                                  
			chunkheader++;              // Add 1 To The Value To Get Total Number Of Raw Pixels

			// Start Pixel Reading Loop
			for(short counter = 0; counter < chunkheader; counter++)
			{
				// Try To Read 1 Pixel
				if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed To read File*/ }

				t->imageUByteData[currentbyte] = colorbuffer[2];        // Write The 'R' Byte
				t->imageUByteData[currentbyte + 1   ] = colorbuffer[1]; // Write The 'G' Byte
				t->imageUByteData[currentbyte + 2   ] = colorbuffer[0]; // Write The 'B' Byte
				if(bytesPerPixel == 4)                  // If It's A 32bpp Image...
				{
					t->imageUByteData[currentbyte + 3] = colorbuffer[3];    // Write The 'A' Byte
				}
				// Increment The Byte Counter By The Number Of Bytes In A Pixel
				currentbyte += bytesPerPixel;
				currentpixel++;                 // Increment The Number Of Pixels By 1
			}
		}
		else                        // If It's An RLE Header
		{
			chunkheader -= 127;         // Subtract 127 To Get Rid Of The ID Bit

			// Read The Next Pixel
			if(fread(colorbuffer, 1, bytesPerPixel, file) != bytesPerPixel) { delete [] colorbuffer; delete t; return loadErrorTexture(); /* Failed To read File*/ }

			// Start The Loop
			for(short counter = 0; counter < chunkheader; counter++)
			{
				// Copy The 'R' Byte
				t->imageUByteData[currentbyte] = colorbuffer[2];
				// Copy The 'G' Byte
				t->imageUByteData[currentbyte + 1   ] = colorbuffer[1];
				// Copy The 'B' Byte
				t->imageUByteData[currentbyte + 2   ] = colorbuffer[0];
				if(bytesPerPixel == 4)      // If It's A 32bpp Image
				{
					// Copy The 'A' Byte
					t->imageUByteData[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += bytesPerPixel;   // Increment The Byte Counter
				currentpixel++;             // Increment The Pixel Counter
			}
		}
	}    while(currentpixel < pixelcount);    // More Pixels To Read? ... Start Loop Over
    fclose(file);               // Close File

	if (t->type == GL_RGB) generateTexture(t, GL_BGR);
	else if (t->type == GL_RGBA) generateTexture(t, GL_RGBA);

	delete [] colorbuffer;

	return t;
}

bool Texture::generateTexture(Texture * t, GLenum fileFormat)
{
    glGenTextures(1, &t->textureID);
    glBindTexture(GL_TEXTURE_2D, t->textureID);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	const unsigned char * ver = glGetString(GL_VERSION);
	string v = "";
	int i = 0;
	while (ver[i] != ' ') { v += ver[i]; i++; }

	float version = atof(v.c_str());
	
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (t->imageDataType == GL_UNSIGNED_BYTE) {
		glTexImage2D(
			GL_TEXTURE_2D, 0, t->type, 
			t->width, t->height,
			0,
			fileFormat, GL_UNSIGNED_BYTE, t->imageUByteData
		);
	}
	else if (t->imageDataType == GL_FLOAT) {
		glTexImage2D(
			GL_TEXTURE_2D, 0, t->type, 
			t->width, t->height,
			0,
			fileFormat, GL_FLOAT, t->imageFloatData
		);
	}

	if (version >= 3.0)
	{	
		glGenerateMipmap(GL_TEXTURE_2D); 

		/*glGenTextures(1, &t->textureID);
		glBindTexture(GL_TEXTURE_2D, t->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, t->imageData);
		glGenerateMipmap(GL_TEXTURE_2D);  //Generate mipmaps now!!!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);*/
	}
	else if (version >= 1.4)
	{
		glGenTextures(1, &t->textureID);
		glBindTexture(GL_TEXTURE_2D, t->textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); 
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); 
		if (t->imageDataType == GL_UNSIGNED_BYTE)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, t->imageUByteData);
		else if (t->imageDataType == GL_FLOAT)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->width, t->height, 0, GL_BGRA, GL_FLOAT, t->imageFloatData);
	}
	else
	{
		if (t->imageDataType == GL_UNSIGNED_BYTE)
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, t->width, t->height, GL_BGRA, GL_UNSIGNED_BYTE, t->imageUByteData);
		else if (t->imageDataType == GL_FLOAT)
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, t->width, t->height, GL_BGRA, GL_FLOAT, t->imageFloatData);
	}

    glBindTexture(GL_TEXTURE_2D, 0);

	if (t->imageUByteData != 0) delete [] t->imageUByteData;
	if (t->imageFloatData != 0) delete [] t->imageFloatData;
	return true;
}

void Texture::saveUncompressedTGA(string filepath, Texture * t)
{
	string ext = "";
	for (int i = filepath.length() - 1; filepath[i]!='.' && i >= 0; i--)
		ext = filepath[i] + ext;
	if (ext != "tga") return;
	if (t == NULL) return;

	remove(filepath.c_str());

	glActiveTexture(GL_TEXTURE0);
	t->bindTexture();
	glReadBuffer(GL_TEXTURE0);

	FILE * file = fopen(filepath.c_str(), "wb");

	GLubyte header[18] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	header[12] = (t->width)			& 0xFF;
	header[13] = (t->width >> 8)	& 0xFF;
	header[14] = (t->height)		& 0xFF;
	header[15] = (t->height >> 8)	& 0xFF;
	header[16] = t->bitdepth;
	
	fwrite(header, sizeof(GLubyte), 18, file);
	fseek(file, 18, 0);

	if (t->type == GL_UNSIGNED_BYTE) {
		GLubyte * imageData = new GLubyte[t->width * t->height * t->bitdepth / 8];
		glReadPixels(0, 0, t->width, t->height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		fwrite(imageData, sizeof(GLubyte), t->width * t->height * t->bitdepth/8, file);
	}
	if (t->type == GL_FLOAT) {
		GLfloat * imageData = new GLfloat[t->width * t->height * t->bitdepth / 8];
		glReadPixels(0, 0, t->width, t->height, GL_RGBA, GL_FLOAT, imageData);
		fwrite(imageData, sizeof(GLfloat), t->width * t->height * t->bitdepth / 8, file);
	}

	fclose(file);
}