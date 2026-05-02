# CampusSphere 🎓
**Academic Social Media Platform** — OOP Semester Project  
Section: BSSE-2A | Team #02

## Team Members
| Role | Name | Roll No |
|------|------|---------|
| Team Lead | Abdullah Hassan Butt | 25L-3039 |
| Database Layer | Salman Baloch | 25L-3081 |
| Feed & Posts Module | Hisham Ahmed | 25L-3033 |
| User Authentication | Abeeha Warda | 25L-3073 |
| Messages & Friends | Zain ul Abidin | 25L-3102 |

## Features
- Secure Login/Signup with CAPTCHA verification
- Live password strength indicator  
- Home Feed: posts, upvotes, comments, image attachments
- Friend request system: send, accept, reject
- Private messaging with chat bubbles
- User profiles with bio editing and stats
- Activity logging system
- One-click CSV data export
- Dark / Light mode toggle
- SQLite persistent database (no server needed)

## OOP Concepts Demonstrated
| Concept | Where Used |
|---------|-----------|
| **Encapsulation** | Private members in User, Post, Comment classes |
| **Inheritance** | Post and Comment both inherit from abstract Content |
| **Polymorphism** | Virtual `display()` overridden differently in Post vs Comment |
| **Abstraction** | DatabaseManager hides all SQL; UI only calls high-level methods |

## Tech Stack
- **Language:** C++17
- **UI Framework:** Qt 6 Widgets + Qt SQL
- **Database:** SQLite (built into Qt — no server install)
- **Build:** CMake 3.16+

## How to Run
1. Install Qt 6 Creator with Widgets + SQL modules
2. Open `CMakeLists.txt` in Qt Creator
3. Press Ctrl+R to build and run
4. `campussphere.db` is created automatically next to the executable
