#include "pch.hpp"
#include "Data.hpp"
#include <cstdlib>
#include <fstream>

void Data::Load()
{
    std::filesystem::path pathToData = std::filesystem::temp_directory_path() / "HookCord.bin";
    if(!std::filesystem::exists(pathToData))
    {
        std::cout << "data does not exist\n";
        return;
    }

    std::ifstream data(pathToData, std::ios::binary);
    std::size_t Lengths[6];

    //read sizes
    data.read(reinterpret_cast<char*>(&Lengths), sizeof(std::size_t) * 6);

    //read bools
    data.read(reinterpret_cast<char*>(&specifyName), sizeof(bool) * 5);

    //read strings
    auto ReadStr = [&](std::string& str, int id)
    {
        char* temp = new char[Lengths[id]+1];
        data.read(temp, Lengths[id]);
        temp[Lengths[id]] = '\0';
        str = temp;
        delete[] temp;
    };
    ReadStr(webhookURL, 0);
    ReadStr(username, 1);
    ReadStr(imageURL, 2);
    ReadStr(message, 3);
    ReadStr(idToEdit, 4);
    ReadStr(preMessageId, 5);
}

void Data::Save()
{
    if(!shouldSave) return;
    
    std::ofstream data(std::filesystem::temp_directory_path() / "HookCord.bin", std::ios::binary);
    //Save string sizes
    std::size_t temp = webhookURL.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = username.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = imageURL.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = message.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = idToEdit.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = preMessageId.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);

    //Save bools
    data.write(reinterpret_cast<char*>(&specifyName), sizeof specifyName);
    data.write(reinterpret_cast<char*>(&specifyImage), sizeof specifyImage);
    data.write(reinterpret_cast<char*>(&useMCImage), sizeof useMCImage);
    data.write(reinterpret_cast<char*>(&tts), sizeof tts);
    data.write(reinterpret_cast<char*>(&editMessage), sizeof editMessage);

    //Save strings
    data.write(webhookURL.data(), webhookURL.size());
    data.write(username.data(), username.size());
    data.write(imageURL.data(), imageURL.size());
    data.write(message.data(), message.size());
    data.write(idToEdit.data(), idToEdit.size());
    data.write(preMessageId.data(), preMessageId.size());
}

bool Data::Evaluate(std::string& errMessage)
{
    errMessage = "Invalid:";
    bool result = false;

    if(webhookURL.empty())
    {
        result = true;
        errMessage += "\n\n\t* Webhook URL is empty.";
    }   

    if(specifyName && username.empty())
    {
        result = true;
        errMessage += "\n\n\t* Username is empty.";
    }

    if(specifyImage && !useMCImage && imageURL.empty())
    {
        result = true;
        errMessage += "\n\n\t* Image URL is empty.";
    }

    if(editMessage && idToEdit.empty())
    {

        result = true;
        errMessage += "\n\n\t* Edit Message ID is empty.";
    }

    if(message.empty())
    {
        result = true;
        errMessage += "\n\n\t* Message is empty.";
    } 

    return result;
}