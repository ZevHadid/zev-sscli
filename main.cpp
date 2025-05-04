#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <cstdlib>

std::string directory = "C:\\Windows\\";

std::string removeExtraSpaces(const std::string& input) {
    std::string result;
    bool inSpace = false;

    for (char ch : input) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!inSpace) {
                result += ' ';
                inSpace = true;
            }
        } else {
            result += ch;
            inSpace = false;
        }
    }

    if (!result.empty() && result.front() == ' ')
        result.erase(result.begin());

    if (!result.empty() && result.back() == ' ')
        result.pop_back();

    return result;
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    
    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time))) {
        return std::string(buffer);
    } else {
        return "unknown time";
    }
}

void createCMD(const std::string &exePath, const std::string &aliasName) {
    std::ofstream batFile("C:\\Windows\\" + aliasName + ".bat");
    batFile << "@echo off\n\"" << exePath << "\" %*";
    batFile.close();
}

bool removeCMD(const std::string &aliasName) {
    std::string batPath = "C:\\Windows\\" + aliasName + ".bat";
    if (std::remove(batPath.c_str())) {
        std::cerr << "Error: Could not delete " << batPath << std::endl;
        return false;
    } else {
        std::cout << "Successfully removed alias: " << aliasName << std::endl;
        return true;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {

        if (std::string(argv[1]) == "cmd-list") {
            
            std::cout << "Listing commands located in: " << directory << std::endl;
            
            try {
                for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                    if (entry.is_regular_file()) {
                        std::string filename = entry.path().filename().string();
                        if (filename.ends_with(".exe") || filename.ends_with(".bat") || filename.ends_with(".cmd")) {
                            std::cout << "Found command: " << filename << std::endl;
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Zerror: " << e.what() << std::endl;
            }
        }
    } else if (std::string(argv[1]) == "create-cmd") {
        std::string executable_path = std::string(argv[2]);
        createCMD(executable_path, std::string(argv[3]));

        std::cout << "Command Successfully created in " << directory << " {" << std::endl;
        std::cout << "  name: " << argv[3] << std::endl;
        std::cout << "  executable: " << executable_path << std::endl;

        std::string cmd_description = std::string(argv[5]);
        cmd_description = removeExtraSpaces(cmd_description.substr(1, cmd_description.length() - 2));

        if (argc == 5 && std::string(argv[4]) == "-desc" && !cmd_description.empty()) {
            std::cout << "  description: " << " [" << std::endl;
            
            std::istringstream iss(cmd_description);

            std::string word;
            int count = 0;
        
            std::cout << "    ";
        
            while (iss >> word) {
                std::cout << word << " ";
                count++;
        
                if (count == 8) {
                    std::cout << std::endl << "    ";
                    count = 0;
                }
            }
        
            std::cout << std::endl;

            std::cout << "  ]" << std::endl;
        } else {
            std::cout << "  description: " << "[]" << std::endl;
        }

        std::ofstream outFile("zev-commands.txt");
        if (outFile.is_open()) {
            outFile << std::string(argv[3]) << "\n";
            outFile << executable_path<< "\n";
            outFile << cmd_description << "\n";
            outFile << getCurrentTimestamp() << "\n";
            outFile.close();
        } else {
            std::cerr << "Error opening file!" << std::endl;
        }
    }

    return 0;
}
