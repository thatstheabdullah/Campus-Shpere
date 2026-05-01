#ifndef POST_H
#define POST_H

#include "Content.h"

// ─────────────────────────────────────────────
//  Class: Post  (inherits Content)
//  OOP Concept: Inheritance + Polymorphism
// ─────────────────────────────────────────────
class Post : public Content {
private:
    int    likesCount;
    int    commentsCount;
    string imageUrl;

public:
    Post() : Content(), likesCount(0), commentsCount(0) {}
    Post(int id, const string& text, const string& author,
         const string& createdAt, const string& imageUrl = "")
        : Content(id, text, author, createdAt),
          likesCount(0), commentsCount(0), imageUrl(imageUrl) {}

    // Overrides pure virtual display() from Content
    // POLYMORPHISM: Post formats differently from Comment
    void display() const override {
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
        cout << "POST  by @" << authorUsername    << endl;
        cout << "      at " << createdAt          << endl;
        cout << "  "  << text                     << endl;
        cout << "  ♥ " << likesCount
             << "  💬 " << commentsCount          << endl;
        cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << endl;
    }

    int    getLikesCount()    const { return likesCount; }
    int    getCommentsCount() const { return commentsCount; }
    string getImageUrl()      const { return imageUrl; }

    void setLikesCount(int l)    { likesCount = l; }
    void setCommentsCount(int c) { commentsCount = c; }
    void incrementLikes()        { likesCount++; }
};

#endif // POST_H
