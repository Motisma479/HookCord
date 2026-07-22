#pragma once

class Data
{
public:
    std::string webhookURL = "";
	std::string username   = "";
	bool useMCImage        = true;
	std::string imageURL    = "";
	std::string message    = "";

    void Load();
    void Save();
    bool shouldSave        = false;
};