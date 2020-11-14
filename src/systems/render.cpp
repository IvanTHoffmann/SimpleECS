#include "systems/render.hpp"
#include "appData.hpp"

#include "SimpleECS/application.hpp"
#include "SimpleECS/entity.hpp"

void initRenderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return;
	}

	glfwWindowHint(GLFW_DOUBLEBUFFER, appData->doubleBuf);
	appData->window = glfwCreateWindow(100, 100, "GLFWwindow", NULL, NULL);
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

	glClearColor(.35, .35, .5, 1);

	glActiveTexture(GL_TEXTURE0);
}

void updateRenderSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();

	Entity cam(app);
	Entity ent(app);

	mat4 matrix;

	FontInfo* font;
	ShaderInfo* shader;
	FrameBufferInfo* fbo;
	ModelInfo* model;

	cam.set(app->componentManager.getPrefabID("camera"));
	
	shader = app->assetManager.getShader("simple");
	//shader = app->assetManager.shaders + 0;

	glDisable(GL_BLEND);

	while (cam.next()) {
		cam.copyCamera();
		cam.copyTransform();

		fbo = app->assetManager.getFbo(cam.Camera->fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo->fboId);
		glViewport(0, 0, fbo->w, fbo->h);
		cam.Camera->aspect = (float)fbo->w / fbo->h;
		cam.syncCamera();
		glUseProgram(shader->id);
		setMatrix(shader->uniforms[U_PROJ], transpose(perspective(cam.Camera->fov, cam.Camera->aspect, 1.0f, 100.0f)));

		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT, GL_FILL);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);

		glUseProgram(shader->id);

		matrix = mat4_cast(cam.Transform->rot);
		matrix = translate(matrix, -cam.Transform->pos);

		setMatrix(shader->uniforms[U_VIEW], matrix);

		// DRAW MESHES
		glEnable(GL_DEPTH_TEST);

		ent.set();
		while (ent.next(MeshBit, GuiBit)) {
			ent.copyMesh();
			glBindVertexArray(app->assetManager.models[ent.Mesh->meshId].vao);
			glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[ent.Mesh->texId].id);

			glUniform2fv(shader->uniforms[U_TILING], 1, value_ptr(ent.Mesh->tiling));

			ent.copyTransform();
			matrix = mat4(1.0f);
			matrix = translate(matrix, ent.Transform->pos);
			matrix *= mat4_cast(ent.Transform->rot);
			matrix = scale(matrix, ent.Transform->scale);

			setMatrix(shader->uniforms[U_MODEL], matrix);
			glDrawArrays(GL_TRIANGLES, 0, app->assetManager.models[ent.Mesh->meshId].vertexCount);
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
	glViewport(0, 0, appData->inputInfo.windowW, appData->inputInfo.windowH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader->id);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_BLEND);
	//glDisable(GL_DEPTH_TEST);

	vec2 framePos = { 0, 0 };
	vec2 frameSize = { 1, 1 };

	mat4 shift = mat4(
		2, 0, 0, 0,
		0, 2, 0, 0,
		0, 0, 2, 0,
		-1, -1, -1, 1);

	char c;
	float charWidth;

	//* Draw Text
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	framePos = vec2(0, 0);
	frameSize = vec2(1, 1);

	ent.set(app->componentManager.getPrefabID("gui"));
	while (ent.next()) {
		ent.copyMesh();
		glBindVertexArray(app->assetManager.models[ent.Mesh->meshId].vao);
		glBindTexture(GL_TEXTURE_2D, app->assetManager.textures[ent.Mesh->texId].id);

		ent.copyGui();
		glUniform4fv(shader->uniforms[U_COLOR], 1, value_ptr(ent.Gui->color));

		ent.copyTransform();
		matrix = mat4(1.0f);
		if (ent.Gui->flags & COMP_GUI_PIXEL_POS) {
			matrix = translate(matrix, ent.Transform->pos / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}
		else {
			matrix = translate(matrix, ent.Transform->pos);
		}

		if (ent.Gui->flags & COMP_GUI_PIXEL_ANCHOR) {
			matrix = translate(matrix, -ent.Gui->anchor / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}

		if (ent.Gui->flags & COMP_GUI_PIXEL_SCALE) {
			matrix = scale(matrix, ent.Transform->scale / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}
		else {
			matrix = scale(matrix, ent.Transform->scale);
		}

		if (!(ent.Gui->flags & COMP_GUI_PIXEL_ANCHOR)) {
			matrix = translate(matrix, -ent.Gui->anchor);
		}

		glUniform2fv(shader->uniforms[U_FRAME_POS], 1, value_ptr(framePos));
		glUniform2fv(shader->uniforms[U_FRAME_SIZE], 1, value_ptr(frameSize));
		glUniformMatrix4fv(shader->uniforms[U_MODEL], 1, GL_FALSE, value_ptr(shift * matrix));
		glDrawArrays(GL_TRIANGLES, 0, app->assetManager.models[ent.Mesh->meshId].vertexCount);
	}


	//glEnable(GL_BLEND);
	//glBlendFuncSeparate(GL_SRC_ALPHA_SATURATE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_ADD);

	// DRAW TEXT
	
	ent.set(app->componentManager.getPrefabID("text"));
	model = app->assetManager.getModel("rect");
	glBindVertexArray(model->vao); // rectangle mesh

	TextureInfo* texture;

	while (ent.next()) {
		ent.copyText();
		font = app->assetManager.getFont(ent.Text->fontIndex);
		texture = app->assetManager.getTexture(font->texIndex);

		glBindTexture(GL_TEXTURE_2D, texture->id);

		ent.copyGui();
		glUniform4fv(shader->uniforms[U_COLOR], 1, value_ptr(ent.Gui->color));

		ent.copyTransform();
		matrix = mat4(1.0f);
		if (ent.Gui->flags & COMP_GUI_PIXEL_POS) {
			matrix = translate(matrix, ent.Transform->pos / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}
		else {
			matrix = translate(matrix, ent.Transform->pos);
		}

		if (ent.Gui->flags & COMP_GUI_PIXEL_ANCHOR) {
			matrix = translate(matrix, -ent.Gui->anchor / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}

		if (ent.Gui->flags & COMP_GUI_PIXEL_SCALE) {
			matrix = scale(matrix, ent.Transform->scale / vec3(appData->inputInfo.windowW, appData->inputInfo.windowH, 1));
		}
		else {
			matrix = scale(matrix, ent.Transform->scale);
		}

		if (!(ent.Gui->flags & COMP_GUI_PIXEL_ANCHOR)) {
			matrix = translate(matrix, -ent.Gui->anchor);
		}

		float aspect = appData->inputInfo.windowH / (float)appData->inputInfo.windowW;

		framePos.y = 0;
		frameSize.y = 1;
		vec3 bottomLeft = ent.Transform->pos - ent.Gui->anchor * ent.Transform->scale;
		vec2 rectSize = { 0, matrix[1][1] / ent.Text->lineCount };

		for (uint8_t i = 0; ent.Text->str[i] != '\0'; i++) {
			switch (ent.Text->str[i]) {
			case '\n':
				matrix[3][1] -= matrix[1][1];
				matrix[3][0] = bottomLeft.x;
				break;
			default:
				c = ent.Text->str[i] - 32;
				if (0 <= c && c < font->charCount) {
					framePos.x = font->offsets[c] / (float)texture->w;

					charWidth = font->offsets[c + 1] - font->offsets[c] - 1;
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