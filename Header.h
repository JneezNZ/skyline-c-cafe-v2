#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include <iomanip>
#include <regex>
#include <fstream>
#include <cstdlib>

struct alluser {
    std::string name;
    std::string email;
    std::string password;
    std::string userID;
    time_t joiningDate = 0;
    std::vector<std::pair<time_t, time_t>> sessions;
    double totalPrintCost = 0;
    double totalScanCost = 0;
};

std::string currentUserEmail;
extern std::unordered_map<std::string, alluser> users;
extern double printCostPerPage;
extern double scanCostPerPage;

const std::string filename = "users.txt";
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

void adminMenu();
void viewAllUsers();
void deleteUser();
void editUser();

std::unordered_map<std::string, alluser> users;
double printCostPerPage = 0.20;
double scanCostPerPage = 0.10;


void clearConsole() {
    system("cls");  // Clears the screen on Windows
}

std::string generateUserId() {
    return "User" + std::to_string(counter++);
}

void loadUsersFromFile() {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "No user data file found, starting fresh.\n";
        return;
    }

    std::string name, email, password, userID;
    time_t joiningDate;
    while (file >> std::ws && std::getline(file, name) &&
        std::getline(file, email) &&
        std::getline(file, password) &&
        std::getline(file, userID) &&
        (file >> joiningDate)) {
        alluser user = { name, email, password, userID, joiningDate };
        users[email] = user;
        file.ignore();
    }
}

void saveUserToFile(const alluser& user) {
    std::ofstream file(filename, std::ios::app);
    if (file) {
        file << user.name << "\n" << user.email << "\n" << user.password << "\n"
            << user.userID << "\n" << user.joiningDate << "\n"
        << "_____________________________________________________\n\n\n\n";
    }
    else {
        std::cout << "Error saving user data to file.\n";
    }
}
bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"((^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$))");
    if (std::regex_match(email, pattern)) {
        std::cout << "Email \"" << email << "\" is valid.\n";
        return true;
    }
    else {

        return false;
    }

    
}

bool isValidPassword(const std::string& password) {
    // Regular expression pattern for password validation
    const std::regex passwordPattern(R"((?=.*[A-Z])(?=.*[a-z])(?=.*\d)(?=.*[!@#$%^&*])[A-Za-z\d!@#$%^&*]{8,})");

    // Check if password matches the pattern
    if (std::regex_match(password, passwordPattern)) {
        std::cout << "Password \"" << password << "\" \n";
        return true;
    }
    else {
        std::cout << "Password is invalid. Requirements:\n";
        std::cout << "- At least 8 characters long\n";
        std::cout << "- At least one uppercase letter\n";
        std::cout << "- At least one lowercase letter\n";
        std::cout << "- At least one digit (0-9)\n";
        std::cout << "- At least one special character (!@#$%^&*)\n";
        return false;
    }

}

void registerUser() {
    std::string name, email, password;
    while(true){
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);
    std::regex nameRegex("^[A-Za-z\\s]+$"); 

    if (!std::regex_match(name, nameRegex)) {
        std::cout << "Invalid name. Please enter a name containing only alphabetic characters and spaces.\n";
        continue;
    }
    break;

    
    
}

    while (true) {
        std::cout << "Enter your email: ";
        std::getline(std::cin, email);

        // Check if the email is valid
        if (!isValidEmail(email)) {
            std::cout << "Invalid email format. Please enter a valid email.\n";
            continue;
        }

        
        if (users.find(email) != users.end()) {
            std::cout << "User with this email: \"" << email << "\" is already registered. Please type a different email.\n";
        }
        else {
            
            break; 
        }
    }
    while (true)
    {
        std::cout << "Enter your password: ";
        std::getline(std::cin, password);
        if (isValidPassword(password)) {
            break;
        }
    }
    alluser newUser = { name, email, password, generateUserId(), time(nullptr) };
    users[email] = newUser; // Add the user to the in-memory map
    saveUserToFile(newUser);
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
        currentUserEmail = email; // Store email of the logged-in user
        std::cout << "Login successful!\n";
        return true;
    }else{
        std::cout << "Invalid email or password.\n";
        return false;
    }
    
    
}

std::string getCurrentLoggedInUserEmail() {
    return currentUserEmail;
}

void startSession(const std::string& userID) {
    time_t startTime = time(nullptr);
    users[userID].sessions.push_back({ startTime, 0 });
    std::cout << "Session started for user: " << users[userID].name << "\n";
}
void processPrintAndScan(const std::string& userID) {
    int printPages, scanPages;

    // Input pages printed and scanned
    std::cout << "Enter number of pages printed: ";
    std::cin >> printPages;
    std::cout << "Enter number of pages scanned: ";
    std::cin >> scanPages;

    // Calculate costs
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


void endSession(const std::string& userID) {
    time_t endTime = time(nullptr);
    auto& session = users[userID].sessions.back();
    session.second = endTime;

    double duration = std::difftime(endTime, session.first) / 60;
    std::cout << "Session ended for user: " << users[userID].name << ", Duration: " << duration << " minutes\n";

    
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
    std::cout << "Session ended for user: " << users[userID].name << ", Duration: " << duration << " minutes\n";

    double browsingCost = duration * 0.50;
    std::cout << "Total Browsing Cost: " << browsingCost << std::endl;
}

void Billdoc(const std::string& email, double print, double scan, double internet, double debt) {
    std::ofstream file("bill.txt", std::ios::app);
    if (file.is_open()) {
        file << "Account: " << email << "\n"
            << "Print: $" << print << "\n"
            << "Scan: $" << scan << "\n"
            << "Internet: $" << internet << "\n\n"
            << "Total Outstanding: $" << debt << "\n"
            << "_____________________________________________________\n\n";
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
    }
}

void adminMenu() {
    int choice;
    std::cout << "ADMIN MENU\n";
    std::cout << "1) View all users\n";
    std::cout << "2) Delete a user\n";
    std::cout << "3) Edit a user\n";
    std::cout << "4) Exit\n";
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
    case 4:
        std::cout << "Exiting admin menu...\n";
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

void viewAllUsers() {
    for (const auto& i : users) {
        std::cout << "Name: " << i.second.name << std::endl;
        std::cout << "Email: " << i.second.email << std::endl;
        std::cout << "UserID: " << i.second.userID << std::endl;
        std::cout << "-------------------------------\n";
    }
}

void deleteUser() {
    std::string email;
    std::cout << "Enter email of user to delete: ";
    std::getline(std::cin, email);

    if (users.find(email) != users.end()) {
        users.erase(email);
        std::cout << "User deleted successfully.\n";
    }
    else {
        std::cout << "User not found.\n";
    }
}

void editUser() {
    std::string email;
    std::cout << "Enter the email of the user you want to edit: ";
    std::getline(std::cin, email);

    if (users.find(email) == users.end()) {
        std::cout << "User with email \"" << email << "\" not found.\n";
        return;
    }

    alluser& user = users[email];
    int choice = 0;

    std::cout << "\nEditing user: " << user.name << " (" << user.email << ")\n";
    std::cout << "1) Name\n";
    std::cout << "2) Email\n";
    std::cout << "3) Password\n";
    std::cout << "4) Cancel\n";

    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (choice) {
    case 1: {
        std::cout << "Enter new name: ";
        std::getline(std::cin, user.name);
        std::cout << "Name has been successfully updated.\n";
        adminMenu();
        break;
    }
    case 2: {
        std::string newEmail;
        std::cout << "Enter new email: ";
        std::getline(std::cin, newEmail);

        if (users.find(newEmail) != users.end()) {
            std::cout << "Email \"" << newEmail << "\" is already in use by another user.\n";
            return;
        }
        user.email = newEmail;
        users[newEmail] = user;
        users.erase(email);
        std::cout << "Email updated successfully.\n";
        adminMenu();
        break;
    }
    case 3: {
        std::cout << "Enter new password: ";
        std::getline(std::cin, user.password);
        std::cout << "Password has been successfully updated.\n";
        adminMenu();
        break;
    }
    case 4:
        std::cout << "Edit cancelled.\n";
        adminMenu();
        return;
    default:
        std::cout << "Invalid choice.\n";
        adminMenu();
        return;
    }

    // Save the updated user details back to the file
    std::ofstream file("users.txt");
    if (!file) {
        std::cout << "Error loading the file.\n";
        return;
    }

    for (const auto& entry : users) {
        const alluser& user = entry.second;
        file << user.name << std::endl
            << user.email << std::endl
            << user.password << std::endl
            << user.userID << std::endl
            << user.joiningDate << std::endl;
    }
    file.close();

    std::cout << "User details updated successfully in the file.\n";
}


double Tcost; 




void logoutUser(const std::string& userID) {
   
    double printCost = users[userID].totalPrintCost;
    double scanCost = users[userID].totalScanCost;
    double internetCost = Tcost; 
    double totalDebt = printCost + scanCost + internetCost;

    
    Billdoc(users[userID].name, printCost, scanCost, internetCost, totalDebt);

    
    std::cout << "Logging out...\n";
    mainMenu();




    

   
     
}



void mainMenu() {
    int mainChoice;
    while (true) {
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

                        case 4: logoutUser(user.userID); break;

                        }
                    }
                }
            }

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
            return;
        default:
            std::cout << "Invalid choice, please try again.\n";
            break;
        }
    
    
    
    
    }


    return;






}









