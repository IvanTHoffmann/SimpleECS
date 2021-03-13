#include "SimpleECS/simpleECS.hpp"


AssetManager::AssetManager() {
	app = nullptr;
}

AssetManager::~AssetManager() {}


void AssetManager::setApp(Application* _app) {
	app = _app;
}

u16 AssetManager::loadFbo(std::string name, u16 w, u16 h) {
	cout << "loadFbo(" << name << ")\n";
	u16 fboIndex = getFboIndex(name);
	if (fboIndex != INVALID_INDEX_16) {
		std::cout << "ERROR: Fbo \"" << name << "\" already exists\n";
		return INVALID_INDEX_16; // return fboIndex;
	}
	
	fboIndex = fbos.size();
	FrameBufferInfo fbo;
	fbos.push_back(fbo);

	u16 texIndex = textures.size();
	TextureInfo texture;
	textures.push_back(texture);

	fboNames.add(name, fboIndex);
	fbos[fboIndex].flags |= ASSET_ACTIVE;
	fbos[fboIndex].w = w;
	fbos[fboIndex].h = h;
	fbos[fboIndex].texIndex = texIndex;

	textureNames.add(name, texIndex);
	textures[fbos[fboIndex].texIndex].flags |= ASSET_ACTIVE;
	textures[fbos[fboIndex].texIndex].w = w;
	textures[fbos[fboIndex].texIndex].h = h;

	glGenFramebuffers(1, &(fbos[fboIndex].fboId));
	glBindFramebuffer(GL_FRAMEBUFFER, fbos[fboIndex].fboId);

	// generate texture
	glGenTextures(1, &(textures[fbos[fboIndex].texIndex].id));
	glBindTexture(GL_TEXTURE_2D, textures[fbos[fboIndex].texIndex].id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[fbos[fboIndex].texIndex].id, 0);

	glGenRenderbuffers(1, &(fbos[fboIndex].rbo));
	glBindRenderbuffer(GL_RENDERBUFFER, fbos[fboIndex].rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbos[fboIndex].rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fboIndex;
}

void AssetManager::finishTexture(GLboolean linear) {
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (linear) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
}

void AssetManager::loadTexture(TextureInfo* texInfo, std::string filename, GLboolean linear) {
	FILE* f;
	if (fopen_s(&f, &filename[0], "rb")) {
		cout << "could not open " << filename << "\n";
		return;
	}

	glGenTextures(1, &texInfo->id);
	u32 dataOffset;
	u16 bitsPerPixel;

	fseek(f, 10, SEEK_CUR);
	fread_s(&dataOffset, 4, 1, 4, f);
	fseek(f, 4, SEEK_CUR);
	fread_s(&(texInfo->w), 4, 1, 4, f);
	fread_s(&(texInfo->h), 4, 1, 4, f);
	fseek(f, 2, SEEK_CUR);
	fread_s(&bitsPerPixel, 2, 1, 2, f);
	fseek(f, 4, SEEK_CUR);

	size_t bytesPerPixel = (bitsPerPixel / 8);
	size_t dataSize = bytesPerPixel * ((size_t)texInfo->w * texInfo->h);
	GLubyte* data = (GLubyte*)app->memoryManager.getVolatileBlock(dataSize);
	//GLubyte* data = (GLubyte*)malloc(dataSize);

	size_t rowSize = (bytesPerPixel * texInfo->w + 3) / 4 * 4;
	for (size_t row = 0; row < texInfo->h; row++) {
		fseek(f, dataOffset + rowSize * row, SEEK_SET);
		fread_s(data + bytesPerPixel * row * texInfo->w, dataSize, bytesPerPixel, texInfo->w, f);
	}

	fclose(f);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // RESEARCH WHAT THIS ACTUALLY DOES
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texInfo->id);

	// arg[1] specifies the mipmap level
	// arg[5] "Specifies the width of the border. Must be 0."

	if (bitsPerPixel == 32) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texInfo->w, texInfo->h, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
	}
	else if (bitsPerPixel == 24) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texInfo->w, texInfo->h, 0,
			GL_BGR, GL_UNSIGNED_BYTE, data);
	}
	else if (bitsPerPixel == 8) {
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texInfo->w, texInfo->h, 0,
		//	GL_RED, GL_UNSIGNED_BYTE, texInfo->data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, texInfo->w, texInfo->h, 0,
			GL_ALPHA, GL_UNSIGNED_BYTE, data);
	}

	finishTexture(linear);

	//free((u8*)data);
}

void AssetManager::compileShader(ShaderInfo* shaderInfo, std::string vert, std::string frag) {
	shaderInfo->id = glCreateProgram();
	glAttachShader(shaderInfo->id, loadShader(vert, GL_VERTEX_SHADER));
	glAttachShader(shaderInfo->id, loadShader(frag, GL_FRAGMENT_SHADER));
	glLinkProgram(shaderInfo->id);
	glUseProgram(shaderInfo->id);

	shaderInfo->uniforms[U_MODEL] = glGetUniformLocation(shaderInfo->id, "model");
	shaderInfo->uniforms[U_VIEW] = glGetUniformLocation(shaderInfo->id, "view");
	shaderInfo->uniforms[U_PROJ] = glGetUniformLocation(shaderInfo->id, "proj");
	shaderInfo->uniforms[U_ASPECT] = glGetUniformLocation(shaderInfo->id, "aspect");
	shaderInfo->uniforms[U_FRAME_POS] = glGetUniformLocation(shaderInfo->id, "framePos");
	shaderInfo->uniforms[U_FRAME_SIZE] = glGetUniformLocation(shaderInfo->id, "frameSize");
	shaderInfo->uniforms[U_TEX] = glGetUniformLocation(shaderInfo->id, "tex");
	shaderInfo->uniforms[U_COLOR] = glGetUniformLocation(shaderInfo->id, "color");
	shaderInfo->uniforms[U_TIME] = glGetUniformLocation(shaderInfo->id, "time");
	shaderInfo->uniforms[U_CAM_POS] = glGetUniformLocation(shaderInfo->id, "camPos");
	shaderInfo->uniforms[U_LERP] = glGetUniformLocation(shaderInfo->id, "lerp");
	shaderInfo->uniforms[U_TILING] = glGetUniformLocation(shaderInfo->id, "tiling");
	shaderInfo->uniforms[U_DIFFUSE_TEX] = glGetUniformLocation(shaderInfo->id, "diffuseTex");
	shaderInfo->uniforms[U_NORMAL_TEX] = glGetUniformLocation(shaderInfo->id, "normalTex");
	shaderInfo->uniforms[U_SPECULAR_TEX] = glGetUniformLocation(shaderInfo->id, "specularTex");
}

GLuint AssetManager::loadShader(std::string filename, const GLuint shaderType) {
	const fpos_t fpos = 0;
	size_t fileSize;
	FILE* f;
	char* buffer;

	if (!fopen_s(&f, &filename[0], "r")) {
		fileSize = 0;
		while (getc(f) != EOF) {
			fileSize++;
		}

		buffer = (char*)app->memoryManager.getVolatileBlock((fileSize + 1) * sizeof(char));
		//buffer = (char*)malloc((fileSize + 1) * sizeof(char));

		fsetpos(f, &fpos);
		fread_s(buffer, fileSize, sizeof(char), fileSize, f);
		buffer[fileSize] = '\0';

		//puts(buffer);

		fclose(f);
	}
	else {
		std::cout << "ERROR: Failed to load shader file \"" << filename << "\"\n";
		return 0;
	}

	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &buffer, NULL);
	glCompileShader(shaderId);
	//free((u8*)buffer);

	// ERROR CHECKING
	GLint isCompiled = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = (GLchar*)malloc(maxLength * sizeof(GLchar));
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, errorLog);
		free(errorLog);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shaderId); // Don't leak the shader.
		return 0;
	}

	return shaderId;
}

void AssetManager::loadMODEL(ModelInfo* modelInfo, std::string filename) {
	FILE* fp;
	if (fopen_s(&fp, &filename[0], "rb")) {
		std::cout << "ERROR: Could not load " << filename << "\n";
	}

	// read vertex count
	fread_s(&modelInfo->vertexCount, sizeof(uint16_t), sizeof(uint16_t), 1, fp);
	Vertex* vertices = (Vertex*)app->memoryManager.getVolatileBlock(modelInfo->vertexCount * sizeof(Vertex));
	//Vertex* vertices = (Vertex*)malloc(modelInfo->vertexCount * sizeof(Vertex));

	uint16_t vectorCount, refIndex;
	float vectorData[3];
	uint8_t refCount;
	for (uint8_t attrIndex = 0; attrIndex < 3; attrIndex++) {
		fread_s(&vectorCount, sizeof(uint16_t), sizeof(uint16_t), 1, fp);
		for (uint16_t vi = 0; vi < vectorCount; vi++) {
			uint8_t axisCount = 3 - (attrIndex == 1);
			fread_s(vectorData, sizeof(float) * 3, sizeof(float), axisCount, fp);
			fread_s(&refCount, sizeof(uint8_t), sizeof(uint8_t), 1, fp);
			for (uint8_t ri = 0; ri < refCount; ri++) {
				fread_s(&refIndex, sizeof(uint16_t), sizeof(uint16_t), 1, fp);
				switch (attrIndex) {
				case 0:
					vertices[refIndex].position[0] = vectorData[0];
					vertices[refIndex].position[1] = vectorData[1];
					vertices[refIndex].position[2] = vectorData[2];
					break;
				case 1:
					vertices[refIndex].texCoord[0] = vectorData[0];
					vertices[refIndex].texCoord[1] = vectorData[1];
					break;
				case 2:
					vertices[refIndex].normal[0] = vectorData[0];
					vertices[refIndex].normal[1] = vectorData[1];
					vertices[refIndex].normal[2] = vectorData[2];
					break;
				}
			}
		}
	}
	fclose(fp);

	modelInfo->vao = loadVao(vertices, modelInfo->vertexCount);
	//free((u8*)vertices);
}

GLuint AssetManager::loadVao(Vertex* vertices, GLuint vertexCount) {
	GLuint vao, vertexBuffer;// , indexBuffer;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// LOAD INDEX BUFFER
	/*glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint),
				 indices, GL_STATIC_DRAW);*/

				 // LOAD VERTEX BUFFER
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex),
		vertices, GL_STATIC_DRAW);

	GLuint bindingIndex = 0;
	glBindVertexBuffer(bindingIndex, vertexBuffer, 0, sizeof(Vertex)); // 0 is offset

	// POSITIONS
	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexAttribBinding(0, bindingIndex);

	// NORMALS
	glEnableVertexAttribArray(1);
	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexAttribBinding(1, bindingIndex);

	// TEXCOORDS
	glEnableVertexAttribArray(2);
	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));
	glVertexAttribBinding(2, bindingIndex);

	// UNBIND VAO
	glBindVertexArray(0);

	return vao;
}

void AssetManager::loadFont(FontInfo* fontInfo, std::string fontName, GLboolean linear) {
	fontInfo->texIndex = getTextureIndex(fontName);

	FILE* f;
	fopen_s(&f, &(ASSETS_PATH "fonts\\" + fontName + ".offs")[0], "rb");
	fread_s(&(fontInfo->charCount), sizeof(uint16_t), sizeof(uint16_t), 1, f);
	size_t arraySize = ((size_t)fontInfo->charCount + 1) * sizeof(u16);
	fontInfo->offset_loc = app->memoryManager.getHead();
	u16* p = (u16*)app->memoryManager.getBlock(arraySize);
	//fontInfo->offsets = (u16*)malloc(arraySize);
	fread_s(p, arraySize, sizeof(u16), fontInfo->charCount, f);
	p[fontInfo->charCount] = textures[fontInfo->texIndex].w; // this is an extra offset for calculating the width of the last symbol
	fclose(f);
}

bool strCmp4(char* a, char* b) {
	for (u8 i = 0; i < 4; i++) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

/*
void printRIFF(SoundInfo* r) {
	printf("sampleCount: %d\n", r->sampleCount);
	printf("audioFormat: %d\n", r->audioFormat);
	printf("numChannels: %d\n", r->numChannels);
	printf("sampleRate: %d\n", r->sampleRate);
	printf("byteRate: %d\n", r->byteRate);
	printf("blockAlign: %d\n", r->blockAlign);
	printf("bitsPerSample: %d\n", r->bitsPerSample);

	uint16_t secs = r->sampleCount / r->sampleRate / r->numChannels;
	printf("duration: %dm:%ds\n", secs / 60, secs % 60);
}
*/

void AssetManager::loadWAV(SoundInfo* soundInfo, std::string filename) {
	FILE* f;
	if (fopen_s(&f, &filename[0], "rb")) {
		std::cout << "ERROR: Could not open " << filename << " for reading\n";
		return;
	}

	uint32_t bytesRead = 0, mainChunkSize, subChunkSize;
	fseek(f, 4, SEEK_CUR);
	fread_s(&mainChunkSize, 4, 1, 4, f);
	fread_s(&soundInfo->format, 4, 1, 4, f);
	bytesRead += 4; // riff format

	char chunkName[4];
	while (bytesRead < mainChunkSize) {
		fread_s(chunkName, 4, 1, 4, f);
		fread_s(&subChunkSize, 4, 1, 4, f);
		bytesRead += subChunkSize + 8;

		if (bytesRead > mainChunkSize) {
			puts("ERROR: exceeded file size");
			break;
		}

		if (strCmp4(chunkName, "fmt ")) {
			fread_s(soundInfo, 16, 1, 16, f);
			soundInfo->bytesPerSample = soundInfo->bitsPerSample / 8;
		}
		else if (strCmp4(chunkName, "data")) {
			soundInfo->sampleCount = subChunkSize / soundInfo->bytesPerSample;
			soundInfo->data = app->memoryManager.getHead();
			i16* p = (i16*)app->memoryManager.getBlock(subChunkSize);
			//soundInfo->data = (i16*)malloc(subChunkSize);
			fread_s(p, subChunkSize, soundInfo->bytesPerSample, soundInfo->sampleCount, f);
			break;
		}
		else {
			fseek(f, subChunkSize, SEEK_CUR);
		}
	}
}

u16 AssetManager::getFboIndex(std::string name) {
	u16 index;
	if (fboNames.getIndex(&index, name)) {
		return index;
	}
	return INVALID_INDEX_16;
}

u16 AssetManager::getShaderIndex(std::string vert, std::string frag, bool loadNew) {
	std::string name = vert + "/" + frag;

	u16 index;
	if (shaderNames.getIndex(&index, name)) {
		return index;
	}
	else if (loadNew) {
		ShaderInfo shader;
		compileShader(&shader, ASSETS_PATH "shaders\\" + vert + ".vert", ASSETS_PATH "shaders\\" + frag + ".frag");
		index = shaders.size();
		shaderNames.add(name, index);
		shaders.push_back(shader);
		return index;
	}
	cout << "Couldnt get shader index\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getShaderIndex(std::string name, bool loadNew) {
	return getShaderIndex(name, name, loadNew);
}

u16 AssetManager::getModelIndex(std::string name, bool loadNew) {
	u16 index;
	if (modelNames.getIndex(&index, name)) {
		return index;
	}
	else if (loadNew) {
		ModelInfo model;
		index = models.size();
		loadMODEL(&model, ASSETS_PATH "models\\" + name + ".MODEL");
		modelNames.add(name, index);
		models.push_back(model);
		return index;
	}
	cout << "Couldnt get model index\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getTextureIndex(std::string name, bool loadNew) {
	u16 index = textures.size();
	if (textureNames.getIndex(&index, name)) {
		return index;
	}
	else if (loadNew) {
		TextureInfo texture;
		loadTexture(&texture, ASSETS_PATH "textures\\" + name + ".bmp", true); // TODO: check for errors
		textureNames.add(name, index);
		textures.push_back(texture);
		return index;
	}
	else {
		return index;
	}
	cout << "Couldnt get texture index for {" << name << "}\n";
	return INVALID_INDEX_16;
} 

u16 AssetManager::getFontIndex(std::string name, bool loadNew) {
	u16 index;
	if (fontNames.getIndex(&index, name)) {
		return index;
	}
	else if (loadNew) {
		FontInfo font;
		index = fonts.size();
		fontNames.add(name, index);
		loadFont(&font, name, true);
		fonts.push_back(font);
		return index;
	}
	cout << "Couldnt get font index\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getSoundIndex(std::string name, bool loadNew) {
	u16 index;
	if (soundNames.getIndex(&index, name)) {
		return index;
	}
	else if (loadNew) {
		SoundInfo sound;
		index = sounds.size();
		soundNames.add(name, index);
		loadWAV(&sound, ASSETS_PATH "audio\\" + name + ".wav");
		sounds.push_back(sound);
		return index;
	}
	cout << "Couldnt get sound index\n";
	return INVALID_INDEX_16;
}

// if the index returned from a string function is invalid, the program will crash :|
FrameBufferInfo* AssetManager::getFbo(u16 index) { return &fbos[index]; }
FrameBufferInfo* AssetManager::getFbo(std::string name) { return &fbos[getFboIndex(name)]; }
TextureInfo* AssetManager::getTexture(u16 index) { return &textures[index]; }
TextureInfo* AssetManager::getTexture(std::string name) { return &textures[getTextureIndex(name)]; }
ModelInfo* AssetManager::getModel(u16 index) { return &models[index]; }
ModelInfo* AssetManager::getModel(std::string name) { return &models[getModelIndex(name)]; }
FontInfo* AssetManager::getFont(u16 index) { return &fonts[index]; }
FontInfo* AssetManager::getFont(std::string name) { return &fonts[getFontIndex(name)]; }
ShaderInfo* AssetManager::getShader(u16 index) { return &shaders[index]; }
ShaderInfo* AssetManager::getShader(std::string vert, std::string frag) { return &shaders[getShaderIndex(vert, frag)]; }
ShaderInfo* AssetManager::getShader(std::string name) { return &shaders[getShaderIndex(name)]; }
SoundInfo* AssetManager::getSound(u16 index) { return &sounds[index]; }
SoundInfo* AssetManager::getSound(std::string name) { return &sounds[getSoundIndex(name)]; }


vec3 AssetManager::getTextSize(std::string str, u16 fontIndex) {
	FontInfo* font = &fonts[fontIndex];
	TextureInfo* texture = &textures[font->texIndex];

	u16* p = (u16*)(app->memoryManager.memStart + font->offset_loc);

	float width = 0;
	vec3 size = vec3(0, 1, 0);
	u8 i = 0;
	while (str[i] != '\0') {
		if (str[i] == '\n') {
			if (width > size.x) { size.x = width; }
			width = 0;
			size.y++;
		}
		else {
			width += (p[str[i] - 31] - p[str[i] - 32] - 1) / (float)texture->h;
		}
		i++;
	}
	if (width > size.x) { size.x = width; }
	return size;
}