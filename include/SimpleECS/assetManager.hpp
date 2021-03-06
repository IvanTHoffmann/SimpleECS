#pragma once

#include "SimpleECS/util.hpp"
#include "SimpleECS/decl.hpp"

enum Uniforms {
	U_MODEL,
	U_VIEW,
	U_PROJ,
	U_ASPECT,
	U_FRAME_POS,
	U_FRAME_SIZE,
	U_TEX,
	U_COLOR,
	U_TIME,
	U_CAM_POS,
	U_LERP,
	U_TILING,
	U_DIFFUSE_TEX,
	U_NORMAL_TEX,
	U_SPECULAR_TEX,
	U_COUNT
};

typedef struct {
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texCoord[2];
} Vertex;

#define ASSET_ACTIVE 0x1

typedef struct {
	u8 flags;
	GLuint id;
	GLint uniforms[U_COUNT]; // TODO: fix this so it doesnt rely on (what should be) user defined macros
} ShaderInfo;

typedef struct {
	u8 flags;
	GLuint fboId, rbo;
	GLuint w, h;
	u16 texIndex;
} FrameBufferInfo;

typedef struct {
	u8 flags;
	GLuint vao;
	uint16_t vertexCount;
} ModelInfo;

typedef struct {
	u8 flags;
	GLuint id, w, h;
} TextureInfo;

typedef struct {
	u8 flags;
	u16 texIndex;
	u16 charCount;
	size_t offset_loc; // address of u16 array
} FontInfo;

struct SoundInfo { // WAV file must have int16 sample size and 44100 sample rate to be played correctly
	u16 audioFormat;
	u16 numChannels;
	u32 sampleRate;
	u32 byteRate;
	u16 blockAlign;
	u16 bitsPerSample;

	u8 flags;
	u32 sampleCount;
	char format[4];
	u8 bytesPerSample;
	size_t data; // address of i16 array
};

#define ASSETS_PATH "assets\\"

class AssetManager {
private:
	Application* app;
	NameMap fboNames, shaderNames, modelNames, textureNames, fontNames, soundNames;

public:
	vector<FrameBufferInfo> fbos;
	vector<ShaderInfo> shaders;
	vector<ModelInfo> models;
	vector<TextureInfo> textures;
	vector<FontInfo> fonts;
	vector<SoundInfo> sounds;

	AssetManager(Application* app);
	~AssetManager();

	void setApp(Application* _app);

	u16 loadFbo(std::string name, u16 w, u16 h);

	void finishTexture(GLboolean linear);
	void loadTexture(TextureInfo* texInfo, std::string filename, GLboolean linear);

	void compileShader(ShaderInfo* shaderInfo, std::string vert, std::string frag);
	GLuint loadShader(std::string filename, const GLuint shaderType);

	void loadMODEL(ModelInfo* modelInfo, std::string filename);
	GLuint loadVao(Vertex* vertices, GLuint vertexCount);

	void loadFont(FontInfo* fontInfo, std::string fontName, GLboolean linear);

	void loadWAV(SoundInfo* soundInfo, std::string filename);

	u16 getFboIndex(std::string name);
	u16 getTextureIndex(std::string name, bool loadNew = true);
	u16 getModelIndex(std::string name, bool loadNew = true);
	u16 getFontIndex(std::string name, bool loadNew = true);
	u16 getShaderIndex(std::string vert, std::string frag, bool loadnew = true);
	u16 getShaderIndex(std::string name, bool loadNew = true);
	u16 getSoundIndex(std::string name, bool loadNew = true);

	FrameBufferInfo* getFbo(u16 index);
	FrameBufferInfo* getFbo(std::string name);
	TextureInfo* getTexture(u16 index);
	TextureInfo* getTexture(std::string name);
	ModelInfo* getModel(u16 index);
	ModelInfo* getModel(std::string name);
	FontInfo* getFont(u16 index);
	FontInfo* getFont(std::string name);
	ShaderInfo* getShader(u16 index);
	ShaderInfo* getShader(std::string vert, std::string frag);
	ShaderInfo* getShader(std::string name);
	SoundInfo* getSound(u16 index);
	SoundInfo* getSound(std::string name);

	vec3 getTextSize(std::string str, u16 fontIndex);
};