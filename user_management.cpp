#include "user_management.h"
#include <fstream>
#include <iostream>

UserManager::UserManager(const string& dataFile)
    : userCount_(0), dataFile_(dataFile),
    pendingPinUser_(""), pendingPin_("") {
    loadFromFile();
}

UserManager::~UserManager() {
    saveToFile();
}

int UserManager::findIndex(const string& username) const {
    for (int i = 0; i < userCount_; i++) {
        if (users_[i].getUsername() == username)
            return i;
    }
    return -1;
}

bool UserManager::signUp(const string& username,
    const string& password,
    const string& email) {
    if (!isValidUsername(username)) {
        cout << "[Error] Username must be 3-20 chars, letters/digits/underscore only.\n";
        return false;
    }
    if (password.size() < 6) {
        cout << "[Error] Password must be at least 6 characters.\n";
        return false;
    }
    if (!isValidEmail(email)) {
        cout << "[Error] Invalid email format.\n";
        return false;
    }
    if (findIndex(username) != -1) {
        cout << "[Error] Username \"" << username << "\" is already taken.\n";
        return false;
    }
    if (userCount_ >= MAX_USERS) {
        cout << "[Error] User limit reached.\n";
        return false;
    }

    users_[userCount_] = User(username, password, email);
    userCount_++;
    saveToFile();
    cout << "[Success] Account created for \"" << username << "\".\n";
    return true;
}

bool UserManager::login(const string& username,
    const string& password) {
    int i = findIndex(username);
    if (i == -1) {
        cout << "[Error] User \"" << username << "\" not found.\n";
        return false;
    }
    if (users_[i].getPassword() != password) {
        cout << "[Error] Incorrect password.\n";
        return false;
    }
    users_[i].setIsLoggedIn(true);
    cout << "[Success] Welcome back, " << users_[i].getprofileName() << "!\n";
    return true;
}

bool UserManager::deleteAccount(const string& username,
    const string& password) {
    int i = findIndex(username);
    if (i == -1) {
        cout << "[Error] User \"" << username << "\" not found.\n";
        return false;
    }
    if (users_[i].getPassword() != password) {
        cout << "[Error] Incorrect password. Cannot delete account.\n";
        return false;
    }

    for (int j = i; j < userCount_ - 1; j++) {
        users_[j] = users_[j + 1];
    }
    userCount_--;
    saveToFile();
    cout << "[Success] Account \"" << username << "\" has been deleted.\n";
    return true;
}

//manual pin generation
static string generatePin() {
    static unsigned long seed = (unsigned long)time(nullptr);
    const char charset[] = "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    const int charsetSize = 62;
    const int pinLength = 8;

    string pin = "";
    for (int i = 0; i < pinLength; i++) {
        seed = seed * 1664525UL + 1013904223UL;
        pin += charset[seed % charsetSize];
    }
    return pin;
}
string UserManager::issueResetPin(const string& username, const string& email) {
    int i = findIndex(username);
    if (i == -1) {
        cout << "[Error] User \"" << username << "\" not found.\n";
        return "";
    }
    if (users_[i].getEmail() != email) {
        cout << "[Error] Username and email do not match any account.\n";
        return "";
    }
    pendingPinUser_ = username;
    pendingPin_ = generatePin();
    return pendingPin_;
}

bool UserManager::resetPassword(const string& username,
    const string& enteredPin,
    const string& newPassword) {
    if (username != pendingPinUser_ || pendingPin_.empty()) {
        cout << "[Error] No active reset PIN for this user.\n";
        return false;
    }
    if (enteredPin != pendingPin_) {
        cout << "[Error] Incorrect PIN. Password reset cancelled.\n";
        pendingPin_.clear();
        pendingPinUser_.clear();
        return false;
    }
    if (newPassword.size() < 6) {
        cout << "[Error] Password must be at least 6 characters.\n";
        return false;
    }
    int i = findIndex(username);
    if (i == -1) return false;
    users_[i].setPassword(newPassword);
    pendingPin_.clear();
    pendingPinUser_.clear();
    saveToFile();
    cout << "[Success] Password has been reset for \"" << username << "\".\n";
    return true;
}

bool UserManager::createProfile(const string& username,
    const string& displayName,
    const string& bio) {
    int i = findIndex(username);
    if (i == -1) {
        cout << "[Error] User \"" << username << "\" not found.\n";
        return false;
    }
    users_[i].setprofileName(displayName.empty() ? username : displayName);
    users_[i].setBio(bio);
    saveToFile();
    cout << "[Success] Profile created for \"" << username << "\".\n";
    return true;
}

bool UserManager::editProfile(const string& username,
    const string& newDisplayName,
    const string& newBio) {
    int i = findIndex(username);
    if (i == -1) {
        cout << "[Error] User \"" << username << "\" not found.\n";
        return false;
    }
    if (!newDisplayName.empty()) users_[i].setprofileName(newDisplayName);
    if (!newBio.empty())         users_[i].setBio(newBio);
    saveToFile();
    cout << "[Success] Profile updated for \"" << username << "\".\n";
    return true;
}

void UserManager::searchByName(const string& query, User results[], int& count) const {
    string lQuery = toLower(query);
    count = 0;

    for (int i = 0; i < userCount_; i++) {
        if (toLower(users_[i].getUsername()).find(lQuery) != string::npos ||
            toLower(users_[i].getprofileName()).find(lQuery) != string::npos) {
            results[count] = users_[i];
            count++;
        }
    }

    // Bubble sort results by displayName
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (results[j].getprofileName() > results[j + 1].getprofileName()) {
                User temp = results[j];
                results[j] = results[j + 1];
                results[j + 1] = temp;
            }
        }
    }
}

bool UserManager::logout(const string& username) {
    int i = findIndex(username);
    if (i == -1) return false;
    users_[i].setIsLoggedIn(false);
    cout << "[Success] \"" << username << "\" has been logged out.\n";
    return true;
}

bool UserManager::isLoggedIn(const string& username) const {
    int i = findIndex(username);
    return (i != -1) && users_[i].getIsLoggedIn();
}

User* UserManager::getUser(const string& username) {
    int i = findIndex(username);
    return (i != -1) ? &users_[i] : nullptr;
}

void UserManager::printProfile(const string& username) const {
    int i = findIndex(username);
    if (i == -1) { cout << "[Error] User not found.\n"; return; }

    cout << "\n    Profile                    \n";
    cout << " Username    : " << users_[i].getUsername() << "\n";
    cout << " Display Name: " << users_[i].getprofileName() << "\n";
    cout << " Email       : " << users_[i].getEmail() << "\n";
    cout << " Bio         : " << (users_[i].getBio().empty() ? "(no bio)" : users_[i].getBio()) << "\n";
    cout << " Status      : " << (users_[i].getIsLoggedIn() ? "Online" : "Offline") << "\n";
    cout << endl;
}

static string escapeField(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '|')  out += "\\pipe";
        else if (s[i] == '\n') out += "\\n";
        else                   out += s[i];
    }
    return out;
}

static string unescapeField(const string& s) {
    string out;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            if (s.substr(i, 5) == "\\pipe") { out += '|';  i += 4; continue; }
            if (s.substr(i, 2) == "\\n") { out += '\n'; i += 1; continue; }
        }
        out += s[i];
    }
    return out;
}

void UserManager::saveToFile() const {
    ofstream ofs(dataFile_);
    if (!ofs) { cerr << "[Warning] Cannot write to \"" << dataFile_ << "\".\n"; return; }
    for (int i = 0; i < userCount_; i++) {
        ofs << escapeField(users_[i].getUsername()) << "|"
            << escapeField(users_[i].getPassword()) << "|"
            << escapeField(users_[i].getEmail()) << "|"
            << escapeField(users_[i].getprofileName()) << "|"
            << escapeField(users_[i].getBio()) << "\n";
    }
}

void UserManager::loadFromFile() {
    ifstream ifs(dataFile_);
    if (!ifs) return;
    string line;
    while (getline(ifs, line) && userCount_ < MAX_USERS) {
        if (line.empty()) continue;

        // Split line into 5 fields by '|' manually
        string f[5];
        int    field = 0;
        size_t start = 0;
        for (size_t i = 0; i <= line.size() && field < 5; i++) {
            if (i == line.size() || line[i] == '|') {
                f[field] = line.substr(start, i - start);
                field++;
                start = i + 1;
            }
        }

        User u;
        u.setUsername(unescapeField(f[0]));
        u.setPassword(unescapeField(f[1]));
        u.setEmail(unescapeField(f[2]));
        u.setprofileName(unescapeField(f[3]));
        u.setBio(unescapeField(f[4]));
        u.setIsLoggedIn(false);
        if (!u.getUsername().empty()) {
            users_[userCount_] = u;
            userCount_++;
        }
    }
}
//helpers
string UserManager::toLower(const string& s) {
    string out = s;
    for (size_t i = 0; i < out.size(); i++) {
        if (out[i] >= 'A' && out[i] <= 'Z')
            out[i] = out[i] + ('a' - 'A');
    }
    return out;
}

bool UserManager::isValidEmail(const string& email) {
    size_t at = email.find('@');
    size_t dot = email.rfind('.');
    return at != string::npos &&
        dot != string::npos &&
        dot > at + 1 &&
        dot < email.size() - 1;
}

bool UserManager::isValidUsername(const string& u) {
    if (u.size() < 3 || u.size() > 20) return false;
    for (size_t i = 0; i < u.size(); i++) {
        char c = u[i];
        bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        bool isDigit = (c >= '0' && c <= '9');
        if (!isLetter && !isDigit && c != '_') return false;
    }
    return true;
}
static string prompt(const string& msg, bool maskInput = false) {
    cout << msg;
    string input;
    if (maskInput) {
        char c;
        while (cin.get(c) && c != '\n') { input += c; cout << '*'; }
        cout << '\n';
    }
    else {
        getline(cin, input);
    }
    return input;
}

static int readChoice() {
    string line;
    getline(cin, line);

    if (line.empty()) return -1;

    size_t start = 0;
    if (line[0] == '-') start = 1;
    if (start == line.size()) return -1; 

    for (size_t i = start; i < line.size(); i++) {
        if (line[i] < '0' || line[i] > '9') return -1;
    }

    // Convert manually to int
    int result = 0;
    for (size_t i = start; i < line.size(); i++) {
        result = result * 10 + (line[i] - '0');
    }
    return (line[0] == '-') ? -result : result;
}

static void printMenu(const string& loggedInUser) {
    cout << "     Social Media - User Management   \n";
    if (!loggedInUser.empty())
        cout << "  Logged in as: " << loggedInUser
        << string(22 - loggedInUser.size(), ' ') << "\n";
    cout << "  1. Sign Up                          \n";
    cout << "  2. Login                            \n";
    cout << "  3. Delete Account                   \n";
    cout << "  4. Create Profile                   \n";
    cout << "  5. Edit Profile                     \n";
    cout << "  6. Search Users by Name             \n";
    cout << "  7. View My Profile                  \n";
    cout << "  8. Logout                           \n";
    cout << "  9. Forgot Password                  \n";
    cout << "  0. Exit                             \n";
    cout << "  Choice: ";
}

int main() {
    UserManager um("users.txt");
    string loggedInUser;
    int    choice = -1;

    while (choice != 0) {
        printMenu(loggedInUser);
        choice = readChoice();

        if (choice == -1) {
            cout << "[Error] Invalid input. Please enter a number from the menu.\n";
            continue;
        }

        switch (choice) {

        case 1: {   // Sign Up
            cout << "    Sign Up      "<<endl;
            string username = prompt("  Username : ");
            string password = prompt("  Password : ", true);
            string email = prompt("  Email    : ");
            um.signUp(username, password, email);
            break;
        }

        case 2: {   // Login
            cout << "     Login      "<<endl;
            string username = prompt("  Username : ");
            string password = prompt("  Password : ", true);
            if (um.login(username, password)) loggedInUser = username;
            break;
        }

        case 3: {   // Delete Account
            cout << "     Delete Account      "<<endl;
            string username = prompt("  Username         : ");
            string password = prompt("  Confirm password : ", true);
            if (um.deleteAccount(username, password))
                if (loggedInUser == username) loggedInUser.clear();
            break;
        }

        case 4: {   // Create Profile
            if (loggedInUser.empty()) { cout << "[Error] Please log in first.\n"; break; }
            cout << "     Create Profile      "<<endl;

            // Show all signed-up usernames to pick from
            cout << "  Signed-up users:\n";
            User  tempResults[MAX_USERS];
            int   tempCount = 0;
            um.searchByName("", tempResults, tempCount);
            for (int i = 0; i < tempCount; i++)
                cout << "  " << (i + 1) << ". " << tempResults[i].getUsername() << "\n";

            // Ask which user to create profile for
            cout << "  Select user number (1-" << tempCount << "): ";
            int pick = readChoice();
            if (pick < 1 || pick > tempCount) {
                cout << "[Error] Invalid selection.\n"; break;
            }
            string targetUser = tempResults[pick - 1].getUsername();

            // Display name is mandatory
            string displayName = "";
            while (displayName.empty()) {
                displayName = prompt("  Display name (required): ");
                if (displayName.empty())
                    cout << "[Error] Display name cannot be empty.\n";
            }
            string bio = prompt("  Bio : ");
            um.createProfile(targetUser, displayName, bio);
            break;
        }

        case 5: {   // Edit Profile
            if (loggedInUser.empty()) { cout << "[Error] Please log in first.\n"; break; }
            cout << "     Edit Profile      "<<endl;

            // Show all profiles with their display names
            User  editList[MAX_USERS];
            int   editCount = 0;
            um.searchByName("", editList, editCount);

            if (editCount == 0) { cout << "[Error] No profiles found.\n"; break; }

            cout << "  Profiles:\n";
            for (int i = 0; i < editCount; i++)
                cout << "  " << (i + 1) << ". @" << editList[i].getUsername()
                << "  ->  " << editList[i].getprofileName() << "\n";

            cout << "  Select profile number (1-" << editCount << "): ";
            int epick = readChoice();
            if (epick < 1 || epick > editCount) {
                cout << "[Error] Invalid selection.\n"; break;
            }
            string editTarget = editList[epick - 1].getUsername();

            // Both fields are mandatory
            string newDisplayName = "";
            while (newDisplayName.empty()) {
                newDisplayName = prompt("  New display name (required): ");
                if (newDisplayName.empty())
                    cout << "[Error] Display name cannot be empty.\n";
            }
            string newBio = "";
            while (newBio.empty()) {
                newBio = prompt("  New bio (required): ");
                if (newBio.empty())
                    cout << "[Error] Bio cannot be empty.\n";
            }
            um.editProfile(editTarget, newDisplayName, newBio);
            break;
        }

        case 6: {   // Search
            cout << "     Search Users      "<<endl;
            string query = prompt("  Enter name to search : ");
            User   results[MAX_USERS];
            int    count = 0;
            um.searchByName(query, results, count);
            if (count == 0) {
                cout << "  No users found matching \"" << query << "\".\n";
            }
            else {
                cout << "  Found " << count << " result(s):\n\n";
                for (int i = 0; i < count; i++) {
                    cout << "   @" << results[i].getUsername() << "\n";
                    cout << "   Name : " << results[i].getprofileName() << "\n";
                    cout << "   Bio  : " << (results[i].getBio().empty() ? "(no bio)" : results[i].getBio()) << "\n";
                }
            }
            break;
        }

        case 7:   // View Profile
            um.printProfile(loggedInUser);
            break;

        case 8:   // Logout
            if (!loggedInUser.empty()) {
                um.logout(loggedInUser);
                loggedInUser.clear();
            }
            else {
                cout << "[Info] No user is currently logged in.\n";
            }
            break;

        case 9: {   // Forgot Password
            cout << "     Forgot Password      "<<endl;
            string fpUsername = prompt("  Username : ");
            string fpEmail = prompt("  Email    : ");

            // Verify username + email and issue PIN
            string pin = um.issueResetPin(fpUsername, fpEmail);
            if (pin.empty()) break;

            // Show the generated PIN
            cout << "    Your reset PIN : " << pin << "           \n";
            cout << "    Enter this PIN below to continue     \n";

            // User must type the PIN back
            string enteredPin = prompt("  Enter PIN : ");

            // New password — mandatory, min 6 chars
            string newPassword = "";
            while (newPassword.empty()) {
                newPassword = prompt("  New password         : ", true);
                if (newPassword.size() < 6) {
                    cout << "[Error] Password must be at least 6 characters.\n";
                    newPassword = "";
                }
            }
            string confirmPassword = prompt("  Confirm new password : ", true);
            if (newPassword != confirmPassword) {
                cout << "[Error] Passwords do not match. Reset cancelled.\n";
                break;
            }

            um.resetPassword(fpUsername, enteredPin, newPassword);
            break;
        }

        case 0:
            cout << "Goodbye!\n";
            break;

        default:
            cout << "[Error] Invalid option. Please enter a number between 0-9.\n";
        }
    }
    return 0;
}