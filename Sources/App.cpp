#include "pch.hpp"
#include "App.hpp"
#include "Utils.hpp"

#include <glad/gl.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <misc/cpp/imgui_stdlib.h>

#include <JSON_CPP.hpp>

App::App(const char* name, int width, int height, bool canResize)
{
    data.Load();
    data.isAutoStart = IsAutoStartEnable();

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
	ImGuiIO& guiIO = ImGui::GetIO();
	guiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    guiIO.IniFilename = NULL;
    guiIO.IniSavingRate = 0.0;
    guiIO.LogFilename = NULL;

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
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
        const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollWithMouse;
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
            if (ImGui::BeginMenu("Settings"))
			{
                if (ImGui::Checkbox("Auto Start",&data.isAutoStart)) {ToggleAutoStart(data.isAutoStart);}
                if(ImGui::BeginItemTooltip())
                {
                    ImGui::Text("Only check this option if the app is at a static location");
                    ImGui::EndTooltip();
                }
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("Webhook URL:");
		ImGui::SameLine(100);
		if(ImGui::InputText("##webhookURLBox", &data.webhookURL))
            data.shouldSave = true;
		
        ImGui::Separator();

        if(ImGui::Checkbox("Specify name?", &data.specifyName))
        {
            data.shouldSave = true;
        }
        if(data.specifyName)
        {
            ImGui::Text("Username:");
            ImGui::SameLine(100);
            if(ImGui::InputText("##usernameBox", &data.username))
            data.shouldSave = true;
        }
		
        if(ImGui::Checkbox("Specify image?", &data.specifyImage))
        {
            data.shouldSave = true;
        }

        if(data.specifyImage)
        {
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
        }
		
        ImGui::Separator();
        if(ImGui::Checkbox("Edit?", &data.editMessage))
            data.shouldSave = true;
        if(data.editMessage)
        {
            ImGui::SameLine();
            ImGui::Text("Message Id:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(200);
            if(ImGui::InputText("##MessageIdBox", &data.idToEdit))
                data.shouldSave = true;
        }

        ImGui::Text("Message:");
        if(data.editMessage)
        {
            ImGui::SameLine();
            if(ImGui::Button("Get Old Message"))
            {
                GetOldMessageForEdit();
            }
        }
		if(ImGui::InputTextMultiline("##MessageBox", &data.message, {435, 135}))
            data.shouldSave = true;

		if(ImGui::Button("Send"))
		{
            OnSendPress();
		}
        ImGui::SameLine();
        if(ImGui::Checkbox("TTS?", &data.tts))
            data.shouldSave = true;
        
        ImGui::Text("Previous Id:");
		ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("##PreMessageIdBox", &data.preMessageId, ImGuiInputTextFlags_ReadOnly);

		ImGui::End();

        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;

    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);

    return totalSize;
}
void App::OnSendPress()
{
    curl = curl_easy_init();
    if(curl) {
        SanitizeMessage();
        JSON json;
        json["content"] = data.message;
        if(!data.editMessage)
        {
            if(data.specifyName) json["username"] = data.username;
            if(data.specifyImage) json["avatar_url"] = data.imageURL;
            if(data.tts) json["tts"] = true;
            json["allowed_mentions"]["parse"] = JSON::array("everyone", "roles", "users");
        }

        std::cout << json << std::endl;
        if(data.editMessage)
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        else
            curl_easy_setopt(curl, CURLOPT_POST, 1L);

        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        std::string url = data.webhookURL + "?wait=true";
        if(data.editMessage)
            url = data.webhookURL + "/messages/" + data.idToEdit;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        std::string body = json.ToString(false);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);


        result = curl_easy_perform(curl);
    
        if(result != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
        else
        {
            long responseCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

            std::cout << "HTTP " << responseCode << '\n';
            JSON responseJson;
            responseJson.FromString(response);
            std::cout << responseJson << std::endl;

            if(responseCode == 200)
            {
                data.preMessageId = responseJson["id"].operator std::string();
            }
        }

        curl_easy_cleanup(curl);
    }
    data.message.clear();
    data.shouldSave = true;
}

void App::GetOldMessageForEdit()
{
    curl = curl_easy_init();
    if(!curl) return;
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "http");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    std::string url = data.webhookURL + "/messages/" + data.idToEdit;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
    std::string response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);


    result = curl_easy_perform(curl);
    if(result != CURLE_OK)
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(result) << std::endl;
    else
    {
        long responseCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

        std::cout << "HTTP " << responseCode << '\n';
        JSON responseJson;
        responseJson.FromString(response);
        std::cout << responseJson << std::endl;
        if(responseCode == 200)
        {
            data.message = responseJson["content"].operator std::string();
        }
    }
    data.shouldSave = true;
}

void App::SanitizeMessage()
{
    std::string sanitizedMessage;
    sanitizedMessage.reserve(data.message.size());
    for(int i = 0; i < data.message.size(); ++i)
    {
        char current = data.message[i]; 
        switch (current)
        {
        case '\n':
            sanitizedMessage += "\\n";
            break;
        case '\\':
            sanitizedMessage += "\\\\";
            break;
        default:
            sanitizedMessage+=current;
            break;
        }
    }
    data.message = sanitizedMessage;
}
