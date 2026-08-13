#pragma once

class Data
{
public:
    bool specifyName         = true;
	bool specifyImage        = true;
	bool useMCImage          = true;
    bool tts                 = false;
    bool editMessage         = false;
    
    std::string webhookURL   = "";
    std::string username     = "";
	std::string imageURL     = "";
	std::string message      = "";
    std::string idToEdit     = "";
    std::string preMessageId = "";

    void Load();
    void Save();
    bool shouldSave        = false;
};