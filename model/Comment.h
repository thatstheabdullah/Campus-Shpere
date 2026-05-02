#ifndef COMMENT_H
#define COMMENT_H

#include "Content.h"

// ─────────────────────────────────────────────
//  Class: Comment  (inherits Content)
//  OOP Concept: Inheritance + Polymorphism
// ─────────────────────────────────────────────
class Comment : public Content {
private:
    int postId;
    int likesCount;

public:
    Comment() : Content(), postId(0), likesCount(0) {}
    Comment(int id, int postId, const string& text,
            const string& author, const string& createdAt)
        : Content(id, text, author, createdAt),
          postId(postId), likesCount(0) {}

    // Overrides pure virtual display() from Content
    // POLYMORPHISM: Comment formats differently from Post
    void display() const override {
        cout << "  ↳ @" << authorUsername
             << " (" << createdAt << "): "
             << text
             << "  ♥ " << likesCount << endl;
    }

    int getPostId()     const { return postId; }
    int getLikesCount() const { return likesCount; }
    void setLikesCount(int l) { likesCount = l; }
    void incrementLikes()     { likesCount++; }
};

#endif // COMMENT_H
