#pragma once
#include "Data.hpp"

class App
{
public:
    App(const char* name, size_t width, size_t height, bool canResize);
    ~App();

    void Update();
    void OnSendPress();
    void SanitizeMessage();
private:
    Data data;

    //Network
    CURL* curl;
	CURLcode result;
    struct curl_slist *headers = NULL;
    
    //Window
    GLFWwindow* window;
};