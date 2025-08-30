#define CURL_STATICLIB


/*
I'm using libcurl library
*/

#include <iostream>
#include <string>

#include <curl/curl.h>

enum requestType{
    GET,
    POST,
    DELETE_,
    PUT,
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void HttpRequest(CURL* curl, CURLcode& res, std::string& readBuffer, std::string request, requestType type) {
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        if (type == requestType::GET) {
            curl_easy_setopt(curl, CURLOPT_URL, request);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        }
        else if (type == requestType::POST) {
            curl_easy_setopt(curl, CURLOPT_URL, request);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        }
        else if (type == requestType::DELETE_) {
            curl_easy_setopt(curl, CURLOPT_URL, request);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        }
        else {
            curl_easy_setopt(curl, CURLOPT_URL, request);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        }

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        /*else {

            std::cout << "Response:\n" << readBuffer << std::endl;
        }*/
        curl_easy_cleanup(curl);
    }
    else {
        std::cerr << "Failed to initialize CURL." << std::endl;
    }
    curl_global_cleanup();
}

void replacingShifts(std::string& line) {
    for (int i{}; i < (int)line.size() - 1; i++) {
        if (line[i] == ' ') {
            line[i] = '_';
        }
    }
}

int main() {
    std::cout << "WELCOME TO THE AMAZING TO_DO_LIST PROJECT CREATED BY ARHANCRANE\n";

    //For Http Requests
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    //For Http Requests


    std::string login;
    std::string password;
    std::string port = "44521";
    bool HaveYouLoged{false};

    int option;
        
    while (true) {
        std::cout << "\nLog in   (1)\n" <<
            "Register (2)\n"
            "Quit     (0)\n";
        std::cin >> option;
        if (option == 1) { //Log in
            while (true) {
                std::cout << "\nEnter your login: ";
                std::cin >> login;
                std::cout << "\nEnter your password: ";
                std::cin >> password;
                std::cin.ignore();
                HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/login?name=" + login + "&password=" + password, GET);
                if (readBuffer == "the entry is successful\n") {
                    HaveYouLoged = true;
                    break;
                }
                else {
                    std::cout << "\nthe login or password is incorrect\n";
                    break;
                }
            }
            if (HaveYouLoged) {
                break;
            }            
        }
        else if (option == 2) { //Register
            std::cout << "\nEnter your login: ";
            std::cin >> login;
            std::cout << "\nEnter your password: ";
            std::cin >> password;
            HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/register?name=" + login + "&password=" + password, POST);
            HaveYouLoged = true;
            break;            
        }
        else {
            std::cout << "\nYou have a nice rest of your day!";
            return 0;
        }
    }
    
    
    if (HaveYouLoged) {
        std::cout << "\nWelcome, " + login + "\n";
        while (true) {
            
            std::cout << "\nMake a new item           (1)\n";
            std::cout << "Get the list, made before (2)\n";
            std::cout << "Delete a particular item  (3)\n";
            std::cout << "Edit an existing item     (4)\n";
            std::cout << "Close the app             (0)\n";
            std::cin >> option;
            std::cin.ignore();
            
            std::string title{"ForGetRequestItIsWorthless"};
            std::string description{};

            if (option == 1) {
                std::cout << "\nEnter the title of your note: ";
                //std::cin >> title;
                std::getline(std::cin, title);
                replacingShifts(title);
                std::cout << "\nAdd some description of your note: ";
                //std::cin >> description;
                std::getline(std::cin, description);
                replacingShifts(description);

                HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/todos?name=" + login + "&title=" + title + "&value=" + description, POST);
            }
            else if (option == 2) {
                readBuffer = "";
                HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/todos?name=" + login + "&title=" + title, GET);
                std::cout << "\n" << readBuffer;
            }
            else if (option == 3) {
                std::cout << "\nEnter the title of your note: ";
                //std::cin >> title;
                std::getline(std::cin, title);
                replacingShifts(title);
                HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/todos?name=" + login + "&title=" + title, DELETE_);
            }
            else if (option == 4) {
                std::cout << "\nEnter the title of your note: ";
                //std::cin >> title;
                std::getline(std::cin, title);
                replacingShifts(title);
                std::cout << "\nWrite a new description of your note: ";
                //std::cin >> description;
                std::getline(std::cin, description);
                replacingShifts(description);
                //std::getline(std::cin, description);
                HttpRequest(&curl, res, readBuffer, "localhost:" + port + "/todos?name=" + login + "&title=" + title + "&value=" + description, PUT);
            }
            else {
                std::cout << "\nYou have a nice rest of your day!";
                return 0;
            }
            std::cout << "\n";
        }
    }
    std::cout << "\nYou have a nice rest of your day!";    
    return 0;
}