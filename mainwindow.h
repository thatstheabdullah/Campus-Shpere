#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStackedWidget>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QFrame>
#include <QScrollArea>
#include <QTextEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QInputDialog>
#include <QFileDialog>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QCoreApplication>
#include <QPalette>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <fstream>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoginClicked();
    void onGoToSignup();
    void refreshCaptcha();
    void onSignupClicked();
    void onGoToLogin();
    void onPasswordChanged(const QString &pwd);
    void onPostClicked();
    void onLogoutClicked();
    void onPickImage();
    void toggleDarkMode();
    void onSearchChanged(const QString &text);
    void onSendMessage();
    void onContactSelected(QListWidgetItem *item);

private:
    // ── Database ──────────────────────────────
    void initDatabase();
    void logActivity(const QString &action);

    // ── Navigation ────────────────────────────
    QStackedWidget *stack;
    QStackedWidget *contentStack;
    void showLogin();
    void showSignup();
    void showFeed();
    void showPanel(int i);

    // ── Login page ────────────────────────────
    QWidget*  buildLoginPage();
    QLineEdit *loginUser, *loginPass, *loginCap;
    QLabel    *loginCapLabel, *loginErrLabel;
    QString    captcha;
    QString    genCaptcha();

    // ── Signup page ───────────────────────────
    QWidget*     buildSignupPage();
    QLineEdit    *supUser, *supEmail, *supPass, *supConf;
    QProgressBar *strengthBar;
    QLabel       *strengthLbl, *supErrLabel;
    int           pwStrength(const QString &p);

    // ── Feed panel ────────────────────────────
    QWidget*    buildFeedPanel();
    QTextEdit   *postInput;
    QLabel      *imagePreviewLabel;
    QString      selectedImagePath;
    QVBoxLayout *feedLayout;
    void         loadFeed();
    void         addPostCard(int id, const QString &author, const QString &text,
                     const QString &time, int likes, const QString &imgPath);
    void         loadComments(QVBoxLayout *lay, int postId);

    // ── Friends panel ─────────────────────────
    QWidget*    buildFriendsPanel();
    QLineEdit   *searchInput;
    QVBoxLayout *searchResLayout, *friendsLayout, *pendingLayout;
    void         loadFriends();
    void         loadPending();

    // ── Messages panel ────────────────────────
    QWidget*    buildMessagesPanel();
    QListWidget *contactsList;
    QVBoxLayout *chatLayout;
    QLineEdit   *msgInput;
    QLabel      *chatHeader;
    QFrame      *msgSidebar, *msgInputRow;
    QString      chatWith;
    void         loadContacts();
    void         loadChat(const QString &u);

    // ── Profile panel ─────────────────────────
    QWidget* buildProfilePanel();
    QLabel  *profUser, *profBio, *profStats;
    void     refreshProfile();

    // ── Main shell ────────────────────────────
    QWidget*    buildFeedPage();
    QLabel      *welcomeLabel;
    QLabel      *sidebarLogoText;   // kept as member so applyFullTheme() can restyle it
    QPushButton *navBtns[4];
    QPushButton *darkModeBtn;
    QPushButton *logoutBtn;
    QFrame      *sidebarFrame;

    // ── Theme ─────────────────────────────────
    bool isDark = true;
    void applyFullTheme();

    // Colour palette
    QString bg()   const { return isDark ? "#0D1117" : "#F6F7F8"; }
    QString card() const { return isDark ? "#161B22" : "#FFFFFF"; }
    QString card2()const { return isDark ? "#1C2128" : "#F0F2F5"; }
    QString acc()  const { return "#FF4500"; }
    QString acc2() const { return isDark ? "#FF6534" : "#CC3700"; }
    QString txt()  const { return isDark ? "#E6EDF3" : "#0D1117"; }   // always readable
    QString txt2() const { return isDark ? "#8B949E" : "#57606A"; }
    QString bor()  const { return isDark ? "#21262D" : "#E1E4E8"; }
    QString err()  const { return "#F85149"; }
    QString ok()   const { return "#3FB950"; }
    QString blue() const { return "#58A6FF"; }

    // ── Reusable style snippets ───────────────

    // Input field style
    QString FS() const {
        return QString(
                   "QLineEdit{background:%1;color:%2;border:1.5px solid %3;"
                   "border-radius:6px;padding:0 12px;font-size:13px;}"
                   "QLineEdit:focus{border-color:%4;}")
            .arg(card2(), txt(), bor(), acc());
    }

    // Primary button (orange)
    QString btnPrimary() const {
        return QString(
                   "QPushButton{background:%1;color:white;border-radius:6px;"
                   "padding:8px 20px;font-size:13px;font-weight:600;border:none;}"
                   "QPushButton:hover{background:%2;}"
                   "QPushButton:pressed{background:#CC3700;}")
            .arg(acc(), acc2());
    }

    // Secondary / outline button
    QString btnSecondary() const {
        return QString(
                   "QPushButton{background:%1;color:%2;border-radius:6px;"
                   "padding:7px 16px;font-size:12px;font-weight:500;border:1.5px solid %3;}"
                   "QPushButton:hover{border-color:%4;color:%4;}")
            .arg(card2(), txt2(), bor(), txt());
    }

    // Sidebar nav button style (active vs. idle)
    QString navStyle(bool active) const {
        if (active)
            return QString(
                       "QPushButton{background:%1;color:white;border-radius:6px;"
                       "padding:9px 14px;font-size:13px;font-weight:600;text-align:left;border:none;}")
                .arg(acc());
        return QString(
                   "QPushButton{background:transparent;color:%1;border-radius:6px;"
                   "padding:9px 14px;font-size:13px;font-weight:500;text-align:left;border:none;}"
                   "QPushButton:hover{background:%2;color:%1;}")
            .arg(txt(), card2());
    }

    // ── Helpers ───────────────────────────────
    QString    me;
    QString    hashPw(const QString &p);

    // Avatar colour pool
    QStringList avatarColors = {
        "#FF4500","#0079D3","#46D160","#FF585B","#FFB000",
        "#25B1FF","#FF66AC","#A06EF5","#00A693","#FF8717"
    };

    // Pick a deterministic colour for a given username
    QString avatarColor(const QString &username) const {
        int h = 0;
        for (QChar c : username) h += c.unicode();
        return avatarColors[h % avatarColors.size()];
    }

    // Draw a circular avatar with a letter inside
    QPixmap makeAvatar(const QString &letter, int size, const QString &bgColor = "#FF4500") const {
        QPixmap pm(size, size);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(0, 0, size, size);
        p.fillPath(path, QColor(bgColor));
        p.setPen(Qt::white);
        QFont f;
        f.setPixelSize(qMax(1, int(size * 0.4)));
        f.setBold(true);
        f.setFamily("Segoe UI");
        p.setFont(f);
        p.drawText(QRect(0, 0, size, size), Qt::AlignCenter,
                   letter.isEmpty() ? "?" : letter.toUpper());
        p.end();
        return pm;
    }
};

#endif // MAINWINDOW_H