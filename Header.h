#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <regex>
#include <fstream>

struct alluser {
    std::string name;
    std::string email;
    std::string password;
    std::string userID;
    std::string joiningDate;
    std::vector<std::pair<time_t, time_t>> sessions;
    double totalPrintCost = 0;
    double totalScanCost = 0;
};

std::string currentUserEmail;
extern std::unordered_map<std::string, alluser> users;
extern double printCostPerPage;
extern double scanCostPerPage;

const std::string filename = "user_data.txt";
int counter = users.size();

std::string generateUserId();
void loadUsersFromFile();
void saveUserToFile(const alluser& user);
void registerUser();
bool loginUser();
void startSession(const std::string& userID);
void endSession(const std::string& userID);
void startBrowsing(const std::string& userID);
void endBrowsing(const std::string& userID);
void Billdoc(const std::string& email, double print, double scan, double internet, double debt);
bool loginAdmin();
void mainMenu();
void viewUserBills();
void adminMenu();
void viewAllUsers();
void deleteUser();
void editUser();

std::unordered_map<std::string, alluser> users;
double printCostPerPage = 0.20;
double scanCostPerPage = 0.10;

std::string generateUserId() {
    std::string userId;
    int suffix = 1;

    do {
        userId = "User " + std::to_string(suffix);
        suffix++;
    } while (users.find(userId) != users.end());

    return userId;
}

std::string formatJoiningDate(time_t date) {
    char buffer[80];
    struct tm* timeinfo = localtime(&date);
    strftime(buffer, sizeof(buffer), "%A, %d %B %Y  %I:%M:%S %p", timeinfo);
    return std::string(buffer);
}

void loadUsersFromFile() {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "No user data file found, starting fresh.\n";
        return;
    }

    std::string name, email, password, userID, joiningDateStr;

    while (file >> std::ws && std::getline(file, name) &&
        std::getline(file, email) &&
        std::getline(file, password) &&
        std::getline(file, userID) &&
        std::getline(file, joiningDateStr)) {

        alluser user = { name, email, password, userID, joiningDateStr };
        users[email] = user;
        file.ignore();
    }
}

void saveUserToFile(const alluser& user) {
    std::ofstream file("user_data.txt", std::ios::app);
    if (file) {
        file << user.name << "\n" << user.email << "\n" << user.password << "\n"
            << user.userID << "\n" << user.joiningDate << "\n"
            << "_____________________________________________________\n\n\n\n";

        std::cout << "User data saved to file.\n";
    }
    else {
        std::cout << "Error saving user data to file.\n";
    }
}

// [Previous functions remain unchanged up to registerUser()]

void registerUser() {
    alluser user;
    while (true) {
        std::cout << "Enter your name: ";
        std::getline(std::cin, user.name);
        std::regex nameRegex("^[A-Za-z\\s]+$");

        if (!std::regex_match(user.name, nameRegex)) {
            std::cout << "Invalid name. Please enter a name containing only alphabetic characters and spaces.\n";
            continue;
        }
        break;
    }

    while (true) {
        std::cout << "Enter your email: ";
        std::getline(std::cin, user.email);

        if (!isValidEmail(user.email)) continue;

        if (users.find(user.email) != users.end()) {
            std::cout << "User with this email: \"" << user.email << "\" is already registered. Please type a different email.\n";
        }
        else {
            break;
        }
    }

    while (true) {
        std::cout << "Enter your password: ";
        std::getline(std::cin, user.password);
        if (isValidPassword(user.password)) break;
    }

    user.userID = generateUserId();
    time_t currentTime = time(nullptr);
    std::string formattedJoiningDate = formatJoiningDate(currentTime);
    user.joiningDate = formattedJoiningDate;

    users[user.email] = user;
    saveUserToFile(user);
    std::cout << "User joined on: " << formatJoiningDate(currentTime) << std::endl;
    std::cout << "User registered with ID: " << user.userID << std::endl;
}

// [The rest of the code remains unchanged until Billdoc]

void Billdoc(const std::string& email, double print, double scan, double internet, double debt) {
    std::ofstream file("bill.txt", std::ios::app);

    if (file.is_open()) {
        file << "Account: " << currentUserEmail << "\n"
            << "Print: NZD " << print << "\n"
            << "Scan: NZD " << scan << "\n"
            << "Internet: NZD " << internet << "\n\n"
            << "Total Outstanding: NZD " << debt << "\n"
            << "_____________________________________________________\n\n";

        std::cout << "Bill for " << email << " saved to bill.txt\n";
        file.flush();
    }
    else {
        std::cerr << "Failed to open bill.txt for writing.\n";
    }
}

void Viewbill(const std::string& email) {
    std::ifstream file("bill.txt");

    if (file.is_open()) {
        std::string line;
        bool found = false;

        while (std::getline(file, line)) {
            if (line.find("Account: " + email) != std::string::npos) {
                found = true;
                double print = 0.0, scan = 0.0, internet = 0.0, debt = 0.0;

                std::getline(file, line);
                std::sscanf(line.c_str(), "Print: NZD %lf", &print);

                std::getline(file, line);
                std::sscanf(line.c_str(), "Scan: NZD %lf", &scan);

                std::getline(file, line);
                std::sscanf(line.c_str(), "Internet: NZD %lf", &internet);

                std::getline(file, line);
                std::sscanf(line.c_str(), "Total Outstanding: NZD %lf", &debt);

                std::cout << "Bill for " << email << ":\n"
                    << "Print: NZD " << print << "\n"
                    << "Scan: NZD " << scan << "\n"
                    << "Internet: NZD " << internet << "\n"
                    << "Total Outstanding: NZD " << debt << "\n";
                std::cout << "_____________________________________________________\n";
                break;
            }
        }

        if (!found) {
            std::cout << "No bill found for " << email << "\n";
        }

        file.close();
    }
    else {
        std::cerr << "Failed to open bill.txt for reading.\n";
    }
}

void viewBillForLoggedInUser() {
    std::string email = currentUserEmail;
    std::cout << "Enter the user's email to view their bill: ";
    std::getline(std::cin, email);
    Viewbill(email);
}

void logoutUser(const std::string& userID) {
    if (users.find(userID) != users.end()) {
        const std::string& email = users[userID].email;
        double printCost = users[userID].totalPrintCost;
        double scanCost = users[userID].totalScanCost;
        double internetCost = Tcost;
        double totalDebt = printCost + scanCost + internetCost;

        Billdoc(email, printCost, scanCost, internetCost, totalDebt);

        std::cout << "Logging out...\n";
        mainMenu();
    }
    else {
        std::cout << "User not found!\n";
    }
}

// [Rest of the code remains unchanged]