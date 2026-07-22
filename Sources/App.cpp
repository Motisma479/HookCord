#include "pch.hpp"
#include "App.hpp"

#include <glad/gl.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

App::App(const char* name, size_t width, size_t height, bool canResize)
{
    data.Load();

    //Network
    result = curl_global_init(CURL_GLOBAL_ALL);
	if(result != CURLE_OK)
		throw std::runtime_error("cannot init network");
	
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
	
	//Window
    if (!glfwInit())
		throw std::runtime_error("cannot init window");
    if(!canResize)
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
    		throw std::runtime_error("cannot init window");
    }
    glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress))
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		throw std::runtime_error("cannot init window");
	}

    ImGui::CreateContext();
	ImGuiIO* guiIO = &ImGui::GetIO();
	guiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	guiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

App::~App()
{
    //Clean Network
    curl_global_cleanup();

    //Clean GUI
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    //Clean window
	glfwDestroyWindow(window);
    glfwTerminate();

    data.Save();
}

void App::Update()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::Begin("Main Window", NULL, flags);

        //Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close", "Alt+f4"))  { glfwSetWindowShouldClose(window, true); }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("Webhook URL:");
		ImGui::SameLine(100);
		if(ImGui::InputText("##webhookURLBox", &data.webhookURL))
            data.shouldSave = true;
		
        ImGui::Separator();

        ImGui::Text("Username:");
		ImGui::SameLine(100);
		if(ImGui::InputText("##usernameBox", &data.username))
            data.shouldSave = true;
		
        if(ImGui::Checkbox("Use minecraft image", &data.useMCImage))
        {
            data.imageURL.clear();
            data.shouldSave = true;
        }
        if(!data.useMCImage)
        {
            ImGui::Text("Image URL:");
            ImGui::SameLine(100);
            if(ImGui::InputText("##imageURLBox", &data.imageURL)) 		    
                data.shouldSave = true;
        } else {
            data.imageURL = "https://mc-api.io/render/face/" + data.username + "/java?size=1024";
        }
		
        ImGui::Separator();

        ImGui::Text("Message:");
		if(ImGui::InputTextMultiline("##MessageBox", &data.message, {435, 135}))
            data.shouldSave = true;

		if(ImGui::Button("Send"))
		{
            OnSendPress();
		}
        
		ImGui::End();

        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void App::OnSendPress()
{
    curl = curl_easy_init();
    if(curl) {
        
        std::string json = "{\n   \"username\": \"" + data.username + "\",\n   \"avatar_url\": \"" + data.imageURL + "\",\n   \"content\": \"" + data.message + "\",\n   \"allowed_mentions\": {\"parse\":[\"everyone\", \"roles\", \"users\"]}\n}";
        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, data.webhookURL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
    
        result = curl_easy_perform(curl);
    
        if(result != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
    
        curl_easy_cleanup(curl);
    }
    data.message.clear();
    data.shouldSave = true;
}