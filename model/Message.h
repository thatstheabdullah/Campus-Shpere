#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  Class: Message
//  OOP Concept: Encapsulation
// ─────────────────────────────────────────────
class Message {
private:
    int    id;
    string senderUsername;
    string receiverUsername;
    string messageText;
    string sentAt;
    bool   isRead;

public:
    Message() : id(0), isRead(false) {}
    Message(int id, const string& sender, const string& receiver,
            const string& text, const string& sentAt)
        : id(id), senderUsername(sender), receiverUsername(receiver),
          messageText(text), sentAt(sentAt), isRead(false) {}

    int    getId()          const { return id; }
    string getSender()      const { return senderUsername; }
    string getReceiver()    const { return receiverUsername; }
    string getMessageText() const { return messageText; }
    string getSentAt()      const { return sentAt; }
    bool   getIsRead()      const { return isRead; }

    void markAsRead()               { isRead = true; }
    void setMessageText(const string& t) { messageText = t; }
};

#endif // MESSAGE_H
