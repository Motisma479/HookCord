#include "pch.hpp"
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include <cmath>

#include <curl/curl.h>

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

int main(int argc, char** argv)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// this row is used to stop at new/malloc N 284
	//_CrtSetBreakAlloc(284);
#endif

	std::cout << "Hello World!\n";

	curl_version_info_data* info = curl_version_info(CURLVERSION_NOW);
    std::cout << "Protocoles supportes par votre curl : " << std::endl;
    
    for (int i = 0; info->protocols[i] != nullptr; ++i) {
        std::cout << "- " << info->protocols[i] << std::endl;
    }

	CURL* curl;
	CURLcode result;
	result = curl_global_init(CURL_GLOBAL_ALL);
	if(result != CURLE_OK)
		return 1;
	
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
	
	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

	ImGui::CreateContext();
	ImGuiIO* guiIO = &ImGui::GetIO();
	guiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	guiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	std::string webhookURL;
	std::string username;
	bool useMCImage = true;
	std::string message;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::Begin("Main Window", NULL, flags);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close", "Alt+f4"))  { glfwSetWindowShouldClose(window, true); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("webhook URL:");
		ImGui::SameLine();
		ImGui::InputText("##WebhookURLBox", &webhookURL);
		ImGui::Text("Username:");
		ImGui::SameLine();
		ImGui::InputText("##UsernameBox", &username);
		ImGui::Checkbox("Use minecraft image", &useMCImage);//TODO
		ImGui::Text("Message:");
		ImGui::InputTextMultiline("##MessageBox", &message);
		if(ImGui::Button("Send"))
		{
			curl = curl_easy_init();
			if(curl) {
				
				std::string json = "{\n   \"username\": \"" + username + "\",\n   \"avatar_url\": \"https://mc-api.io/render/face/" + username + "/java?size=1024\",\n   \"content\": \"" + message + "\",\n   \"allowed_mentions\": {\"parse\":[\"everyone\", \"roles\", \"users\"]}\n}";
				curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
				curl_easy_setopt(curl, CURLOPT_URL, webhookURL.c_str());
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
			
				result = curl_easy_perform(curl);
			
				if(result != CURLE_OK)
					std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
			
				curl_easy_cleanup(curl);
			}
			message.clear();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
	curl_global_cleanup();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
