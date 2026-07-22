#include "pch.hpp"
#include "Data.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>

void Data::Load()
{
    std::filesystem::path pathToData = std::filesystem::path(std::getenv("TEMP")) / "HookCord.bin";
    if(!std::filesystem::exists(pathToData))
    {
        std::cout << "data does not exist\n";
        return;
    }

    std::ifstream data(pathToData, std::ios::binary);
    std::size_t webhookURLLength;
    std::size_t usernameLength;
    std::size_t imageURLLength;
    std::size_t messageLength;

    data.read(reinterpret_cast<char*>(&webhookURLLength), sizeof webhookURLLength);
    data.read(reinterpret_cast<char*>(&usernameLength), sizeof usernameLength);
    data.read(reinterpret_cast<char*>(&imageURLLength), sizeof imageURLLength);
    data.read(reinterpret_cast<char*>(&messageLength), sizeof messageLength);

    char* temp = new char[webhookURLLength+1];
    data.read(temp, webhookURLLength);
    temp[webhookURLLength] = '\0';
    webhookURL = temp;
    delete temp;
    
    temp = new char[usernameLength+1];
    data.read(temp, usernameLength);
    temp[usernameLength] = '\0';
    username = temp;
    delete temp;

    data.read(reinterpret_cast<char*>(&useMCImage), sizeof useMCImage);
    
    temp = new char[imageURLLength+1];
    data.read(temp, imageURLLength);
    temp[imageURLLength] = '\0';
    imageURL = temp;
    delete temp;

    temp = new char[messageLength+1];
    data.read(temp, messageLength);
    temp[messageLength] = '\0';
    message = temp;
    delete temp;
}

void Data::Save()
{
    if(!shouldSave) return;
    
    std::ofstream data(std::filesystem::path(std::getenv("TEMP")) / "HookCord.bin", std::ios::binary);
    std::size_t temp = webhookURL.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = username.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = imageURL.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);
    temp = message.size();
    data.write(reinterpret_cast<char*>(&temp), sizeof temp);


    data.write(webhookURL.data(), webhookURL.size());
    data.write(username.data(), username.size());

    data.write(reinterpret_cast<char*>(&useMCImage), sizeof useMCImage);
    
    data.write(imageURL.data(), imageURL.size());
    data.write(message.data(), message.size());

}