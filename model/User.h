#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  Class: User
//  OOP Concept: Encapsulation
//  All fields are PRIVATE — accessed only
//  through public getters and setters.
// ─────────────────────────────────────────────
class User {
private:
    int    id;
    string username;
    string passwordHash;
    string email;
    string bio;
    string joinedAt;
    bool   isActive;

public:
    User() : id(0), isActive(true) {}
    User(int id, const string& username, const string& email,
         const string& passwordHash, const string& bio = "")
        : id(id), username(username), email(email),
          passwordHash(passwordHash), bio(bio), isActive(true) {}

    // ── Getters ──────────────────────────────
    int    getId()           const { return id; }
    string getUsername()     const { return username; }
    string getEmail()        const { return email; }
    string getPasswordHash() const { return passwordHash; }
    string getBio()          const { return bio; }
    string getJoinedAt()     const { return joinedAt; }
    bool   getIsActive()     const { return isActive; }

    // ── Setters ──────────────────────────────
    void setId(int i)                     { id = i; }
    void setUsername(const string& u)     { username = u; }
    void setEmail(const string& e)        { email = e; }
    void setPasswordHash(const string& p) { passwordHash = p; }
    void setBio(const string& b)          { bio = b; }
    void setJoinedAt(const string& t)     { joinedAt = t; }
    void setIsActive(bool a)              { isActive = a; }

    bool isValid() const { return !username.empty() && !email.empty(); }
};

#endif // USER_H
