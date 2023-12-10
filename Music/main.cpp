//Based on Singly Linked list a ** Music Player App ** in your CLI

#include <iostream> // input/output stream library
#include <fstream> // functionality for reading and writing files
#include <chrono>  //  utilities for measuring time durations
#include <ctime> // functions for working with date and time information
#include <SFML/Audio.hpp> //It allows for audio-related functionalities like playing music and sounds
#include <vector> //  used for dynamic arrays
#include <string>  //for handling strings
#include <cstring>  // functions for handling strings and arrays of characters
#include <dirent.h> //declarations for the opendir and readdir functions used for directory manipulation

struct Song {
    std::string title;
    std::string filePath;
    Song* next;
    Song* prev;
};

void playSong(const std::string& filePath) {
    sf::Music music;

    if (!music.openFromFile(filePath)) {
        std::cerr << "Error opening audio file: " << filePath << std::endl;
        return;
    }

    // Get the current time and date
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string timestamp = std::ctime(&now);

    music.play();

    std::cout << "Started playing: " << filePath << " at " << timestamp;

    while (music.getStatus() == sf::Music::Playing) {
        sf::sleep(sf::milliseconds(100));
    }

    music.stop();
}

void displayPlaylist(Song* playlist) {
    Song* current = playlist;

    while (current != nullptr) {
        std::cout << "Title: " << current->title << " - Path: " << current->filePath << std::endl;
        current = current->next;
    }
}

void freePlaylist(Song*& playlist) {
    while (playlist != nullptr) {
        Song* temp = playlist;
        playlist = playlist->next;
        delete temp;
    }
}

std::vector<std::string> getSongFiles(const std::string& directoryPath) {
    std::vector<std::string> songFiles;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(directoryPath.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string fileName = entry->d_name;
            if (fileName.size() > 4 && fileName.substr(fileName.size() - 4) == ".ogg") {
                songFiles.push_back(directoryPath + "/" + fileName);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << directoryPath << std::endl;
    }

    return songFiles;
}

void createPlaylist(const std::vector<std::string>& songFiles, Song*& playlist) {
    Song* head = nullptr;
    Song* tail = nullptr;

    for (const auto& filePath : songFiles) {
        // Extracting title from the file name (assuming file name is the title)
        std::string title = filePath.substr(filePath.find_last_of("/\\") + 1);
        title = title.substr(0, title.size() - 4);  // Remove ".ogg" extension
        Song* newSong = new Song{title, filePath, nullptr, nullptr};

        if (head == nullptr) {
            head = newSong;
            tail = newSong;
        } else {
            tail->next = newSong;
            newSong->prev = tail;
            tail = newSong;
        }
    }

    playlist = head;
}

void logToFile(const std::string& logMessage) {
    std::ofstream logFile("log.txt", std::ios::app);
    if (logFile.is_open()) {
        // Get the current time and date for the log entry
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string timestamp = std::ctime(&now);

        logFile << "[" << timestamp << "]  " << logMessage << std::endl;
        logFile << "-------------------------------------------" << std::endl; //separator
        logFile.close();
    } else {
        std::cerr << "Error opening log file." << std::endl;
    }
}

int main() {
    // Directory containing the songs
    std::string songsDirectory = "songs";

    // Get the list of song files in the directory
    std::vector<std::string> songFiles = getSongFiles(songsDirectory);

    if (songFiles.empty()) {
        std::cerr << "No song files found in the directory: " << songsDirectory << std::endl;
        return 1;
    }

    // Create a playlist from the song files
    Song* playlist = nullptr;
    createPlaylist(songFiles, playlist);

    // Display the playlist
    std::cout << "Playlist:" << std::endl;
    displayPlaylist(playlist);

    // Play the songs
    Song* current = playlist;
    while (current != nullptr) {
        playSong(current->filePath);

        // Log information about the played song with timestamp
        std::string logMessage = "Started playing: " + current->title + " - Path: " + current->filePath;
        logToFile(logMessage);

        current = current->next;
    }

    // Free the allocated memory
    freePlaylist(playlist);

    return 0;
}


