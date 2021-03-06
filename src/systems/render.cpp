#include "systems/render.hpp"
#include "appData.hpp"


void initRenderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return;
	}

	glfwWindowHint(GLFW_DOUBLEBUFFER, appData->doubleBuf);
	appData->window = glfwCreateWindow(500, 500, "GLFWwindow", NULL, NULL);
	if (!appData->window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(appData->window); 
	if (!appData->doubleBuf) {
		glfwSwapInterval(0);
	}

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LESS);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(.35f, .35f, .5f, 1.0f);

	glActiveTexture(GL_TEXTURE0);
}


void updateRenderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity* cam = appData->ent;
	Entity* ent = appData->ent2;

	mat4 matrix;

	FontInfo* font;
	ShaderInfo* shader;
	FrameBufferInfo* fbo;
	ModelInfo* model;

	cam->setPrefab("camera");
	
	shader = app->assetManager.getShader("simple", "simple");

	glDisable(GL_BLEND);

	while (cam->next()) {
		fbo = app->assetManager.getFbo(GET(cam, Camera)->fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
		glViewport(0, 0, fbo->w, fbo->h);
		GET(cam, Camera)->aspect = (float)fbo->w / fbo->h;
		glUseProgram(shader->id);
		setMatrix(shader->uniforms[U_PROJ], transpose(perspective(GET(cam, Camera)->fov, GET(cam, Camera)->aspect, 1.0f, 100.0f)));

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		glUseProgram(shader->id);

		matrix = mat4_cast(GET(cam, Transform)->rot);
		matrix = inverse(matrix);
		matrix = translate(matrix, -GET(cam, Transform)->pos);

		setMatrix(shader->uniforms[U_VIEW], matrix);

		// DRAW MESHES
		glEnable(GL_DEPTH_TEST);

		ent->setPrefab(0);
		while (ent->next(MeshBit | TransformBit, GuiBit)) {
			glBindVertexArray(app->assetManager.models[GET(ent, Mesh)->meshId].vao);
			glUniform1i(shader->uniforms[U_DIFFUSE_TEX], 0);
			glUniform1i(shader->uniforms[U_NORMAL_TEX], 1);
			glUniform1i(shader->uniforms[U_SPECULAR_TEX], 2);

			glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 2
			glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[GET(ent, Mesh)->diffuseId].id);

			if (GET(ent, Mesh)->normalId != (GLuint)(-1)) {
				glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 2
				glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[GET(ent, Mesh)->normalId].id);
			}
			if (GET(ent, Mesh)->specularId != (GLuint)(-1)) {
				glActiveTexture(GL_TEXTURE0 + 2); // Texture unit 2
				glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[GET(ent, Mesh)->specularId].id);
			}

			
			glUniform2fv(shader->uniforms[U_TILING], 1, value_ptr(GET(ent, Mesh)->tiling));

			matrix = mat4(GET(ent, Transform)->scale.x, 0, 0, 0,
						  0, GET(ent, Transform)->scale.y, 0, 0,
						  0, 0, GET(ent, Transform)->scale.z, 0,
						  GET(ent, Transform)->pos.x, GET(ent, Transform)->pos.y, GET(ent, Transform)->pos.z, 1) * mat4_cast(GET(ent, Transform)->rot);

			setMatrix(shader->uniforms[U_MODEL], matrix);
			glDrawArrays(GL_TRIANGLES, 0, app->assetManager.models[GET(ent, Mesh)->meshId].vertexCount);
		}
	}


	// TODO: find a better way to do double buffering

	TextureInfo *source, *target;
	source = app->assetManager.getTexture(app->assetManager.getFbo("fbo-default")->texIndex);
	target = app->assetManager.getTexture(app->assetManager.getFbo("fbo-copy")->texIndex);
	glBindTexture(GL_TEXTURE_2D, source->id);
	glCopyTextureSubImage2D(target->id, 0, 0, 0, 0, 0, target->w, target->h);

	// DRAW GUI
	shader = app->assetManager.getShader("ui");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, app->inputManager.windowW, app->inputManager.windowH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->id);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);

	mat4 shift = mat4(
		2, 0, 0, 0,
		0, 2, 0, 0,
		0, 0, 2, 0,
		-1, -1, -1, 1);

	//* Draw Text
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	vec2 framePos = { 0, 0 };
	vec2 frameSize = { 1, 1 };

	vec3 s;
	float percScale, percAnchor, percPos;
	vec3 invWinSize = 1.0f / vec3(app->inputManager.windowW, app->inputManager.windowH, 1);

	//
#define SET_GUI_MATRIX \
	percScale = (GET(ent, Gui)->flags & COMP_GUI_PIXEL_SCALE) == 0; \
	percAnchor = (GET(ent, Gui)->flags & COMP_GUI_PIXEL_ANCHOR) == 0; \
	percPos = (GET(ent, Gui)->flags & COMP_GUI_PIXEL_POS) == 0; \
	matrix = translate(mat4(1.0f), GET(ent, Transform)->pos * (percPos * vec3(1) - (percPos - 1) * invWinSize)); \
	s = GET(ent, Transform)->scale * (percScale * vec3(1) - (percScale - 1) * invWinSize); \
	matrix = translate(matrix, -GET(ent, Gui)->anchor * (percAnchor * s - (percAnchor - 1) * invWinSize)); \
	matrix = scale(matrix, s);
	//
	
	ent->setPrefab("gui");
	while (ent->next()) {
		glBindVertexArray(app->assetManager.models[GET(ent, Mesh)->meshId].vao);
		glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[GET(ent, Mesh)->diffuseId].id);

		glUniform4fv(shader->uniforms[U_COLOR], 1, value_ptr(GET(ent, Gui)->color));

		SET_GUI_MATRIX

		glUniform2fv(shader->uniforms[U_FRAME_POS], 1, value_ptr(framePos));
		glUniform2fv(shader->uniforms[U_FRAME_SIZE], 1, value_ptr(frameSize));
		glUniformMatrix4fv(shader->uniforms[U_MODEL], 1, GL_FALSE, value_ptr(shift * matrix));
		glDrawArrays(GL_TRIANGLES, 0, app->assetManager.models[GET(ent, Mesh)->meshId].vertexCount);
	}

	//glEnable(GL_BLEND);
	//glBlendFuncSeparate(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);

	// DRAW TEXT
	
	ent->setPrefab("text");
	model = app->assetManager.getModel("rect");
	glBindVertexArray(model->vao);

	TextureInfo* texture;

	u16* p;
	char c;
	float charWidth, aspect = app->inputManager.windowH / (float)app->inputManager.windowW;

	while (ent->next()) {
		font = app->assetManager.getFont(GET(ent, Text)->fontIndex);
		texture = app->assetManager.getTexture(font->texIndex);

		glBindTexture(GL_TEXTURE_2D, texture->id);

		glUniform4fv(shader->uniforms[U_COLOR], 1, value_ptr(GET(ent, Gui)->color));

		SET_GUI_MATRIX

		framePos.y = 0;
		frameSize.y = 1;
		vec3 bottomLeft = GET(ent, Transform)->pos - GET(ent, Gui)->anchor * GET(ent, Transform)->scale;
		vec2 rectSize = { 0, matrix[1][1] / GET(ent, Text)->lineCount };

		p = (u16*)(app->memoryManager.memStart + font->offset_loc);

		for (uint8_t i = 0; GET(ent, Text)->str[i] != '\0'; i++) {
			switch (GET(ent, Text)->str[i]) {
			case '\n':
				matrix[3][1] -= matrix[1][1];
				matrix[3][0] = bottomLeft.x;
				break;
			default:
				c = GET(ent, Text)->str[i] - 32;
				if (0 <= c && c < font->charCount) {
					framePos.x = p[c] / (float)texture->w;

					charWidth = p[c + 1] - p[c] - 1;
					frameSize.x = charWidth / texture->w;

					rectSize.x = rectSize.y * charWidth / texture->h;
					matrix[0][0] = rectSize.x * aspect;

					glUniform2fv(shader->uniforms[U_FRAME_POS], 1, value_ptr(framePos));
					glUniform2fv(shader->uniforms[U_FRAME_SIZE], 1, value_ptr(frameSize));
					glUniformMatrix4fv(shader->uniforms[U_MODEL], 1, GL_FALSE, value_ptr(shift * matrix));
					glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);
					matrix[3][0] += matrix[0][0];
				}
				break;
			}
		}
	}

	if (!appData->doubleBuf) {
		glFlush();
	}
	glfwSwapBuffers(appData->window);
}