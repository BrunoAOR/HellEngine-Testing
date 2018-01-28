#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> /* Has to be included AFTER Windows.h */
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

ModuleImGui::ModuleImGui()
{
	licenseString = std::string("")
		+ "MIT License\n"
		+ "\n"
		+ "Copyright(c) 2017 Marc Campins, Bruno Ortiz, Sergi Ramirez, Daniel Reyes, Mario Rodriguez\n"
		+ "\n"
		+ "Permission is hereby granted, free of charge, to any person obtaining a copy "
		+ "of this software and associated documentation files(the \"Software\"), to deal "
		+ "in the Software without restriction, including without limitation the rights "
		+ "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
		+ "copies of the Software, and to permit persons to whom the Software is "
		+ "furnished to do so, subject to the following conditions:\n"
		+ "\n"
		+ "The above copyright notice and this permission notice shall be included in all "
		+ "copies or substantial portions of the Software.\n"
		+ "\n"
		+ "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
		+ "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
		+ "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE "
		+ "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
		+ "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
		+ "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
		+ "SOFTWARE.";
}

/* Destructor */
ModuleImGui::~ModuleImGui()
{
}

/* Called before render is available */
bool ModuleImGui::Init()
{
	// Setup ImGui binding
	ImGui_ImplSdlGL3_Init(App->window->window);

	// Setup style
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsDark();

	return true;
}

/* Called each loop iteration */
UpdateStatus ModuleImGui::PreUpdate()
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called each loop iteration */
UpdateStatus ModuleImGui::Update()
{
	static bool shouldQuit = false;
	static bool showDemoWindow = false;
	static bool showAbout = false;
	static bool showCameraWindow = false;
	static bool showTexturesWindow = false;
	static bool showOpenGLWindow = false;

	static bool rendererWireFrame = false;
	static bool rendererRotate = false;

	float mainMenuBarHeight;

	if (ImGui::BeginMainMenuBar())
	{
		mainMenuBarHeight = ImGui::GetWindowHeight();
		if (ImGui::BeginMenu("Main"))
		{
			ImGui::MenuItem("Show demo window", nullptr, &showDemoWindow);
			ImGui::Separator();

			ImGui::MenuItem("Quit", nullptr, &shouldQuit);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Configuration"))
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				if (ImGui::MenuItem("Wireframe", nullptr, &rendererWireFrame))
				{
					App->renderer->wireframe = !App->renderer->wireframe;
				}
				if (ImGui::MenuItem("Rotate", nullptr, &rendererRotate))
				{
					App->renderer->shouldRotate = !App->renderer->shouldRotate;
				}
				ImGui::EndMenu();
			}

			ImGui::MenuItem("Camera", nullptr, &showCameraWindow);
			ImGui::MenuItem("Textures", nullptr, &showTexturesWindow);
			ImGui::MenuItem("OpenGL", nullptr, &showOpenGLWindow);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About HellEngine", nullptr, &showAbout);

			ImGui::MenuItem("Yes, HELP US...", nullptr, false, false);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("I am a helpful tooltip!");
				ImGui::EndTooltip();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	if (showAbout)
	{
		ShowAboutWindow(mainMenuBarHeight, &showAbout);
	}

	if (showCameraWindow)
	{
		ShowEditorCameraWindow(mainMenuBarHeight, &showCameraWindow);
	}

	if (showTexturesWindow)
	{
		ShowTexturesWindow(mainMenuBarHeight, &showTexturesWindow);
	}

	if (showOpenGLWindow)
	{
		ShowOpenGLWindow(mainMenuBarHeight, &showOpenGLWindow);
	}

	ImGui::Render();

	if (shouldQuit)
	{
		return UpdateStatus::UPDATE_STOP;
	}
	else
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
}

/* Called before quitting */
bool ModuleImGui::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleImGui::LinkToWebPage(const char* text, const char* url)
{
	if (ImGui::Selectable(text, false))
		ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}

void ModuleImGui::ShowAboutWindow(float mainMenuBarHeight, bool* pOpen)
{
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::Begin("About", pOpen, ImGuiWindowFlags_NoCollapse);

	LinkToWebPage("Hell Engine v0.0.1", "https://github.com/BrunoAOR/HellEngine");
	ImGui::TextWrapped("The one and only 3D Game Engine that guarantees an increased developer suicide rate!");
	ImGui::Text("");

	ImGui::Text("Authors:");
	ImGui::Bullet(); LinkToWebPage("Marc Campins", "https://github.com/McCampins");
	ImGui::Bullet(); LinkToWebPage("Bruno Ortiz", "https://github.com/BrunoAOR");
	ImGui::Bullet(); LinkToWebPage("Sergi Ramirez", "https://github.com/Orphen5");
	ImGui::Bullet(); LinkToWebPage("Daniel Reyes", "https://github.com/Winour");
	ImGui::Bullet(); LinkToWebPage("Mario Rodriguez", "https://github.com/Mac092");
	ImGui::Text("");

	ImGui::Text("3rd party libraries used:");
	ImGui::Bullet(); LinkToWebPage("SDL 2.0.4", "https://www.libsdl.org");
	ImGui::Bullet(); LinkToWebPage("SDL Image 2.0.0", "https://www.libsdl.org/projects/SDL_image/");
	ImGui::Bullet(); LinkToWebPage("SDL Mixer 2.0.0", "https://www.libsdl.org/projects/SDL_mixer/");
	ImGui::Bullet(); LinkToWebPage("Brofiler 1.1.2", "https://github.com/bombomby/brofiler");
	ImGui::Bullet(); LinkToWebPage("MathGeoLib 1.5", "https://github.com/juj/MathGeoLib");
	ImGui::Bullet(); LinkToWebPage("JSON for Modern C++ 3.0.1", "https://github.com/nlohmann/json");
	ImGui::Bullet(); LinkToWebPage("ImGui 1.53", "https://github.com/ocornut/imgui");
	ImGui::Bullet(); LinkToWebPage("Glew 2.1.0", "http://glew.sourceforge.net");
	ImGui::Bullet(); LinkToWebPage("OpenGL 3.1", "https://www.khronos.org/opengl/");
	ImGui::Text("");

	LinkToWebPage("License:", "https://github.com/BrunoAOR/HellEngine/blob/master/LICENSE");
	ImGui::Text("");
	ImGui::TextWrapped(licenseString.c_str());

	ImGui::End();
}

void ModuleImGui::ShowEditorCameraWindow(float mainMenuBarHeight, bool* pOpen)
{
	static float fovh = 0.0f;
	static float fovv = 0.0f;
	static float nearPlane = App->editorCamera->getNearPlaneDistance();;
	static float farPlane = App->editorCamera->getFarPlaneDistance();
	static float aspectRatio = 0.0f;
	static bool active = false;
	static bool frustumCulling = false;
	static bool isActiveCamera = true;

	ImGui::SetNextWindowPos(ImVec2((float)App->window->getWidth() - 300, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::Begin("Camera", pOpen, ImGuiWindowFlags_NoCollapse);

	ImGui::Checkbox("Active", &active);

	static float front[3] = { 0, 0, 0 };
	static const float* editorFront = nullptr;
	editorFront = App->editorCamera->GetFront();
	front[0] = editorFront[0];
	front[1] = editorFront[1];
	front[2] = editorFront[2];
	ImGui::InputFloat3("Front", front, 2);

	static float up[3] = { 0, 0, 0 };
	static const float* editorUp = nullptr;
	editorUp = App->editorCamera->GetUp();
	up[0] = editorUp[0];
	up[1] = editorUp[1];
	up[2] = editorUp[2];
	ImGui::InputFloat3("Up", up, 3);

	static float position[3] = { 0, 0, 0 };
	static const float* editorPos = nullptr;
	editorPos = App->editorCamera->getPosition();
	position[0] = editorPos[0];
	position[1] = editorPos[1];
	position[2] = editorPos[2];
	if (ImGui::InputFloat3("Position", position))
		App->editorCamera->SetPosition(position[0], position[1], position[2]);

	ImGui::SliderFloat("Mov Speed", &App->editorCamera->moveSpeed, 1.0f, 25.0f);
	ImGui::SliderFloat("Rot Speed", &App->editorCamera->rotationSpeed, 1.0f, 25.0f);
	ImGui::SliderFloat("Zoom Speed", &App->editorCamera->zoomSpeed, 10.0f, 200.0f);

	ImGui::Checkbox("Frustum Culling", &frustumCulling);
	if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.01f, 30.0f))
		App->editorCamera->SetNearPlaneDistance(nearPlane);
	if (ImGui::SliderFloat("Far Plane", &farPlane, 50.0f, 2000.0f))
		App->editorCamera->SetFarPlaneDistance(farPlane);

	fovh = App->editorCamera->getHorizontalFOV();
	ImGui::SliderFloat("Fov H", &fovh, 0.1f, 180.0f);
	fovv = App->editorCamera->getVerticalFOV();
	if (ImGui::SliderFloat("Fov V", &fovv, 0.1f, 180.0f))
		App->editorCamera->SetFOV(fovv);
	aspectRatio = App->editorCamera->getAspectRatio();
	ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 5.0f);

	static float backgroundColor[3] = { 0.0f, 0.0f, 0.0f };
	if (ImGui::ColorEdit3("color 1", backgroundColor))
	{
		App->editorCamera->background.r = backgroundColor[0];
		App->editorCamera->background.g = backgroundColor[1];
		App->editorCamera->background.b = backgroundColor[2];
	}

	ImGui::Checkbox("Is Active Camera", &isActiveCamera);

	ImGui::End();
}

void ModuleImGui::ShowTexturesWindow(float mainMenuBarHeight, bool * pOpen)
{
	static bool mipMaps = false;

	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::Begin("Textures", pOpen, ImGuiWindowFlags_NoCollapse);

	GLuint w = App->renderer->GetTextureWidth();
	GLuint h = App->renderer->GetTextureHeight();
	GLuint bytesPerPixel = App->renderer->GetBytesPerPixel();
	GLuint wInBytes = w * bytesPerPixel;
	GLuint hInBytes = h * bytesPerPixel;

	ImGui::Text("Image width (pixels) = "); ImGui::SameLine();
	if (w != 0)
		ImGui::TextColored(ImVec4(0, 0, 1, 1), std::to_string(w).c_str());
	else
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "NA");

	ImGui::Text("Image height (pixels) = "); ImGui::SameLine();
	if (h != 0)
		ImGui::TextColored(ImVec4(0, 0, 1, 1), std::to_string(h).c_str());
	else
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "NA");


	ImGui::Text("Bytes per pixel = "); ImGui::SameLine();
	if (bytesPerPixel != 0)
		ImGui::TextColored(ImVec4(0, 0, 1, 1), std::to_string(bytesPerPixel).c_str());
	else
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "NA");


	ImGui::Text("Image width (bytes) = "); ImGui::SameLine();
	if (wInBytes != 0)
		ImGui::TextColored(ImVec4(0, 0, 1, 1), std::to_string(wInBytes).c_str());
	else
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "NA");


	ImGui::Text("Image height (bytes) = "); ImGui::SameLine();
	if (hInBytes != 0)
		ImGui::TextColored(ImVec4(0, 0, 1, 1), std::to_string(hInBytes).c_str());
	else
		ImGui::TextColored(ImVec4(0, 0, 1, 1), "NA");
	
	ImGui::Checkbox("Mipmaps", &mipMaps);

	ImGui::End();
}

void ModuleImGui::ShowOpenGLWindow(float mainMenuBarHeight, bool * pOpen)
{
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::Begin("OpenGL Configuration", pOpen, ImGuiWindowFlags_NoCollapse);

	ImGui::End();
}
