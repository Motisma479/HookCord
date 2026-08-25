#pragma once
#include "Data.hpp"

class App
{
public:
    App(const char* name, int width, int height, bool canResize);
    ~App();

    void Update();
    void OnSendPress();
    void GetOldMessageForEdit();
    void SanitizeMessage();
private:
    Data data;
    bool isDataIncorrect = false;
    std::string errMessage;

    //Network
    CURL* curl;
	CURLcode result;
    struct curl_slist *headers = NULL;
    
    //Window
    GLFWwindow* window;
};