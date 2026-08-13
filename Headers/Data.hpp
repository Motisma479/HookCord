#pragma once

class Data
{
public:
    std::string webhookURL = "";
	bool specifyName = true;
    std::string username   = "";
	bool specifyImage = true;
	bool useMCImage        = true;
	std::string imageURL    = "";
	std::string message    = "";

    void Load();
    void Save();
    bool shouldSave        = false;
};