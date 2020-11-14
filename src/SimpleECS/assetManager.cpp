#include "SimpleECS/assetManager.hpp"


AssetManager::AssetManager() {
	bool finished = false;
	u16 i = 0;
	while (!finished) {
		finished = true;
		if (i < MAX_MODELS) {
			finished = false;
			models[i].flags = 0;
		}
		if (i < MAX_SHADERS) {
			finished = false;
			shaders[i].flags = 0;
		}
		if (i < MAX_FONTS) {
			finished = false;
			fonts[i].flags = 0;
		}
		if (i < MAX_TEXTURES) {
			finished = false;
			textures[i].flags = 0;
		}
		if (i < MAX_FBOS) {
			finished = false;
			fbos[i].flags = 0;
		}
		if (i < MAX_SOUNDS) {
			finished = false;
			sounds[i].flags = 0;
		}
		i++;
	}
}

AssetManager::~AssetManager() {
	for (u16 i = 0; i < MAX_FONTS; i++) {
		if (fonts[i].flags & ASSET_ACTIVE) {
			free(fonts[i].offsets);
		}
	}
	for (u16 i = 0; i < MAX_SOUNDS; i++) {
		if (sounds[i].flags & ASSET_ACTIVE) {
			free(sounds[i].data);
		}
	}
}

u16 AssetManager::loadFbo(std::string name, u16 w, u16 h) {
	u16 fboIndex = getFboIndex(name);
	if (fboIndex != INVALID_INDEX_16) {
		std::cout << "ERROR: Fbo \"" << name << "\" already exists\n";
		return INVALID_INDEX_16;
	}
	
	for (fboIndex = 0; fboIndex < MAX_FBOS; fboIndex++) {
		if (!(fbos[fboIndex].flags & ASSET_ACTIVE)) {
			break;
		}
	}

	if (fboIndex >= MAX_FBOS) {
		std::cout << "ERROR: Attempted to exceed the maximum number of fbos (name: \"" << name << "\")\n";
		return INVALID_INDEX_16;
	}

	u16 texIndex = getTextureIndex(name, false);
	if (texIndex == INVALID_INDEX_16) {
		return INVALID_INDEX_16;
	}

	fboNames.setIndex(name, fboIndex);
	FrameBufferInfo* fbo = fbos + fboIndex;
	fbo->flags |= ASSET_ACTIVE;
	fbo->w = w;
	fbo->h = h;
	fbo->texIndex = texIndex;

	textureNames.setIndex(name, texIndex);
	TextureInfo* tex = textures + fbo->texIndex;
	tex->flags |= ASSET_ACTIVE;
	tex->w = w;
	tex->h = h;

	glGenFramebuffers(1, &(fbo->fboId));
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);

	// generate texture
	glGenTextures(1, &(tex->id));
	glBindTexture(GL_TEXTURE_2D, tex->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->id, 0);

	glGenRenderbuffers(1, &(fbo->rbo));
	glBindRenderbuffer(GL_RENDERBUFFER, fbo->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo->rbo);

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
		printf("could not open %s\n", filename);
		return;
	}

	glGenTextures(1, &texInfo->id);
	uint32_t dataOffset;
	uint32_t headerSize;
	uint16_t bitsPerPixel;

	fseek(f, 10, SEEK_CUR);
	fread_s(&dataOffset, 4, 1, 4, f);
	fseek(f, 4, SEEK_CUR);
	fread_s(&(texInfo->w), 4, 1, 4, f);
	fread_s(&(texInfo->h), 4, 1, 4, f);
	fseek(f, 2, SEEK_CUR);
	fread_s(&bitsPerPixel, 2, 1, 2, f);
	fseek(f, 4, SEEK_CUR);

	uint32_t bytesPerPixel = (bitsPerPixel / 8);
	uint32_t dataSize = (texInfo->w * texInfo->h) * bytesPerPixel;
	GLubyte* data = (GLubyte*)malloc(dataSize);

	uint32_t rowSize = (bytesPerPixel * texInfo->w + 3) / 4 * 4;
	for (uint32_t row = 0; row < texInfo->h; row++) {
		fseek(f, dataOffset + rowSize * row, SEEK_SET);
		fread_s(data + texInfo->w * bytesPerPixel * row, dataSize, bytesPerPixel, texInfo->w, f);
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

	free(data);
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
}

GLuint AssetManager::loadShader(std::string filename, const GLuint shaderType) {
	const fpos_t fpos = 0;
	uint16_t fileSize;
	FILE* f;
	char* buffer;

	if (!fopen_s(&f, &filename[0], "r")) {
		fileSize = 0;
		while (getc(f) != EOF) {
			fileSize++;
		}

		buffer = (char*)calloc(fileSize + 1, sizeof(char));

		fsetpos(f, &fpos);
		fread_s(buffer, fileSize, sizeof(char), fileSize, f);
		buffer[fileSize] = '\0';

		//puts(buffer);

		fclose(f);
	}
	else {
		printf("ERROR: Failed to load %s\n", filename);
		return 0;
	}

	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &buffer, NULL);
	glCompileShader(shaderId);
	free(buffer);

	// ERROR CHECKING
	GLint isCompiled = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = (GLchar*)calloc(maxLength, sizeof(GLchar));
		glGetShaderInfoLog(shaderId, maxLength, &maxLength, errorLog);

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
		printf("ERROR: Could not load %s\n", filename);
	}

	// read vertex count
	fread_s(&modelInfo->vertexCount, sizeof(uint16_t), sizeof(uint16_t), 1, fp);
	Vertex* vertices = (Vertex*)calloc(modelInfo->vertexCount, sizeof(Vertex));

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
	free(vertices);
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
	fontInfo->offsets = (uint16_t*)calloc(fontInfo->charCount + 1, sizeof(uint16_t));
	fread_s(fontInfo->offsets, sizeof(uint16_t) * fontInfo->charCount, sizeof(uint16_t), fontInfo->charCount, f);
	fontInfo->offsets[fontInfo->charCount] = textures[fontInfo->texIndex].w; // this is an extra offset for calculating the width of the last symbol
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
		printf("ERROR: Could not open %s for reading\n", filename);
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
			if (!(soundInfo->data = (int16_t*)malloc(subChunkSize))) {
				puts("ERROR: malloc didnt work");
				return;
			}
			fread_s(soundInfo->data, subChunkSize, soundInfo->bytesPerSample, soundInfo->sampleCount, f);
			break;
		}
		else {
			fseek(f, subChunkSize, SEEK_CUR);
		}
	}
}

u16 AssetManager::getFboIndex(std::string name) {
	u16 index = fboNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}
	return INVALID_INDEX_16;
}

u16 AssetManager::getShaderIndex(std::string name, bool loadNew) {
	u16 index = shaderNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}

	for (index = 0; index < MAX_SHADERS; index++) {
		if (!(shaders[index].flags & ASSET_ACTIVE)) {
			if (loadNew) {
				shaderNames.setIndex(name, index);
				shaders[index].flags |= ASSET_ACTIVE;
				compileShader(shaders + index, ASSETS_PATH "shaders\\" + name + ".vert", ASSETS_PATH "shaders\\" + name + ".frag");
			}
			return index;
		}
	}

	std::cout << "ERROR: Attempted to exceed maximum number of shaders\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getModelIndex(std::string name, bool loadNew) {
	u16 index = modelNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}

	for (index = 0; index < MAX_MODELS; index++) {
		if (!(models[index].flags & ASSET_ACTIVE)) {
			if (loadNew) {
				modelNames.setIndex(name, index);
				models[index].flags |= ASSET_ACTIVE;
				loadMODEL(models + index, ASSETS_PATH "models\\" + name + ".MODEL");
			}
			return index;
		}
	}

	std::cout << "ERROR: Attempted to exceed maximum number of models\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getTextureIndex(std::string name, bool loadNew) {
	u16 index = textureNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}

	for (index = 0; index < MAX_TEXTURES; index++) {
		if (!(textures[index].flags & ASSET_ACTIVE)) {
			if (loadNew) {
				textureNames.setIndex(name, index);
				textures[index].flags |= ASSET_ACTIVE;
				loadTexture(textures + index, ASSETS_PATH "textures\\" + name + ".bmp", true);
			}
			return index;
		}
	}

	std::cout << "ERROR: Attempted to exceed maximum number of textures\n";
	return INVALID_INDEX_16;
} 

u16 AssetManager::getFontIndex(std::string name, bool loadNew) {
	u16 index = fontNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}

	for (index = 0; index < MAX_FONTS; index++) {
		if (!(fonts[index].flags & ASSET_ACTIVE)) {
			if (loadNew) {
				fontNames.setIndex(name, index);
				fonts[index].flags |= ASSET_ACTIVE;
				loadFont(fonts + index, name, true);
			}
			return index;
		}
	}

	std::cout << "ERROR: Attempted to exceed maximum number of fonts\n";
	return INVALID_INDEX_16;
}

u16 AssetManager::getSoundIndex(std::string name, bool loadNew) {
	u16 index = soundNames.getIndex(name);
	if (index != INVALID_INDEX_16) {
		return index;
	}

	for (index = 0; index < MAX_SOUNDS; index++) {
		if (!(sounds[index].flags & ASSET_ACTIVE)) {
			if (loadNew) {
				soundNames.setIndex(name, index);
				sounds[index].flags |= ASSET_ACTIVE;
				loadWAV(sounds + index, ASSETS_PATH "audio\\" + name + ".wav");
			}
			return index;
		}
	}

	std::cout << "ERROR: Attempted to exceed maximum number of sounds\n";
	return INVALID_INDEX_16;
}

FrameBufferInfo* AssetManager::getFbo(u16 index) { return fbos + index; }
FrameBufferInfo* AssetManager::getFbo(std::string name) { return fbos + getFboIndex(name); }
TextureInfo* AssetManager::getTexture(u16 index) { return textures + index; }
TextureInfo* AssetManager::getTexture(std::string name) { return textures + getTextureIndex(name); }
ModelInfo* AssetManager::getModel(u16 index) { return models + index; }
ModelInfo* AssetManager::getModel(std::string name) { return models + getModelIndex(name); }
FontInfo* AssetManager::getFont(u16 index) { return fonts + index; }
FontInfo* AssetManager::getFont(std::string name) { return fonts + getFontIndex(name); }
ShaderInfo* AssetManager::getShader(u16 index) { return shaders + index; }
ShaderInfo* AssetManager::getShader(std::string name) { return shaders + getShaderIndex(name); }
SoundInfo* AssetManager::getSound(u16 index) { return sounds + index; }
SoundInfo* AssetManager::getSound(std::string name) { return sounds + getSoundIndex(name); }


vec3 AssetManager::getTextSize(std::string str, u16 fontIndex) {
	FontInfo* font = fonts + fontIndex;
	TextureInfo* texture = textures + font->texIndex;

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
			width += (font->offsets[str[i] - 31] - font->offsets[str[i] - 32] - 1) / (float)texture->h;
		}
		i++;
	}
	if (width > size.x) { size.x = width; }
	return size;
}