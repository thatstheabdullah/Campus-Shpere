#include <string>

using namespace std;

const int MAX_USERS = 500;


class User {
public:
    User()
        : username(""), password(""), email(""),
        profileName(""), bio(""), isLoggedIn(false) {
    }

    User(const string& uname,
        const string& pwd,
        const string& mail)
        : username(uname), password(pwd), email(mail),
        profileName(uname), bio(""), isLoggedIn(false) {
    }

    // Getters
    string getUsername()    const { return username; }
    string getPassword()    const { return password; }
    string getEmail()       const { return email; }
    string getprofileName() const { return profileName; }
    string getBio()         const { return bio; }
    bool   getIsLoggedIn()  const { return isLoggedIn; }

    // Setters
    void setUsername(const string& v) { username = v; }
    void setPassword(const string& v) { password = v; }
    void setEmail(const string& v) { email = v; }
    void setprofileName(const string& v) { profileName = v; }
    void setBio(const string& v) { bio = v; }
    void setIsLoggedIn(bool v) { isLoggedIn = v; }

private:
    string username;
    string password;
    string email;
    string profileName;
    string bio;
    bool   isLoggedIn;
};

class UserManager {
public:
    explicit UserManager(const string& dataFile = "users.txt");
    ~UserManager();

   
    bool signUp(const string& username,
        const string& password,
        const string& email);

    bool login(const string& username,
        const string& password);

    bool deleteAccount(const string& username,
        const string& password);

   
    string issueResetPin(const string& username, const string& email);

    
    bool resetPassword(const string& username,
        const string& enteredPin,
        const string& newPassword);

    
    bool createProfile(const string& username,
        const string& displayName,
        const string& bio);

    bool editProfile(const string& username,
        const string& newDisplayName,
        const string& newBio);

    
    void searchByName(const string& query, User results[], int& count) const;

    // Utilities
    bool  logout(const string& username);
    bool  isLoggedIn(const string& username) const;
    User* getUser(const string& username);
    void  printProfile(const string& username) const;

private:
    User   users_[MAX_USERS];   
    int    userCount_;
    string dataFile_;

  
    string pendingPinUser_;
    string pendingPin_;

    int  findIndex(const string& username) const;  
    void saveToFile() const;
    void loadFromFile();

    static string toLower(const string& s);
    static bool   isValidEmail(const string& email);
    static bool   isValidUsername(const string& username);
};

