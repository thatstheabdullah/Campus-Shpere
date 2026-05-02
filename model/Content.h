#ifndef CONTENT_H
#define CONTENT_H

#include <string>
#include <iostream>
using namespace std;

// ─────────────────────────────────────────────
//  Abstract Base Class: Content
//  OOP Concept: Abstraction + Polymorphism
//  Inherited by: Post, Comment
// ─────────────────────────────────────────────
class Content {
protected:
    int    id;
    string text;
    string authorUsername;
    string createdAt;

public:
    Content() : id(0) {}
    Content(int id, const string& text,
            const string& author, const string& createdAt)
        : id(id), text(text), authorUsername(author), createdAt(createdAt) {}

    virtual ~Content() {}

    // Pure virtual — forces every subclass to implement display()
    // This is POLYMORPHISM — same call, different behaviour per class
    virtual void display() const = 0;

    // Getters (Encapsulation — data accessed through methods only)
    int    getId()        const { return id; }
    string getText()      const { return text; }
    string getAuthor()    const { return authorUsername; }
    string getCreatedAt() const { return createdAt; }

    void setText(const string& t) { text = t; }
};

#endif // CONTENT_H
