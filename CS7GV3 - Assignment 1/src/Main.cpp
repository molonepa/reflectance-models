#define GLM_SWIZZLE

#include "WindowManager.h"
#include "EventHandler.h"
#include "Clock.h"
#include "Camera.h"
#include "Transform.h"
#include "Mesh.h."
#include "Shader.h"

#define PI 3.14159265359

int main(int argc, char* args[]) {
	// initialise window, event handler, clock, camera
	WindowManager wm("SDL OpenGL Application", 1600, 900);
	EventHandler input;
	Clock clock;
	Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
	//input.addMouseBinding(std::bind(&Camera::Look, &camera, std::placeholders::_1, std::placeholders::_2));
	//input.addKeyBinding(SDL_SCANCODE_W, std::bind(&Camera::MoveForwards, &camera));
	//input.addKeyBinding(SDL_SCANCODE_S, std::bind(&Camera::MoveBackwards, &camera));
	//input.addKeyBinding(SDL_SCANCODE_D, std::bind(&Camera::MoveRight, &camera));
	//input.addKeyBinding(SDL_SCANCODE_A, std::bind(&Camera::MoveLeft, &camera));

	// load and compile shader programs
	Shader lightShader("src/shaders/light");
	Shader blinnPhongShader = Shader("src/shaders/blinn-phong");
	Shader toonShader = Shader("src/shaders/toon");
	Shader cookTorranceShader = Shader("src/shaders/cook-torrance");

	// initialise transforms for light and teapot objects
	Transform lightTransform(glm::vec3(3.0f, 2.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.5f));
	Transform teapotTransform;

	// initialise meshes for light and teapot objects
	Mesh lightMesh("assets/sphere.obj");
	Mesh teapotMesh = Mesh("assets/teapot.obj");

	// initialise uniform variables
	glm::vec3 lightColour = glm::vec3(1.0f);
	float ambientStrength = 0.2f;

	float blinnPhongShininess = 128.0f;
	float blinnPhongSpecular = 256.0f;
	glm::vec3 blinnPhongBaseColour = glm::vec3(1.0f, 1.0f, 0.5f);

	glm::vec3 toonBaseColour = glm::vec3(1.0f, 1.0f, 0.5f);
	float toonSpecularThreshold = 0.95f;
	float toonBandThreshold = 0.5f;
	float toonShadowThreshold = 0.25f;

	float cookTorranceMetallic = 0.5f;
	float cookTorranceRoughness = 0.4f;
	glm::vec3 cookTorranceAlbedo = glm::vec3(1.0f, 1.0f, 0.5f);

	// initialise functionality for changing lighting model dynamically
	enum LightingModels { Blinn_Phong, Toon, Cook_Torrance };
	const char* lightingModelText[] = { "Blinn-Phong", "Toon", "Cook-Torrance" };
	int currentLightingModel = 0;

	while (wm.IsRunning()) {
		//handle user input
		input.handleKeyboardInput();
		input.handleMouseInput();

		// clear window with grey colour
		wm.Clear(0.5f, 0.5f, 0.5f, 1.0f);

		// update light position
		float xPos = sin(0.0000001 * clock.GetElapsedTime()) * 3.0f;
		float zPos = cos(0.0000001 * clock.GetElapsedTime()) * 3.0f;
		lightTransform.SetPosition(glm::vec3(xPos, 2.0f, zPos));

		// draw light source
		lightShader.Bind();

		lightShader.UpdateUniform("u_ModelMatrix", lightTransform.GetModelMatrix());
		lightShader.UpdateUniform("u_ViewMatrix", camera.GetViewMatrix());
		lightShader.UpdateUniform("u_ProjectionMatrix", camera.GetProjectionMatrix());
		lightShader.UpdateUniform("u_LightColour", lightColour);

		lightMesh.Render();

		// get light position in world space for use in lighting calculations
		glm::vec3 lightPosition_WorldSpace = (glm::vec4(lightTransform.GetPosition(), 1.0f) * lightTransform.GetModelMatrix()).xyz;

		if (currentLightingModel == Blinn_Phong) {
			blinnPhongShader.Bind();

			// vertex shader uniforms
			blinnPhongShader.UpdateUniform("u_ModelMatrix", teapotTransform.GetModelMatrix());
			blinnPhongShader.UpdateUniform("u_ViewMatrix", camera.GetViewMatrix());
			blinnPhongShader.UpdateUniform("u_ProjectionMatrix", camera.GetProjectionMatrix());

			// fragment shader uniforms
			blinnPhongShader.UpdateUniform("u_BaseColour", blinnPhongBaseColour);
			blinnPhongShader.UpdateUniform("u_LightColour", lightColour);
			blinnPhongShader.UpdateUniform("u_LightPosition", lightPosition_WorldSpace);
			blinnPhongShader.UpdateUniform("u_CameraPosition", camera.GetPosition());
			blinnPhongShader.UpdateUniform("u_AmbientStrength", ambientStrength);
			blinnPhongShader.UpdateUniform("u_Shininess", blinnPhongShininess);
			blinnPhongShader.UpdateUniform("u_SpecularReflectivity", blinnPhongSpecular);

			teapotMesh.Render();
		}
		else if (currentLightingModel == Toon) {
			toonShader.Bind();

			// vertex shader uniforms
			toonShader.UpdateUniform("u_ModelMatrix", teapotTransform.GetModelMatrix());
			toonShader.UpdateUniform("u_ViewMatrix", camera.GetViewMatrix());
			toonShader.UpdateUniform("u_ProjectionMatrix", camera.GetProjectionMatrix());

			// fragment shader uniforms
			toonShader.UpdateUniform("u_BaseColour", toonBaseColour);
			toonShader.UpdateUniform("u_SpecularThreshold", toonSpecularThreshold);
			toonShader.UpdateUniform("u_BandThreshold", toonBandThreshold);
			toonShader.UpdateUniform("u_ShadowThreshold", toonShadowThreshold);
			toonShader.UpdateUniform("u_LightColour", lightColour);
			toonShader.UpdateUniform("u_LightPosition", lightPosition_WorldSpace);
			toonShader.UpdateUniform("u_AmbientStrength", ambientStrength);

			teapotMesh.Render();
		}
		else if (currentLightingModel == Cook_Torrance) {
			cookTorranceShader.Bind();

			// vertex shader uniforms
			cookTorranceShader.UpdateUniform("u_ModelMatrix", teapotTransform.GetModelMatrix());
			cookTorranceShader.UpdateUniform("u_ViewMatrix", camera.GetViewMatrix());
			cookTorranceShader.UpdateUniform("u_ProjectionMatrix", camera.GetProjectionMatrix());

			// fragment shader uniforms
			cookTorranceShader.UpdateUniform("u_Albedo", cookTorranceAlbedo);
			cookTorranceShader.UpdateUniform("u_Metallic", cookTorranceMetallic);
			cookTorranceShader.UpdateUniform("u_Roughness", cookTorranceRoughness);
			cookTorranceShader.UpdateUniform("u_LightColour", lightColour);
			cookTorranceShader.UpdateUniform("u_LightPosition", lightPosition_WorldSpace);
			cookTorranceShader.UpdateUniform("u_CameraPosition", camera.GetPosition());
			cookTorranceShader.UpdateUniform("u_AmbientStrength", ambientStrength);

			teapotMesh.Render();
		}

		// get current teapot position, rotation, scale
		glm::vec3 P = teapotTransform.GetPosition();
		glm::vec3 R = teapotTransform.GetRotation();
		glm::vec3 S = teapotTransform.GetScale();

		// draw teapot transform UI
		ImGui::Begin("Teapot Properties");
		ImGui::SliderFloat3("Position", (float*)&P, -5.0f, 5.0f);
			ImGui::SliderFloat3("Orientation", (float*)&R, 0.0f, (float)2*PI);
			ImGui::SliderFloat3("Scale", (float*)&S, 1.0f, 10.0f);
		ImGui::End();

		// update teapot transform with new values
		teapotTransform.SetPosition(P);
		teapotTransform.SetRotation(R);
		teapotTransform.SetScale(S);

		// draw light property UI
		ImGui::Begin("Light Properties");
			ImGui::Combo("Lighting Model", &currentLightingModel, lightingModelText, IM_ARRAYSIZE(lightingModelText));
			ImGui::ColorPicker3("Light colour", (float*)&lightColour);
			ImGui::SliderFloat("Ambient light", &ambientStrength, 0.0f, 1.0f);
		ImGui::End();

		// draw material property UI
		ImGui::Begin("Material Properties");
			if (currentLightingModel == Blinn_Phong) {
				ImGui::ColorPicker3("Base colour", (float*)&blinnPhongBaseColour);
				ImGui::SliderFloat("Shininess", &blinnPhongShininess, 1.0f, 256.0f);
				ImGui::SliderFloat("Reflectivity", &blinnPhongSpecular, 1.0f, 512.0f);
			}
			else if (currentLightingModel == Toon) {
				ImGui::ColorPicker3("Base colour", (float*)&toonBaseColour);
				ImGui::SliderFloat("Specular highlight threshold", &toonSpecularThreshold, 0.0f, 1.0f);
				ImGui::SliderFloat("Band threshold", &toonBandThreshold, 0.0f, 1.0f);
				ImGui::SliderFloat("Shadow threshold", &toonShadowThreshold, 0.0f, 1.0f);
			}
			else if (currentLightingModel == Cook_Torrance) {
				ImGui::ColorPicker3("Albedo", (float*)&cookTorranceAlbedo);
				ImGui::SliderFloat("Metallic", &cookTorranceMetallic, 0.0f, 1.0f);
				ImGui::SliderFloat("Roughness", &cookTorranceRoughness, 0.001f, 1.0f);
			}
		ImGui::End();

		wm.Update();

		clock.Tick();
	}

	return 0;
}