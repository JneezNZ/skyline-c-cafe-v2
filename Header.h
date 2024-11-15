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

const std::string filename = "users.txt";
int counter = 0;

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

std::string generateUserId() {
    return "user" + std::to_string(counter++);
}

// **** //
std::string formatJoiningDate(time_t date) {
    char buffer[80];
    struct tm* timeinfo = localtime(&date);
    strftime(buffer, sizeof(buffer), "%A, %d %B %Y  %I:%M:%S %p", timeinfo);  // Format as desired
    return std::string(buffer);
}

void loadUsersFromFile() {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "No user data file found, starting fresh.\n";
        return;
    }

    std::string name, email, password, userID, joiningDateStr;

    //*****//
    //time_t joiningDate;

    while (file >> std::ws && std::getline(file, name) &&
        std::getline(file, email) &&
        std::getline(file, password) &&
        std::getline(file, userID) &&

        //****//
        std::getline(file, joiningDateStr)) {

        //(file >> joiningDate)) {
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
time_t getCurrentDateAsTimeT() {
    // Get the current time as `time_t`
    time_t now = time(nullptr);

    // Use `tm` structure for local time
    struct tm localTime;

    // Use `localtime_s` to safely populate `localTime` with the current local time
    if (localtime_s(&localTime, &now) != 0) {
        std::cerr << "Failed to retrieve local time.\n";
        return -1; // Return an error value if localtime_s fails
    }

    // Zero out hour, minute, and second to get midnight of the current date
    localTime.tm_hour = 0;
    localTime.tm_min = 0;
    localTime.tm_sec = 0;

    // Convert back to `time_t` representing midnight of the current date
    return mktime(&localTime);
}

void registerUser() {
    alluser user;
    std::cout << "Enter your name: ";
    std::getline(std::cin, user.name);

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

    user.userID = users.size() + 1; // Assign a new user ID
    //user.joiningDate = getCurrentDateAsTimeT(); // Example joining date

    //*******//
    time_t currentTime = time(nullptr);
    std::string formattedJoiningDate = formatJoiningDate(currentTime);
    user.joiningDate = formattedJoiningDate;


    users[user.email] = user;
    saveUserToFile(user);
    std::cout << "User joined on: " << formatJoiningDate(currentTime) << std::endl;


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
        std::cout << "User joined on: " << it->second.joiningDate << std::endl;
        return true;
    }

    std::cout << "Invalid email or password.\n";
    return false;
}

std::string getCurrentLoggedInUserEmail() {
    return currentUserEmail;
}

void startSession(const std::string& userID) {
    time_t joiningDate = time(nullptr);
    users[userID].sessions.push_back({ joiningDate, 0 });
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
    std::cout << "Browsing session ended for user: " << users[userID].name << ", Duration: " << duration << " minutes\n";

    double browsingCost = duration * 0.50;
    std::cout << "Total Browsing Cost: " << browsingCost << std::endl;
}

void Billdoc(const std::string& userID, double print, double scan, double internet, double debt) {
    std::ofstream file("bill.txt", std::ios::app);
    if (file.is_open()) {
        file << "Account: " << users[userID].email << "\n"
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
    std::cout << "Enter your choice: ";
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
    if (users.empty()) {
        std::cout << "No users found in the system.\n";
        return;
    }


    std::cout << "Displaying all users:\n";
    std::cout << "---------------------------------------------------------------\n";

    for (const auto& i : users) {
        const auto& user = i.second;
        std::cout << "Name: " << user.name << "\n" << "Email: " << user.email << "\n" << ", User ID: " << user.userID << "\n" << user.joiningDate << "\n";
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
    std::cout << "Enter email of user to edit: ";
    std::getline(std::cin, email);

    auto it = users.find(email);
    if (it != users.end()) {
        auto& user = it->second;
        std::cout << "Editing user: " << user.name << "\n";
        // Implement additional user editing functionality here.
    }
    else {
        std::cout << "User not found.\n";
    }
}

double Tcost;


void logoutUser(const std::string& userID) {

    double printCost = users[userID].totalPrintCost;
    double scanCost = users[userID].totalScanCost;
    double internetCost = Tcost;
    double totalDebt = printCost + scanCost + internetCost;
    const std::string& UID = users[userID].name;



    Billdoc(UID, printCost, scanCost, internetCost, totalDebt);


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
            break;
        default:
            std::cout << "Invalid choice, please try again.\n";
            break;
        }




    }


    return;

}









