#include <iostream>
#include <map>
#include <ctime>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include "json.hpp"

namespace j = nlohmann;

const char* JSON_FILENAME{"schedule.json"};
const std::string EXEC_PATH{ '\"' + std::string{std::getenv("APPDATA")} + R"(\Zoom\bin\Zoom.exe)" + '\"'};
const unsigned CHECK_EVERY_MS{static_cast<unsigned>(59.99 * 1000)};
const std::string DAYS_OF_WEEK_ARR[7]{"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};

void sleep(unsigned ms){ Sleep(ms); }

int main () {
	std::cout << "path: " <<  EXEC_PATH << std::endl;
    std::system("color A");
    
    std::cout << "Starting zoom" ;
    std::system((R"(start "" )" + EXEC_PATH).c_str());
    for( int i {0}; i < 5; i++){
        sleep(500);
        std::cout << '.';
    }
    std::cout << std::endl;
    

    
    // Read JSON file
    std::ifstream ifile{JSON_FILENAME};
    if (!ifile.is_open()) {
        std::cerr << "Could not find file with name: "  << JSON_FILENAME;
        system("pause");
        exit(1);
    }

    // Read the file into the json
    j::json jsonObj;
    ifile >> jsonObj;
    ifile.close();
    

    while (true){

        // Now time
        const time_t  tt{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        const tm now{*std::localtime(&tt)};
        
        // Get the weekday string
        const auto& wdayStr{DAYS_OF_WEEK_ARR[now.tm_wday]};
        
        // Format the hh:mm string
        const auto hourStr{std::to_string(now.tm_hour)};
        const auto minStr{ now.tm_min < 10 
            ? '0' + std::to_string(now.tm_min) 
            : std::to_string(now.tm_min)
        }; // Add first 0 to minutes under 10
        const auto timeStr{ hourStr + ':' + minStr};
        
        
        std::cout << wdayStr << ' ' << timeStr << ' ';
        
        
        try{
            const auto& nowSched = jsonObj.at(wdayStr).at(timeStr);
            
            const auto& link {nowSched.at("link").get<std::string>()};
            const auto& actName{nowSched.at("name").get<std::string>()};

            // Print the time
            std::cout << "- You have a meeting!" << std::endl;
            std::cout << "=========== Meeting ===========" << std::endl;
            std::cout << actName << std::endl;
            std::cout << link << std::endl;

            // Join start the program
            std::system((std::string{R"(start "" )"} + link).c_str());
            
            // Display feedback
            std::cout << "\nJoining meeting..." << std::endl;


        }catch(std::exception& e){ 
            e; 
            std::cout << "- No meetings right now." << std::endl;
        }
        sleep(CHECK_EVERY_MS);
    }
    
    return 0;
}