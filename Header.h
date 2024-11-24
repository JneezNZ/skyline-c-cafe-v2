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
void autodeleteUser(std::string); 

std::unordered_map<std::string, alluser> users;
double printCostPerPage = 0.20;
double scanCostPerPage = 0.10;



bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"((^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$))");
    if (std::regex_match(email, pattern)) {
        std::cout << "Email \"" << email << "\" is valid.\n";
        return true;
    }
    else {
        std::cout << "Email \"" << email << "\" is invalid. Please try again\n";
        return false;
    }
}

void viewAllUsers() {
    if (users.empty()) {
        std::cout << "No users to display.\n";
        std::cout << "-----------------------------\n";
        adminMenu();
        return;
    }

    for (const auto& i: users) {
        if (i.second.name.empty() || i.second.email.empty()) {
            continue; // Skip invalid entries
        }
        std::cout << "User ID: " << i.second.userID << '\n'
            << "Name: " << i.second.name << '\n'
            << "Email: " << i.second.email << '\n'
            << "-------------------------------\n";
    }
    adminMenu();
}



bool isValidPassword(const std::string& password) {
    const std::regex passwordPattern(R"((?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*])[A-Za-z\d!@#$%^&*]{8,})");

    if (std::regex_match(password, passwordPattern)) {
        std::cout << "Password \"" << password << "\" is valid.\n";
        return true;
    }
    else {
        std::cout << "Password is invalid. Requirements:\n"
            << "- At least 8 characters long\n"
            << "- At least one uppercase letter\n"
            << "- At least one lowercase letter\n"
            << "- At least one digit (0-9)\n"
            << "- At least one special character (!@#$%^&*)\n";
        return false;
    }
}

std::string generateUserId() {
    std::string userId;
    int suffix = 1;

    do {
        suffix++;
        userId = "User " + std::to_string(suffix);
        
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
    int lineNumber = 0;

    while (file >> std::ws &&
        std::getline(file, name) &&
        std::getline(file, email) &&
        std::getline(file, password) &&
        std::getline(file, userID) &&
        std::getline(file, joiningDateStr)) {

        ++lineNumber;

        if (name.empty() || email.empty() || password.empty() || userID.empty() || joiningDateStr.empty()) {
            std::cout << "Skipped invalid record at line " << lineNumber << ".\n";
            continue;
        }

        alluser user = { name, email, password, userID, joiningDateStr };
        users[email] = user;
    }

    if (file.bad()) {
        std::cerr << "Error reading the file.\n";
    }
    else {
        std::cout << "Users loaded successfully.\n";
    }
}


void saveUserToFile(const alluser& user) {
    std::ofstream file("user_data.txt", std::ios::app);
    if (!file) {
        std::cerr << "Error saving user data to file. Please check file permissions or disk space.\n";
        return;
    }

    file << user.name << '\n'
        << user.email << '\n'
        << user.password << '\n'
        << user.userID << '\n'
        << user.joiningDate << '\n';

    std::cout << "User data saved to file.\n";
}





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

bool loginUser() {
    std::string email, password;
    std::cout << "\nLogin to your account:\n";
    std::cout << "Enter user email: ";
    std::getline(std::cin, email);
    std::cout << "Enter user password: ";
    std::getline(std::cin, password);

    auto it = users.find(email);
    if (it != users.end() && it->second.password == password) {
        currentUserEmail = email; 
        std::cout << "Login successful!\n";
        return true;
    }

    std::cout << "Invalid email or password.\n";
    return false;
}


void startBrowsing(const std::string& userID) {
    time_t startTime = time(nullptr);
    users[userID].sessions.push_back({ startTime, 0 });
    std::cout << "Browsing session started for user: " << users[userID].name << "\n";
}


void endBrowsing(const std::string& userID) {
    time_t endTime = time(nullptr);
    auto& session = users[userID].sessions.back();
    session.second = endTime;

    double duration = std::difftime(endTime, session.first) / 60;
    std::cout << "Browsing session ended for user: " << users[userID].name << ", Duration: " << duration << " minutes\n";

    double browsingCost = duration * 0.50;
    std::cout << "Total Browsing Cost: " << browsingCost << std::endl;
}

void autodeleteUser(std::string email) {
    
    if (users.find(email) == users.end()) {
        std::cout << "User with the email \"" << email << "\" has not been found.\n";
        return;
    }

    users.erase(email);
    std::ofstream file("user_data.txt");
    if (!file) {
        std::cout << "File could not load. Please try again.\n";
        return;
    }

    for (const auto& entry : users) {
        const alluser& user = entry.second;
        file << user.name << "\n"
            << user.email << "\n"
            << user.password << "\n"
            << user.userID << "\n"
            << user.joiningDate << "\n";
    }

    file.close();
    std::cout << "User with email " << email << " has been repaced \n";
    adminMenu();

}




void deleteUser() {
    std::string email;
    std::cout << "Type the email of the user you want to delete: ";
    std::getline(std::cin, email);

    if (users.find(email) == users.end()) {
        std::cout << "User with the email \"" << email << "\" has not been found.\n";
        return; 
    }

    users.erase(email);
    std::ofstream file("user_data.txt");
    if (!file) {
        std::cout << "File could not load. Please try again.\n";
        return; 
    }

    for (const auto& entry : users) {
        const alluser& user = entry.second;
        file << user.name << "\n"
            << user.email << "\n"
            << user.password << "\n"
            << user.userID << "\n"
            << user.joiningDate << "\n";
    }

    file.close();
    std::cout << "User with email " << email << " has been deleted from the file.\n";
    adminMenu();

}


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

void editUser() {
    std::string email;
    std::cout << "Enter the email of the user you want to edit: " << std::endl;
    std::getline(std::cin, email);

    auto it = users.find(email);
    if (it == users.end()) {
        std::cout << "User with email \"" << email << "\" not found.\n";
        return;
    }

    alluser user = it->second; // Work with a copy to avoid invalid references

    int choice{};
    std::cout << "\nEditing user: " << user.name << " (" << user.email << ")\n";
    std::cout << "1) Name" << std::endl;
    std::cout << "2) Email" << std::endl;
    std::cout << "3) Password" << std::endl;
    std::cout << "4) Cancel" << std::endl;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
    case 1: {
        std::cout << "Enter new name: " << std::endl;
        std::getline(std::cin, user.name);
        break;
    }
    case 2: {
        std::string newEmail;
        std::cout << "Enter new email: " << std::endl;
        std::getline(std::cin, newEmail);

        if (users.find(newEmail) != users.end()) {
            std::cout << "Email \"" << newEmail << "\" is already in use by another user.\n";
            return;
        }

        users.erase(email);
        autodeleteUser(email);
        user.email = newEmail;
        email = newEmail;
        break;
    }
    case 3: {
        std::cout << "Enter new password: " << std::endl;
        std::getline(std::cin, user.password);
        break;
    }
    case 4:
        std::cout << "Edit cancelled.\n";
        return;
    default:
        std::cout << "Invalid choice.\n";
        return;
    }

    users[email] = user; // Save updated user back into the map
    saveUserToFile(user); // Persist changes
    std::cout << "User information successfully updated.\n";
    adminMenu();
}


void viewBills(std::string email) {
    

    std::ifstream billFile("bill.txt");
    if (!billFile) {
        std::cout << "Could not open bill file.\n";
        return;
    }

    bool foundBills = false;
    std::string line;
    while (std::getline(billFile, line)) {
        
        if (line.find(email) != std::string::npos) {
            foundBills = true;
            
            std::cout << "------------------------------------------\n";
            
            std::cout << "Bill found for user with email: " << email << "\n";
            std::cout << line << std::endl;
            while (std::getline(billFile, line) && !line.empty()) {
                std::cout << line << std::endl; 
            }
            std::cout << "------------------------------------------\n";
        }
    }

    if (!foundBills) {
        std::cout << "No bills found for the user with email: " << email << "\n";
    }
    billFile.close();
    mainMenu(); 
}











void viewUserBills() {
    std::string email;
    std::cout << "Enter the email of the user whose bills you want to view: ";
    std::getline(std::cin, email);

    std::ifstream billFile("bill.txt");
    if (!billFile) {
        std::cout << "Could not open bill file.\n";
        return;
    }

    bool foundBills = false;
    std::string line;
    while (std::getline(billFile, line)) {
      
        if (line.find(email) != std::string::npos) {
            foundBills = true;
            std::cout << "Bill found for user with email: " << email << "\n";
            std::cout << line << std::endl;  
            while (std::getline(billFile, line) && !line.empty()) {
                std::cout << line << std::endl;  // Print all lines of the bill
            }
            std::cout << "------------------------------------------\n";
        }
    }

    if (!foundBills) {
        std::cout << "No bills found for the user with email: " << email << "\n";
    }
    billFile.close();
    adminMenu();
}


void adminMenu() {
    int choice;
    std::cout << "ADMIN MENU\n";
    std::cout << "1) View all users\n";
    std::cout << "2) Delete a user\n";
    std::cout << "3) Edit a user\n";
    std::cout << "4) View bills\n";
    std::cout << "5) Exit\n";
    std::cin >> choice;
    std::cin.ignore();

    switch (choice) {
    case 1:
        viewAllUsers();
        break;
    case 2:
        deleteUser();
        break;
    case 3:
        editUser();
        break;
    case 4: viewUserBills();
        break;
    case 5:
        std::cout << "Exiting admin menu...\n";
        mainMenu();
        break;
    default:
        std::cout << "Invalid option, please try again.\n";
        break;
    }
}

bool loginAdmin() {
    std::string email, password;
    std::cout << "\nLogin as Admin:\n";
    std::cout << "Enter admin email: ";
    std::getline(std::cin, email);
    std::cout << "Enter admin password: ";
    std::getline(std::cin, password);

    std::ifstream adminFile("admin.txt");
    if (!adminFile) {
        std::cout << "Admin credentials file not found.\n";
        return false;
    }

    std::string storedEmail, storedPassword;
    std::getline(adminFile, storedEmail);
    std::getline(adminFile, storedPassword);

    if (email == storedEmail && password == storedPassword) {
        std::cout << "Admin login successful!\n";
        adminMenu();
        return true;
    }

    std::cout << "Invalid admin email or password.\n";
    return false;
}

void viewBillForLoggedInUser() {
    std::string email = currentUserEmail;
    std::cout << "Enter the user's email to view their bill: ";
    std::getline(std::cin, email);
    viewUserBills();
}

double Tcost; 


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

void processPrintAndScan(const std::string& userID) {
    int printPages, scanPages;

   
    std::cout << "Enter number of pages printed: ";
    std::cin >> printPages;
    std::cout << "Enter number of pages scanned: ";
    std::cin >> scanPages;

    
    double printCost = printPages * printCostPerPage;
    double scanCost = scanPages * scanCostPerPage;

    // Update total costs for the user
    users[userID].totalPrintCost += printCost;
    users[userID].totalScanCost += scanCost;

    // Display the costs
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Print Cost: NZD " << printCost << "\n";
    std::cout << "Total Scan Cost: NZD " << scanCost << "\n";
    std::cout << "Total Amount Payable: NZD " << (printCost + scanCost) << "\n";
}



std::string getCurrentLoggedInUserEmail() {
    return currentUserEmail;
}

void mainMenu() {
    int mainChoice;
    bool k = true;
    while (k) {
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        std::vector<std::string> titleArt = {
            "$$$$$$\\  $$\\   $$\\ $$\\     $$\\ $$\\       $$$$$$\\ $$\\   $$\\ $$$$$$$$\\                           $$\\                                  $$$$$$\\   $$$$$$\\  $$$$$$$$\\ $$$$$$$$\\ ",
            "$$  __$$\\ $$ | $$  |\\$$\\   $$  |$$ |      \\_$$  _|$$$\\  $$ |$$  _____|                          $$ |                                $$  __$$\\ $$  __$$\\ $$  _____|$$  _____|",
            "$$ /  \\__|$$ |$$  /  \\$$\\ $$  / $$ |        $$ |  $$$$\\ $$ |$$ |             $$$$$$$\\ $$\\   $$\\ $$$$$$$\\   $$$$$$\\   $$$$$$\\        $$ /  \\__|$$ /  $$ |$$ |      $$ |      ",
            "\\$$$$$$\\  $$$$$  /    \\$$$$  /  $$ |        $$ |  $$ $$\\$$ |$$$$$\\          $$  _____|$$ |  $$ |$$  __$$\\ $$  __$$\\ $$  __$$\\       $$ |      $$$$$$$$ |$$$$$\\    $$$$$\\    ",
            " \\____$$\\ $$  $$<      \\$$  /   $$ |        $$ |  $$ \\$$$$ |$$  __|         $$ /      $$ |  $$ |$$ |  $$ |$$$$$$$$ |$$ |  \\__|      $$ |      $$  __$$ |$$  __|   $$  __|   ",
            "$$\\   $$ |$$ |\\$$\\      $$ |    $$ |        $$ |  $$ |\\$$$ |$$ |            $$ |      $$ |  $$ |$$ |  $$ |$$   ____|$$ |            $$ |  $$\\ $$ |  $$ |$$ |      $$ |      ",
            "\\$$$$$$  |$$ | \\$$\\     $$ |    $$$$$$$$\\ $$$$$$\\ $$ | \\$$ |$$$$$$$$\\       \\$$$$$$$\\ \\$$$$$$$ |$$$$$$$  |\\$$$$$$$\\ $$ |            \\$$$$$$  |$$ |  $$ |$$ |      $$$$$$$$\\ ",
            " \\______/ \\__|  \\__|    \\__|    \\________|\\______|\\__|  \\__|\\________|       \\_______| \\____$$ |\\_______/  \\_______|\\__|             \\______/ \\__|  \\__|\\__|      \\________|",
            "                                                                                      $$\\   $$ |                                                                             ",
            "                                                                                      \\$$$$$$  |                                                                             ",
            "                                                                                       \\______/                                                                              "
        };

        for (const auto& line : titleArt) {
            std::cout << line << std::endl;
        };
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Login as Admin\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> mainChoice;
        std::cin.ignore();

        switch (mainChoice) {
        case 1:
            registerUser();
            break;
        case 2: {
            if (loginUser()) {
                std::string email;
                email = getCurrentLoggedInUserEmail();

                auto it = users.find(email);
                if (it != users.end()) {
                    alluser& user = it->second;
                    while (true) {
                        std::cout << "\nUser Menu\n";
                        std::cout << "1. Print - Scan\n";

                        std::cout << "2. Start browsing\n";
                        std::cout << "3. End browsing\n";
                        std::cout << "4. Logout\n";
                        std::cout << "Enter your choice: ";
                        int userChoice;
                        std::cin >> userChoice;
                        std::cin.ignore();
                        if (userChoice == 5) break;
                        switch (userChoice) {
                        case 1: processPrintAndScan(user.userID); break;
                        case 2: startBrowsing(user.userID); break;

                        case 3: endBrowsing(user.userID); break;

                        case 4: 
                            
                            viewBills(user.email);
                            
                            logoutUser(user.userID); 

                            
                            
                            
                            
                            break;

                        }
                    }
                }
            }
            break;
        }
        case 3:
            if (loginAdmin()) {
                while (true) {
                    std::cout << "\nAdmin Menu\n";
                    std::cout << "1. View all users\n";
                    std::cout << "2. Delete a user\n";
                    std::cout << "3. Edit a user\n";
                    std::cout << "4. Logout\n";
                    std::cout << "Enter your choice: ";
                    int adminChoice;
                    std::cin >> adminChoice;
                    std::cin.ignore();
                    if (adminChoice == 4) break;
                    switch (adminChoice) {
                    case 1: viewAllUsers(); break;
                    case 2: deleteUser(); break;
                    case 3: editUser(); break;
                    default: std::cout << "Invalid choice\n"; break;
                    }
                }

            }
            break;
        case 4:
            std::cout << "Exiting the program. Goodbye!\n";
            k = false;
            break;
        default:
            std::cout << "Invalid choice, please try again.\n";
            break;
        }




    }


    return;






}

