#include "mainwindow.h"

// ════════════════════════════════════════════════════════════════
//  THEME  –  called every time dark/light is toggled
// ════════════════════════════════════════════════════════════════
void MainWindow::applyFullTheme()
{
    // ... [Palette code stays the same] ...

    // Global stylesheet
    setStyleSheet(QString(
                      "QWidget   { background:%1; color:%2; font-family:'Segoe UI',Arial; }"
                      "QFrame    { background:%3; }"
                      "QFrame#topBar { background:%3; border-bottom:1px solid %5; border-radius:0; }"   // <--- ADD THIS LINE
                      "QFrame#cardFrame { background:%3; border-radius:10px; border:none; }"            // <--- ADD THIS LINE
                      "QLabel    { background:transparent; color:%2; }"
                      "QLineEdit { background:%4; color:%2; border:1.5px solid %5;"
                      // ... [The rest of your stylesheet stays exactly the same] ...
                      ).arg(bg(), txt(), card(), card2(), bor(), acc()));



    // ── Per-widget restyles ──────────────────────────────────────

    if (sidebarFrame)
        sidebarFrame->setStyleSheet(
            QString("QFrame { background:%1; border-right:1px solid %2; border-radius:0; }")
                .arg(card(), bor()));

    // FIX: sidebar logo text always uses txt() so it is visible in both modes
    if (sidebarLogoText)
        sidebarLogoText->setStyleSheet(
            QString("color:%1; font-size:15px; font-weight:700;"
                    "background:transparent; border:none; letter-spacing:-0.3px;")
                .arg(txt()));

    if (welcomeLabel)
        welcomeLabel->setStyleSheet(
            QString("color:%1; font-size:12px; font-weight:600; padding:6px 10px;"
                    "background:%2; border-radius:6px; margin-bottom:4px; border:none;")
                .arg(txt(), card2()));

    if (darkModeBtn) {
        darkModeBtn->setText(isDark ? "  Light Mode" : "  Dark Mode");
        darkModeBtn->setStyleSheet(
            QString("QPushButton { background:%1; color:%2; border-radius:6px;"
                    "padding:8px 14px; font-size:12px; font-weight:500;"
                    "text-align:left; border:none; }"
                    "QPushButton:hover { background:%3; color:%2; }")
                .arg(card2(), txt(), bor()));
    }

    if (logoutBtn)
        logoutBtn->setStyleSheet(
            QString("QPushButton { background:transparent; color:%1; border-radius:6px;"
                    "padding:9px 14px; font-size:13px; font-weight:500;"
                    "text-align:left; border:none; }"
                    "QPushButton:hover { background:#2D1215; color:%1; }")
                .arg(err()));

    if (msgSidebar)
        msgSidebar->setStyleSheet(
            QString("QFrame { background:%1; border-right:1px solid %2; border-radius:0; }")
                .arg(card(), bor()));

    if (msgInputRow)
        msgInputRow->setStyleSheet(
            QString("QFrame { background:%1; border-top:1px solid %2; border-radius:0; }")
                .arg(card(), bor()));

    if (chatHeader)
        chatHeader->setStyleSheet(
            QString("color:%1; font-size:14px; font-weight:700;"
                    "padding:0 20px; background:%2;"
                    "border-bottom:1px solid %3; border-radius:0;")
                .arg(txt(), card(), bor()));

    // FIX: contacts list item colour explicitly uses txt() so names show in light mode
    if (contactsList)
        contactsList->setStyleSheet(
            QString("QListWidget { background:transparent; border:none; color:%1; }"
                    "QListWidget::item { color:%1; padding:12px 16px; border-radius:0;"
                    "  margin:0; border-bottom:1px solid %2; }"
                    "QListWidget::item:hover    { background:%3; }"
                    "QListWidget::item:selected { background:%3; color:%1;"
                    "  border-left:3px solid %4; }")
                .arg(txt(), bor(), card2(), acc()));

    if (msgInput) msgInput->setStyleSheet(FS());
}

// ════════════════════════════════════════════════════════════════
//  CONSTRUCTOR
// ════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("CampusSphere");
    setMinimumSize(1120, 740);
    isDark = true;

    // Null-init all pointer members so applyFullTheme() can guard safely
    sidebarFrame = nullptr;
    sidebarLogoText = nullptr;
    welcomeLabel = nullptr;
    darkModeBtn = nullptr;
    logoutBtn = nullptr;
    msgSidebar = nullptr;
    msgInputRow = nullptr;
    chatHeader = nullptr;
    contactsList = nullptr;
    msgInput = nullptr;

    initDatabase();

    stack = new QStackedWidget(this);
    stack->addWidget(buildLoginPage());   // index 0
    stack->addWidget(buildSignupPage());  // index 1
    stack->addWidget(buildFeedPage());    // index 2
    setCentralWidget(stack);

    applyFullTheme();
    showLogin();
}

MainWindow::~MainWindow() {}

// ════════════════════════════════════════════════════════════════
//  DATABASE
// ════════════════════════════════════════════════════════════════
void MainWindow::initDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/campussphere.db");
    if (!db.open()) { qDebug() << db.lastError().text(); return; }

    QSqlQuery q;
    q.exec("PRAGMA foreign_keys = ON;");
    q.exec("CREATE TABLE IF NOT EXISTS users("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  username TEXT NOT NULL UNIQUE,"
           "  email TEXT NOT NULL UNIQUE,"
           "  password_hash TEXT NOT NULL,"
           "  bio TEXT DEFAULT '',"
           "  joined_at TEXT DEFAULT(datetime('now')))");
    q.exec("CREATE TABLE IF NOT EXISTS posts("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  author_username TEXT NOT NULL,"
           "  text TEXT NOT NULL,"
           "  image_path TEXT DEFAULT '',"
           "  likes_count INTEGER DEFAULT 0,"
           "  created_at TEXT DEFAULT(datetime('now')))");
    q.exec("ALTER TABLE posts ADD COLUMN image_path TEXT DEFAULT ''"); // safe to fail if exists
    q.exec("CREATE TABLE IF NOT EXISTS post_likes("
           "  post_id INTEGER, username TEXT, UNIQUE(post_id,username))");
    q.exec("CREATE TABLE IF NOT EXISTS comments("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  post_id INTEGER NOT NULL,"
           "  author_username TEXT NOT NULL,"
           "  text TEXT NOT NULL,"
           "  created_at TEXT DEFAULT(datetime('now')))");
    q.exec("CREATE TABLE IF NOT EXISTS friendships("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  username TEXT NOT NULL,"
           "  friend_username TEXT NOT NULL,"
           "  status TEXT DEFAULT 'pending',"
           "  UNIQUE(username,friend_username))");
    q.exec("CREATE TABLE IF NOT EXISTS messages("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  sender_username TEXT NOT NULL,"
           "  receiver_username TEXT NOT NULL,"
           "  message_text TEXT NOT NULL,"
           "  sent_at TEXT DEFAULT(datetime('now')))");
    q.exec("CREATE TABLE IF NOT EXISTS activity_log("
           "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "  username TEXT NOT NULL,"
           "  action TEXT NOT NULL,"
           "  logged_at TEXT DEFAULT(datetime('now')))");

    QDir().mkpath(QCoreApplication::applicationDirPath() + "/post_images");
}

void MainWindow::logActivity(const QString &action)
{
    QSqlQuery q;
    q.prepare("INSERT INTO activity_log(username,action) VALUES(:u,:a)");
    q.bindValue(":u", me);
    q.bindValue(":a", action);
    q.exec();
}

// ════════════════════════════════════════════════════════════════
//  NAVIGATION
// ════════════════════════════════════════════════════════════════
void MainWindow::showLogin()  { stack->setCurrentIndex(0); }
void MainWindow::showSignup() { stack->setCurrentIndex(1); }
void MainWindow::showFeed()   { stack->setCurrentIndex(2); showPanel(0); }

void MainWindow::onGoToSignup() { showSignup(); }
void MainWindow::onGoToLogin()  { showLogin();  }

void MainWindow::showPanel(int idx)
{
    contentStack->setCurrentIndex(idx);
    for (int i = 0; i < 4; i++)
        navBtns[i]->setStyleSheet(navStyle(i == idx));

    if (idx == 0) loadFeed();
    if (idx == 1) { loadFriends(); loadPending(); }
    if (idx == 2) loadContacts();
    if (idx == 3) refreshProfile();
}

void MainWindow::toggleDarkMode()
{
    isDark = !isDark;
    applyFullTheme();
    showPanel(contentStack->currentIndex());
}

// ════════════════════════════════════════════════════════════════
//  LOGIN PAGE
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildLoginPage()
{
    auto *pg  = new QWidget;
    auto *out = new QVBoxLayout(pg);
    out->setAlignment(Qt::AlignCenter);

    // Logo row
    auto *logoRow  = new QHBoxLayout;
    logoRow->setAlignment(Qt::AlignCenter);
    auto *logoIcon = new QLabel;
    logoIcon->setPixmap(makeAvatar("C", 52, acc()));
    logoIcon->setStyleSheet("border:none; background:transparent;");
    auto *logoText = new QLabel("CampusSphere");
    logoText->setStyleSheet(
        QString("font-size:26px; font-weight:700; color:%1;"
                "background:transparent; border:none; letter-spacing:-0.5px;").arg(txt()));
    logoRow->addWidget(logoIcon);
    logoRow->addSpacing(10);
    logoRow->addWidget(logoText);

    auto *sub = new QLabel("Sign in to your account");
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet(
        QString("color:%1; font-size:13px; background:transparent; border:none;").arg(txt2()));

    // Card frame
    auto *cardFrame = new QFrame;
    cardFrame->setFixedWidth(400);
    cardFrame->setStyleSheet(
        QString("QFrame { background:%1; border-radius:12px; border:none; }").arg(card()));
    auto *lay = new QVBoxLayout(cardFrame);
    lay->setSpacing(14);
    lay->setContentsMargins(32, 32, 32, 32);

    loginUser = new QLineEdit;
    loginUser->setPlaceholderText("Username");
    loginUser->setMinimumHeight(44);
    loginUser->setStyleSheet(FS());

    loginPass = new QLineEdit;
    loginPass->setPlaceholderText("Password");
    loginPass->setEchoMode(QLineEdit::Password);
    loginPass->setMinimumHeight(44);
    loginPass->setStyleSheet(FS());

    // CAPTCHA box
    auto *capBox = new QFrame;
    capBox->setStyleSheet(
        QString("QFrame { background:%1; border-radius:8px; border:1.5px solid %2; }")
            .arg(card2(), bor()));
    auto *capLay = new QVBoxLayout(capBox);
    capLay->setContentsMargins(14, 12, 14, 12);
    capLay->setSpacing(8);

    auto *capTitle = new QLabel("Security Verification");
    capTitle->setStyleSheet(
        QString("color:%1; font-size:11px; font-weight:600;"
                "background:transparent; border:none; letter-spacing:0.5px;").arg(txt2()));

    auto *capRow = new QHBoxLayout;
    loginCapLabel = new QLabel;
    loginCapLabel->setStyleSheet(
        QString("font-family:'Courier New',monospace; font-size:24px; font-weight:700;"
                "color:%1; background:%2; padding:8px 20px; border-radius:6px;"
                "letter-spacing:10px; border:none;").arg(acc(), bg()));

    auto *refBtn = new QPushButton("↺");
    refBtn->setFixedSize(40, 40);
    refBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:%2; border:1.5px solid %3;"
                "border-radius:6px; font-size:18px; font-weight:bold; }"
                "QPushButton:hover { background:%3; color:%4; }")
            .arg(bg(), acc(), bor(), txt()));
    connect(refBtn, &QPushButton::clicked, this, &MainWindow::refreshCaptcha);

    capRow->addWidget(loginCapLabel);
    capRow->addStretch();
    capRow->addWidget(refBtn);

    loginCap = new QLineEdit;
    loginCap->setPlaceholderText("Enter the code above");
    loginCap->setMinimumHeight(40);
    loginCap->setStyleSheet(FS());

    capLay->addWidget(capTitle);
    capLay->addLayout(capRow);
    capLay->addWidget(loginCap);

    loginErrLabel = new QLabel;
    loginErrLabel->setStyleSheet(
        QString("color:%1; font-size:12px; padding:8px 12px;"
                "background:#2D0F0F; border-radius:6px; border-left:3px solid %1;").arg(err()));
    loginErrLabel->setWordWrap(true);
    loginErrLabel->hide();

    auto *loginBtn = new QPushButton("Sign In");
    loginBtn->setMinimumHeight(44);
    loginBtn->setStyleSheet(btnPrimary());
    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginClicked);

    // OR divider
    auto *divRow = new QHBoxLayout;
    auto *line1  = new QFrame;
    line1->setFrameShape(QFrame::HLine);
    line1->setStyleSheet(QString("color:%1; background:%1;").arg(bor()));
    auto *orLbl  = new QLabel("OR");
    orLbl->setStyleSheet(
        QString("color:%1; font-size:11px; font-weight:600;"
                "background:transparent; border:none;").arg(txt2()));
    orLbl->setAlignment(Qt::AlignCenter);
    orLbl->setFixedWidth(30);
    auto *line2  = new QFrame;
    line2->setFrameShape(QFrame::HLine);
    line2->setStyleSheet(QString("color:%1; background:%1;").arg(bor()));
    divRow->addWidget(line1);
    divRow->addWidget(orLbl);
    divRow->addWidget(line2);

    auto *goSup = new QPushButton("Create Account");
    goSup->setMinimumHeight(44);
    goSup->setStyleSheet(
        QString("QPushButton { background:transparent; color:%1; border-radius:6px;"
                "padding:8px 20px; font-size:13px; font-weight:600; border:1.5px solid %2; }"
                "QPushButton:hover { border-color:%1; background:%3; }")
            .arg(acc(), bor(), card2()));
    connect(goSup, &QPushButton::clicked, this, &MainWindow::onGoToSignup);

    lay->addWidget(loginUser);
    lay->addWidget(loginPass);
    lay->addWidget(capBox);
    lay->addWidget(loginErrLabel);
    lay->addWidget(loginBtn);
    lay->addLayout(divRow);
    lay->addWidget(goSup);

    auto *wrap = new QVBoxLayout;
    wrap->setSpacing(18);
    wrap->setAlignment(Qt::AlignCenter);
    wrap->addLayout(logoRow);
    wrap->addWidget(sub);
    wrap->addWidget(cardFrame);
    out->addLayout(wrap);

    refreshCaptcha();
    return pg;
}

QString MainWindow::genCaptcha()
{
    QString chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    QString result;
    for (int i = 0; i < 5; i++)
        result += chars[QRandomGenerator::global()->bounded(chars.length())];
    return result;
}

void MainWindow::refreshCaptcha()
{
    captcha = genCaptcha();
    loginCapLabel->setText(captcha);
}

void MainWindow::onLoginClicked()
{
    loginErrLabel->hide();
    QString u  = loginUser->text().trimmed();
    QString p  = loginPass->text();
    QString cp = loginCap->text().trimmed().toUpper();

    if (u.isEmpty() || p.isEmpty()) {
        loginErrLabel->setText("Please fill in all fields.");
        loginErrLabel->show(); return;
    }
    if (cp != captcha) {
        loginErrLabel->setText("Incorrect CAPTCHA code. Please try again.");
        loginErrLabel->show(); refreshCaptcha(); loginCap->clear(); return;
    }

    QSqlQuery q;
    q.prepare("SELECT password_hash FROM users WHERE username=:u");
    q.bindValue(":u", u); q.exec();
    if (!q.next()) {
        loginErrLabel->setText("No account found with that username.");
        loginErrLabel->show(); return;
    }
    if (q.value(0).toString() != hashPw(p)) {
        loginErrLabel->setText("Incorrect password. Please try again.");
        loginErrLabel->show(); return;
    }

    me = u;
    welcomeLabel->setText("  " + u);
    logActivity("login");
    loginUser->clear(); loginPass->clear(); loginCap->clear();
    showFeed();
}

// ════════════════════════════════════════════════════════════════
//  SIGNUP PAGE
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildSignupPage()
{
    auto *pg  = new QWidget;
    auto *out = new QVBoxLayout(pg);
    out->setAlignment(Qt::AlignCenter);

    // Logo
    auto *logoRow  = new QHBoxLayout;
    logoRow->setAlignment(Qt::AlignCenter);
    auto *logoIcon = new QLabel;
    logoIcon->setPixmap(makeAvatar("C", 52, acc()));
    logoIcon->setStyleSheet("border:none; background:transparent;");
    auto *logoText = new QLabel("Join CampusSphere");
    logoText->setStyleSheet(
        QString("font-size:24px; font-weight:700; color:%1;"
                "background:transparent; border:none;").arg(txt()));
    logoRow->addWidget(logoIcon);
    logoRow->addSpacing(10);
    logoRow->addWidget(logoText);

    // Card
    auto *cardFrame = new QFrame;
    cardFrame->setFixedWidth(400);
    cardFrame->setStyleSheet(
        QString("QFrame { background:%1; border-radius:12px; border:none; }").arg(card()));
    auto *lay = new QVBoxLayout(cardFrame);
    lay->setSpacing(12);
    lay->setContentsMargins(32, 28, 32, 28);

    supUser  = new QLineEdit; supUser->setPlaceholderText("Choose a username");
    supUser->setMinimumHeight(44); supUser->setStyleSheet(FS());

    supEmail = new QLineEdit; supEmail->setPlaceholderText("University email address");
    supEmail->setMinimumHeight(44); supEmail->setStyleSheet(FS());

    supPass  = new QLineEdit; supPass->setPlaceholderText("Create a password");
    supPass->setEchoMode(QLineEdit::Password);
    supPass->setMinimumHeight(44); supPass->setStyleSheet(FS());
    connect(supPass, &QLineEdit::textChanged, this, &MainWindow::onPasswordChanged);

    // Password strength bar
    strengthBar = new QProgressBar;
    strengthBar->setRange(0, 100); strengthBar->setValue(0);
    strengthBar->setTextVisible(false); strengthBar->setFixedHeight(4);
    strengthBar->setStyleSheet(
        QString("QProgressBar { background:%1; border-radius:2px; border:none; }"
                "QProgressBar::chunk { background:%2; border-radius:2px; }")
            .arg(card2(), err()));

    strengthLbl = new QLabel("Password strength");
    strengthLbl->setStyleSheet(
        QString("color:%1; font-size:11px; background:transparent; border:none;").arg(txt2()));

    supConf  = new QLineEdit; supConf->setPlaceholderText("Confirm password");
    supConf->setEchoMode(QLineEdit::Password);
    supConf->setMinimumHeight(44); supConf->setStyleSheet(FS());

    supErrLabel = new QLabel;
    supErrLabel->setStyleSheet(
        QString("color:%1; font-size:12px; padding:8px 12px;"
                "background:#2D0F0F; border-radius:6px; border-left:3px solid %1;").arg(err()));
    supErrLabel->setWordWrap(true);
    supErrLabel->hide();

    auto *btn = new QPushButton("Create Account");
    btn->setMinimumHeight(44); btn->setStyleSheet(btnPrimary());
    connect(btn, &QPushButton::clicked, this, &MainWindow::onSignupClicked);

    auto *goL = new QPushButton("Already have an account? Sign in");
    goL->setFlat(true);
    goL->setStyleSheet(
        QString("color:%1; font-size:12px; background:transparent;"
                "border:none; font-weight:500;").arg(acc()));
    connect(goL, &QPushButton::clicked, this, &MainWindow::onGoToLogin);

    lay->addWidget(supUser);
    lay->addWidget(supEmail);
    lay->addWidget(supPass);
    lay->addWidget(strengthBar);
    lay->addWidget(strengthLbl);
    lay->addWidget(supConf);
    lay->addWidget(supErrLabel);
    lay->addWidget(btn);
    lay->addWidget(goL, 0, Qt::AlignCenter);

    auto *wrap = new QVBoxLayout;
    wrap->setSpacing(18); wrap->setAlignment(Qt::AlignCenter);
    wrap->addLayout(logoRow); wrap->addWidget(cardFrame);
    out->addLayout(wrap);
    return pg;
}

void MainWindow::onPasswordChanged(const QString &p)
{
    int s = pwStrength(p);
    strengthBar->setValue(s);
    QString colour, label;
    if      (s < 25) { colour = "#F85149"; label = "Weak"; }
    else if (s < 50) { colour = "#E3B341"; label = "Fair"; }
    else if (s < 75) { colour = "#58A6FF"; label = "Good"; }
    else             { colour = "#3FB950"; label = "Strong"; }
    strengthBar->setStyleSheet(
        QString("QProgressBar { background:%1; border-radius:2px; border:none; }"
                "QProgressBar::chunk { background:%2; border-radius:2px; }")
            .arg(card2(), colour));
    strengthLbl->setStyleSheet(
        QString("color:%1; font-size:11px; background:transparent; border:none;").arg(colour));
    strengthLbl->setText("Strength: " + label);
}

int MainWindow::pwStrength(const QString &p)
{
    int s = 0;
    if (p.length() >= 6)  s += 20;
    if (p.length() >= 10) s += 20;
    if (p.contains(QRegularExpression("[A-Z]")))      s += 20;
    if (p.contains(QRegularExpression("[0-9]")))      s += 20;
    if (p.contains(QRegularExpression("[^a-zA-Z0-9]"))) s += 20;
    return s;
}

void MainWindow::onSignupClicked()
{
    supErrLabel->hide();
    QString u = supUser->text().trimmed();
    QString e = supEmail->text().trimmed();
    QString p = supPass->text();
    QString c = supConf->text();

    if (u.isEmpty() || e.isEmpty() || p.isEmpty()) {
        supErrLabel->setText("All fields are required."); supErrLabel->show(); return;
    }
    if (u.contains(' ')) {
        supErrLabel->setText("Username cannot contain spaces."); supErrLabel->show(); return;
    }
    if (u.length() < 3) {
        supErrLabel->setText("Username must be at least 3 characters."); supErrLabel->show(); return;
    }
    if (p != c) {
        supErrLabel->setText("Passwords do not match."); supErrLabel->show(); return;
    }
    if (pwStrength(p) < 40) {
        supErrLabel->setText("Password is too weak. Add uppercase letters, numbers or symbols.");
        supErrLabel->show(); return;
    }

    QSqlQuery q;
    q.prepare("SELECT id FROM users WHERE username=:u");
    q.bindValue(":u", u); q.exec();
    if (q.next()) {
        supErrLabel->setText("Username is already taken. Please choose another.");
        supErrLabel->show(); return;
    }

    q.prepare("INSERT INTO users(username,email,password_hash) VALUES(:u,:e,:p)");
    q.bindValue(":u", u); q.bindValue(":e", e); q.bindValue(":p", hashPw(p));
    if (q.exec()) {
        QMessageBox::information(this, "Account Created",
                                 "Welcome to CampusSphere! You can now sign in.");
        supUser->clear(); supEmail->clear(); supPass->clear(); supConf->clear();
        showLogin();
    } else {
        supErrLabel->setText(q.lastError().text()); supErrLabel->show();
    }
}

// ════════════════════════════════════════════════════════════════
//  MAIN SHELL  (sidebar + content stack)
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildFeedPage()
{
    auto *pg   = new QWidget;
    auto *root = new QHBoxLayout(pg);
    root->setContentsMargins(0, 0, 0, 0); root->setSpacing(0);

    // ── Sidebar ──────────────────────────────────────────────────
    sidebarFrame = new QFrame;
    sidebarFrame->setFixedWidth(220);
    sidebarFrame->setStyleSheet(
        QString("QFrame { background:%1; border-right:1px solid %2; border-radius:0; }")
            .arg(card(), bor()));

    auto *sl = new QVBoxLayout(sidebarFrame);
    sl->setContentsMargins(12, 20, 12, 16); sl->setSpacing(2);

    // Logo
    auto *logoRow  = new QHBoxLayout;
    logoRow->setAlignment(Qt::AlignLeft);
    auto *logoIcon = new QLabel;
    logoIcon->setPixmap(makeAvatar("C", 32, acc()));
    logoIcon->setStyleSheet("border:none; background:transparent;");

    // FIX: use member pointer so applyFullTheme() can restyle it on toggle
    sidebarLogoText = new QLabel("CampusSphere");
    sidebarLogoText->setStyleSheet(
        QString("color:%1; font-size:15px; font-weight:700;"
                "background:transparent; border:none; letter-spacing:-0.3px;").arg(txt()));

    logoRow->addWidget(logoIcon);
    logoRow->addSpacing(8);
    logoRow->addWidget(sidebarLogoText);
    sl->addLayout(logoRow);
    sl->addSpacing(16);

    // Logged-in user chip
    welcomeLabel = new QLabel("  user");
    welcomeLabel->setStyleSheet(
        QString("color:%1; font-size:12px; font-weight:600; padding:6px 10px;"
                "background:%2; border-radius:6px; margin-bottom:4px; border:none;")
            .arg(txt(), card2()));
    sl->addWidget(welcomeLabel);
    sl->addSpacing(8);

    // Nav buttons
    QStringList icons  = { "▣", "⊕", "◎", "◉" };
    QStringList labels = { "Home", "Friends", "Messages", "Profile" };
    for (int i = 0; i < 4; i++) {
        navBtns[i] = new QPushButton(icons[i] + "  " + labels[i]);
        navBtns[i]->setMinimumHeight(40);
        navBtns[i]->setStyleSheet(navStyle(i == 0));
        int idx = i;
        connect(navBtns[i], &QPushButton::clicked, this, [=]() { showPanel(idx); });
        sl->addWidget(navBtns[i]);
    }
    sl->addStretch();

    // Dark / light toggle
    darkModeBtn = new QPushButton("  Light Mode");
    darkModeBtn->setMinimumHeight(36);
    darkModeBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:%2; border-radius:6px;"
                "padding:8px 14px; font-size:12px; font-weight:500;"
                "text-align:left; border:none; }"
                "QPushButton:hover { background:%3; color:%2; }")
            .arg(card2(), txt(), bor()));
    connect(darkModeBtn, &QPushButton::clicked, this, &MainWindow::toggleDarkMode);
    sl->addWidget(darkModeBtn);
    sl->addSpacing(4);

    // Sign out
    logoutBtn = new QPushButton("  Sign Out");
    logoutBtn->setMinimumHeight(40);
    logoutBtn->setStyleSheet(
        QString("QPushButton { background:transparent; color:%1; border-radius:6px;"
                "padding:9px 14px; font-size:13px; font-weight:500;"
                "text-align:left; border:none; }"
                "QPushButton:hover { background:#2D1215; color:%1; }")
            .arg(err()));
    connect(logoutBtn, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    sl->addWidget(logoutBtn);

    // ── Content area ─────────────────────────────────────────────
    contentStack = new QStackedWidget;
    contentStack->addWidget(buildFeedPanel());      // 0
    contentStack->addWidget(buildFriendsPanel());   // 1
    contentStack->addWidget(buildMessagesPanel());  // 2
    contentStack->addWidget(buildProfilePanel());   // 3

    root->addWidget(sidebarFrame);
    root->addWidget(contentStack);
    return pg;
}

// ════════════════════════════════════════════════════════════════
//  FEED PANEL
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildFeedPanel()
{
    auto *panel = new QWidget;
    auto *lay   = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0); lay->setSpacing(0);

    // Top bar
    auto *topBar = new QFrame;
    topBar->setFixedHeight(56);
    topBar->setObjectName("topBar"); // <--- Replaces the inline stylesheet

    auto *tbl = new QHBoxLayout(topBar);
    tbl->setContentsMargins(24, 0, 24, 0);

    auto *feedTitle = new QLabel("Home");
    // <--- Removed 'color:%1;'
    feedTitle->setStyleSheet("font-size:16px; font-weight:700; background:transparent; border:none;");
    tbl->addWidget(feedTitle); tbl->addStretch();

    // Scrollable content
    auto *scroll  = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border:none; background:transparent; }");
    auto *content = new QWidget;
    // <--- DELETED THIS LINE: content->setStyleSheet(QString("background:%1;").arg(bg()));
    auto *contentLay = new QVBoxLayout(content);
    contentLay->setContentsMargins(24, 20, 24, 20); contentLay->setSpacing(12);

    // Compose box
    auto *comp = new QFrame;
    comp->setObjectName("cardFrame"); // <--- Replaces the inline stylesheet

    auto *cl = new QVBoxLayout(comp);
    cl->setContentsMargins(16, 14, 16, 14); cl->setSpacing(10);

    auto *compTop = new QHBoxLayout;
    auto *compAv  = new QLabel;
    compAv->setFixedSize(36, 36);
    compAv->setStyleSheet("border:none; background:transparent;");
    compAv->setPixmap(makeAvatar("?", 36, txt2()));
    compAv->setObjectName("composeAvatar");

    postInput = new QTextEdit;
    postInput->setPlaceholderText("What's on your mind?");
    postInput->setMaximumHeight(80);
    postInput->setStyleSheet(
        QString("QTextEdit { background:%1; color:%2; border:none;"
                "border-radius:8px; padding:10px; font-size:13px; }").arg(card2(), txt()));
    compTop->addWidget(compAv); compTop->addWidget(postInput);

    imagePreviewLabel = new QLabel;
    imagePreviewLabel->setFixedHeight(120);
    imagePreviewLabel->setAlignment(Qt::AlignCenter);
    imagePreviewLabel->setStyleSheet(
        QString("background:%1; border-radius:8px; border:1.5px dashed %2; color:%3; font-size:12px;")
            .arg(card2(), bor(), txt2()));
    imagePreviewLabel->hide();

    auto *compBottom = new QHBoxLayout;
    auto *imgBtn = new QPushButton("  Attach Photo");
    imgBtn->setFixedHeight(32); imgBtn->setStyleSheet(btnSecondary());
    connect(imgBtn, &QPushButton::clicked, this, &MainWindow::onPickImage);

    auto *clearBtn = new QPushButton("Remove");
    clearBtn->setFixedHeight(32);
    clearBtn->setStyleSheet(
        QString("QPushButton { background:transparent; color:%1; border:none;"
                "font-size:12px; font-weight:500; }"
                "QPushButton:hover { color:%2; }").arg(txt2(), err()));
    connect(clearBtn, &QPushButton::clicked, this, [=]() {
        selectedImagePath.clear();
        imagePreviewLabel->setPixmap(QPixmap());
        imagePreviewLabel->setText("");
        imagePreviewLabel->hide();
    });

    auto *postBtn = new QPushButton("Post");
    postBtn->setFixedHeight(32); postBtn->setFixedWidth(80);
    postBtn->setStyleSheet(btnPrimary());
    connect(postBtn, &QPushButton::clicked, this, &MainWindow::onPostClicked);

    compBottom->addWidget(imgBtn); compBottom->addWidget(clearBtn);
    compBottom->addStretch(); compBottom->addWidget(postBtn);

    cl->addLayout(compTop); cl->addWidget(imagePreviewLabel); cl->addLayout(compBottom);

    // Feed list area
    auto *feedWidget = new QWidget;
    // <--- DELETE THIS LINE: feedWidget->setStyleSheet(QString("background:%1;").arg(bg()));

    feedLayout = new QVBoxLayout(feedWidget);
    feedLayout->setSpacing(8); feedLayout->setAlignment(Qt::AlignTop);

    contentLay->addWidget(comp); contentLay->addWidget(feedWidget);
    scroll->setWidget(content);
    lay->addWidget(topBar); lay->addWidget(scroll);
    return panel;
}

void MainWindow::onPickImage()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Select Photo", "", "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (path.isEmpty()) return;

    QString dest = QCoreApplication::applicationDirPath() + "/post_images/"
                   + QString::number(QDateTime::currentMSecsSinceEpoch())
                   + "_" + QFileInfo(path).fileName();
    QFile::copy(path, dest);
    selectedImagePath = dest;

    QPixmap pm(dest);
    if (!pm.isNull())
        imagePreviewLabel->setPixmap(
            pm.scaled(500, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        imagePreviewLabel->setText("Could not load image");
    imagePreviewLabel->show();
}

void MainWindow::onPostClicked()
{
    QString text = postInput->toPlainText().trimmed();
    if (text.isEmpty() && selectedImagePath.isEmpty()) {
        QMessageBox::warning(this, "Empty Post", "Write something or attach a photo.");
        return;
    }
    QSqlQuery q;
    q.prepare("INSERT INTO posts(author_username,text,image_path) VALUES(:a,:t,:i)");
    q.bindValue(":a", me); q.bindValue(":t", text); q.bindValue(":i", selectedImagePath);
    if (q.exec()) {
        postInput->clear(); selectedImagePath.clear();
        imagePreviewLabel->setPixmap(QPixmap()); imagePreviewLabel->hide();
        logActivity("created_post"); loadFeed();
    }
}

void MainWindow::loadFeed()
{
    // Clear previous feed cards
    QLayoutItem *it;
    while ((it = feedLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater();
        delete it;
    }

    // Update compose avatar
    if (auto *av = stack->widget(2)->findChild<QLabel*>("composeAvatar"))
        av->setPixmap(makeAvatar(me.isEmpty() ? "?" : QString(me[0]), 36, avatarColor(me)));

    QSqlQuery q;
    q.exec("SELECT id,author_username,text,likes_count,created_at,image_path "
           "FROM posts ORDER BY created_at DESC LIMIT 50");
    bool any = false;
    while (q.next()) {
        any = true;
        addPostCard(q.value(0).toInt(), q.value(1).toString(), q.value(2).toString(),
                    q.value(4).toString(), q.value(3).toInt(), q.value(5).toString());
    }
    if (!any) {
        auto *e = new QLabel("No posts yet — be the first to share something.");
        e->setAlignment(Qt::AlignCenter);
        e->setStyleSheet(
            QString("color:%1; font-size:14px; padding:60px;"
                    "background:transparent; border:none;").arg(txt2()));
        feedLayout->addWidget(e);
    }
}

void MainWindow::addPostCard(int pid, const QString &author, const QString &text,
                             const QString &time, int likes, const QString &imgPath)
{
    auto *postCard = new QFrame;
    postCard->setStyleSheet(
        QString("QFrame { background:%1; border-radius:10px; border:none; }").arg(card()));

    auto *lay = new QVBoxLayout(postCard);
    lay->setContentsMargins(16, 14, 16, 14); lay->setSpacing(10);

    // Header (avatar + username + time)
    auto *hdr = new QHBoxLayout; hdr->setSpacing(10);
    auto *avLbl = new QLabel;
    avLbl->setPixmap(makeAvatar(author.isEmpty() ? "?" : QString(author[0]), 38, avatarColor(author)));
    avLbl->setStyleSheet("border:none; background:transparent;");

    auto *userCol = new QVBoxLayout; userCol->setSpacing(1);
    auto *aLbl = new QLabel("u/" + author);
    aLbl->setStyleSheet(
        QString("color:%1; font-weight:700; font-size:13px;"
                "background:transparent; border:none;").arg(txt()));
    auto *tLbl = new QLabel(time.left(16));
    tLbl->setStyleSheet(
        QString("color:%1; font-size:11px; background:transparent; border:none;").arg(txt2()));
    userCol->addWidget(aLbl); userCol->addWidget(tLbl);
    hdr->addWidget(avLbl); hdr->addLayout(userCol); hdr->addStretch();
    lay->addLayout(hdr);

    // Post body text
    if (!text.isEmpty()) {
        auto *tLab = new QLabel(text); tLab->setWordWrap(true);
        tLab->setStyleSheet(
            QString("color:%1; font-size:14px; line-height:1.5;"
                    "background:transparent; border:none;").arg(txt()));
        lay->addWidget(tLab);
    }

    // Post image (if any)
    if (!imgPath.isEmpty()) {
        QPixmap pm(imgPath);
        if (!pm.isNull()) {
            auto *imgLbl = new QLabel;
            imgLbl->setPixmap(pm.scaled(700, 360, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            imgLbl->setAlignment(Qt::AlignLeft);
            imgLbl->setStyleSheet("border-radius:8px; background:transparent;");
            lay->addWidget(imgLbl);
        }
    }

    // Action bar (upvote / comments / delete)
    auto *actBar = new QFrame;
    actBar->setStyleSheet(
        QString("QFrame { background:%1; border-radius:6px; border:none; }").arg(card2()));
    auto *abl = new QHBoxLayout(actBar);
    abl->setContentsMargins(4, 4, 4, 4); abl->setSpacing(2);

    QSqlQuery lc;
    lc.prepare("SELECT 1 FROM post_likes WHERE post_id=:p AND username=:u");
    lc.bindValue(":p", pid); lc.bindValue(":u", me); lc.exec();
    bool liked = lc.next();

    auto *upBtn = new QPushButton(QString("▲  %1").arg(likes));
    upBtn->setFixedHeight(30);
    upBtn->setStyleSheet(
        QString("QPushButton { background:%1; color:%2; border-radius:5px;"
                "padding:0 12px; font-size:12px; font-weight:700; border:none; }"
                "QPushButton:hover { background:%3; color:%4; }")
            .arg(liked ? "#3a1500" : card2(),
                 liked ? acc()     : txt2(),
                 liked ? "#3a1500" : "#2D2D2D",
                 acc()));

    int cid = pid;
    connect(upBtn, &QPushButton::clicked, this, [=]() {
        QSqlQuery lq;
        lq.prepare("INSERT OR IGNORE INTO post_likes(post_id,username) VALUES(:p,:u)");
        lq.bindValue(":p", cid); lq.bindValue(":u", me);
        if (lq.exec() && lq.numRowsAffected() > 0) {
            QSqlQuery u2;
            u2.prepare("UPDATE posts SET likes_count=likes_count+1 WHERE id=:p");
            u2.bindValue(":p", cid); u2.exec();
            logActivity("upvoted"); loadFeed();
        }
    });

    QSqlQuery cc;
    cc.prepare("SELECT COUNT(*) FROM comments WHERE post_id=:p");
    cc.bindValue(":p", pid); cc.exec(); cc.next();
    int ccount = cc.value(0).toInt();

    auto *cmtBtn = new QPushButton(QString("  %1 Comments").arg(ccount));
    cmtBtn->setFixedHeight(30);
    cmtBtn->setStyleSheet(
        QString("QPushButton { background:transparent; color:%1; border-radius:5px;"
                "padding:0 12px; font-size:12px; font-weight:600; border:none; }"
                "QPushButton:hover { background:%2; color:%1; }").arg(txt(), card2()));

    abl->addWidget(upBtn); abl->addWidget(cmtBtn); abl->addStretch();

    // Delete button (own posts only)
    if (author == me) {
        auto *del = new QPushButton("Delete");
        del->setFixedHeight(30);
        del->setStyleSheet(
            QString("QPushButton { background:transparent; color:%1; border-radius:5px;"
                    "padding:0 12px; font-size:12px; font-weight:600; border:none; }"
                    "QPushButton:hover { background:#2D1215; color:%2; }").arg(txt(), err()));
        connect(del, &QPushButton::clicked, this, [=]() {
            if (QMessageBox::question(this, "Delete Post",
                                      "Are you sure you want to delete this post?") == QMessageBox::Yes) {
                QSqlQuery dq;
                dq.prepare("DELETE FROM posts WHERE id=:p AND author_username=:u");
                dq.bindValue(":p", cid); dq.bindValue(":u", me); dq.exec(); loadFeed();
            }
        });
        abl->addWidget(del);
    }
    lay->addWidget(actBar);

    // ── Comment section (hidden until toggled) ────────────────
    auto *cmtSec = new QFrame;
    cmtSec->setStyleSheet(
        QString("QFrame { background:%1; border-radius:8px; border:none; }").arg(card2()));
    cmtSec->hide();
    auto *csLay = new QVBoxLayout(cmtSec);
    csLay->setContentsMargins(12, 10, 12, 10); csLay->setSpacing(8);

    auto *cmtContainer = new QWidget;
    cmtContainer->setStyleSheet("background:transparent;");
    auto *cmtLayout = new QVBoxLayout(cmtContainer);
    cmtLayout->setSpacing(6); cmtLayout->setAlignment(Qt::AlignTop);

    auto *inputRow  = new QHBoxLayout;
    auto *cmtInput  = new QLineEdit;
    cmtInput->setPlaceholderText("Add a comment...");
    cmtInput->setMinimumHeight(36); cmtInput->setStyleSheet(FS());

    auto *addBtn = new QPushButton("Reply");
    addBtn->setFixedHeight(36); addBtn->setFixedWidth(70);
    addBtn->setStyleSheet(btnPrimary());
    connect(addBtn, &QPushButton::clicked, this, [=]() {
        QString ct = cmtInput->text().trimmed();
        if (ct.isEmpty()) return;
        QSqlQuery cq;
        cq.prepare("INSERT INTO comments(post_id,author_username,text) VALUES(:p,:a,:t)");
        cq.bindValue(":p", cid); cq.bindValue(":a", me); cq.bindValue(":t", ct);
        if (cq.exec()) {
            cmtInput->clear(); logActivity("commented");
            QLayoutItem *ci;
            while ((ci = cmtLayout->takeAt(0)) != nullptr) {
                if (ci->widget()) ci->widget()->deleteLater(); delete ci;
            }
            loadComments(cmtLayout, cid); loadFeed();
        }
    });
    connect(cmtInput, &QLineEdit::returnPressed, addBtn, &QPushButton::click);

    inputRow->addWidget(cmtInput); inputRow->addWidget(addBtn);
    csLay->addWidget(cmtContainer); csLay->addLayout(inputRow);
    lay->addWidget(cmtSec);

    connect(cmtBtn, &QPushButton::clicked, this, [=]() mutable {
        bool vis = cmtSec->isVisible();
        cmtSec->setVisible(!vis);
        if (!vis) {
            QLayoutItem *ci;
            while ((ci = cmtLayout->takeAt(0)) != nullptr) {
                if (ci->widget()) ci->widget()->deleteLater(); delete ci;
            }
            loadComments(cmtLayout, cid);
        }
    });

    feedLayout->addWidget(postCard);
}

void MainWindow::loadComments(QVBoxLayout *cmtLayout, int postId)
{
    QSqlQuery q;
    q.prepare("SELECT author_username,text,created_at FROM comments "
              "WHERE post_id=:p ORDER BY created_at ASC");
    q.bindValue(":p", postId); q.exec();
    bool any = false;
    while (q.next()) {
        any = true;
        QString author = q.value(0).toString();
        QString text   = q.value(1).toString();
        QString time   = q.value(2).toString().left(16);

        auto *row = new QWidget; row->setStyleSheet("background:transparent;");
        auto *rl  = new QHBoxLayout(row);
        rl->setContentsMargins(0, 4, 0, 4); rl->setSpacing(10);

        auto *av = new QLabel;
        av->setPixmap(makeAvatar(author.isEmpty() ? "?" : QString(author[0]), 26, avatarColor(author)));
        av->setStyleSheet("border:none; background:transparent;");

        auto *col     = new QVBoxLayout; col->setSpacing(2);
        auto *nameRow = new QHBoxLayout;
        auto *nameLbl = new QLabel("u/" + author);
        nameLbl->setStyleSheet(
            QString("color:%1; font-size:11px; font-weight:700;"
                    "background:transparent; border:none;").arg(txt()));
        auto *timeLbl = new QLabel(time);
        timeLbl->setStyleSheet(
            QString("color:%1; font-size:10px; background:transparent; border:none;").arg(txt2()));
        nameRow->addWidget(nameLbl); nameRow->addWidget(timeLbl); nameRow->addStretch();

        auto *textLbl = new QLabel(text); textLbl->setWordWrap(true);
        textLbl->setStyleSheet(
            QString("color:%1; font-size:12px; background:transparent; border:none;").arg(txt()));

        col->addLayout(nameRow); col->addWidget(textLbl);
        rl->addWidget(av); rl->addLayout(col);
        cmtLayout->addWidget(row);
    }
    if (!any) {
        auto *e = new QLabel("No comments yet. Be the first to comment.");
        e->setStyleSheet(
            QString("color:%1; font-size:11px; padding:4px;"
                    "background:transparent; border:none;").arg(txt2()));
        cmtLayout->addWidget(e);
    }
}

void MainWindow::onLogoutClicked()
{
    logActivity("logout"); me.clear();
    loginUser->clear(); loginPass->clear(); loginCap->clear();
    refreshCaptcha(); showLogin();
}

// ════════════════════════════════════════════════════════════════
//  FRIENDS PANEL
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildFriendsPanel()
{
    auto *panel = new QWidget;
    auto *lay   = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0); lay->setSpacing(0);

    // Top bar
    auto *topBar = new QFrame; topBar->setFixedHeight(56);
    topBar->setObjectName("topBar"); // <--- Replaces the inline stylesheet

    auto *tbl = new QHBoxLayout(topBar); tbl->setContentsMargins(24, 0, 24, 0);
    auto *title = new QLabel("Friends");

    // Remove 'color:%1;' so it inherits from the global QLabel style
    title->setStyleSheet("font-size:16px; font-weight:700; background:transparent; border:none;");

    // Scroll area
    auto *scroll  = new QScrollArea; scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border:none; background:transparent; }");

    auto *content = new QWidget;
    // <--- DELETE the content->setStyleSheet(QString("background:%1;").arg(bg())); line completely.
    // The global "QWidget" rule will automatically give it the correct background.

    auto *cl = new QVBoxLayout(content);

    // ── Find Students card ────────────────────────────────────
    auto *sc  = new QFrame;
    sc->setObjectName("cardFrame"); // <--- Replaces the inline stylesheet

    auto *scl = new QVBoxLayout(sc);
    // ...
    auto *sttl = new QLabel("Find Students");
    sttl->setStyleSheet("font-size:14px; font-weight:700; background:transparent; border:none;"); // <--- Removed color

    searchInput = new QLineEdit;
    searchInput->setPlaceholderText("Search by username...");
    searchInput->setMinimumHeight(40);
    // <--- DELETE searchInput->setStyleSheet(FS());
    // Your global QLineEdit stylesheet is identical to FS() and will handle it natively.
    connect(searchInput, &QLineEdit::textChanged, this, &MainWindow::onSearchChanged);

    auto *resW = new QWidget; resW->setStyleSheet("background:transparent;");
    searchResLayout = new QVBoxLayout(resW);
    searchResLayout->setAlignment(Qt::AlignTop); searchResLayout->setSpacing(6);

    auto *resScroll = new QScrollArea; resScroll->setWidgetResizable(true);
    resScroll->setMaximumHeight(200);
    resScroll->setStyleSheet("QScrollArea { border:none; background:transparent; }");
    resScroll->setWidget(resW);

    scl->addWidget(sttl); scl->addWidget(searchInput); scl->addWidget(resScroll);

    // ── Pending Requests card ─────────────────────────────────
    auto *pc  = new QFrame;
    pc->setObjectName("cardFrame"); // <--- Replaces the inline stylesheet

    auto *pcl = new QVBoxLayout(pc);
    // ...
    auto *pttl = new QLabel("Pending Requests");
    pttl->setStyleSheet("font-size:14px; font-weight:700; background:transparent; border:none;"); // <--- Removed color
    pcl->addWidget(pttl);
    pendingLayout = new QVBoxLayout;
    pendingLayout->setAlignment(Qt::AlignTop); pendingLayout->setSpacing(6);
    pcl->addLayout(pendingLayout);

    // ── My Friends card ───────────────────────────────────────
    auto *fc  = new QFrame;
    fc->setObjectName("cardFrame"); // <--- Replaces the inline stylesheet

    auto *fcl = new QVBoxLayout(fc);
    // ...
    auto *fttl = new QLabel("My Friends");
    fttl->setStyleSheet("font-size:14px; font-weight:700; background:transparent; border:none;"); // <--- Removed color
    fcl->addWidget(fttl);
    friendsLayout = new QVBoxLayout;
    friendsLayout->setAlignment(Qt::AlignTop); friendsLayout->setSpacing(6);
    fcl->addLayout(friendsLayout);

    cl->addWidget(sc); cl->addWidget(pc); cl->addWidget(fc);
    scroll->setWidget(content);
    lay->addWidget(topBar); lay->addWidget(scroll);
    return panel;
}

// FIX: all labels explicitly styled with txt() so they're readable in light mode
void MainWindow::onSearchChanged(const QString &text)
{
    QLayoutItem *it;
    while ((it = searchResLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater(); delete it;
    }
    if (text.trimmed().length() < 2) return;

    QSqlQuery q;
    q.prepare("SELECT username FROM users WHERE username LIKE :q AND username!=:me LIMIT 8");
    q.bindValue(":q", "%" + text + "%"); q.bindValue(":me", me); q.exec();

    while (q.next()) {
        QString uname = q.value(0).toString();

        auto *row = new QWidget; row->setStyleSheet("background:transparent;");
        auto *rl  = new QHBoxLayout(row);
        rl->setContentsMargins(0, 4, 0, 4); rl->setSpacing(10);

        auto *av = new QLabel;
        av->setPixmap(makeAvatar(uname.isEmpty() ? "?" : QString(uname[0]), 34, avatarColor(uname)));
        av->setStyleSheet("border:none; background:transparent;");

        // FIX: explicitly use txt() instead of relying on the palette
        auto *un = new QLabel("u/" + uname);
        un->setStyleSheet(
            QString("color:%1; font-size:13px; font-weight:600;"
                    "background:transparent; border:none;").arg(txt()));

        QSqlQuery fq;
        fq.prepare("SELECT status FROM friendships WHERE username=:u AND friend_username=:f");
        fq.bindValue(":u", me); fq.bindValue(":f", uname); fq.exec();

        auto *addBtn = new QPushButton; addBtn->setFixedHeight(30);
        if (fq.next()) {
            QString st = fq.value(0).toString();
            addBtn->setText(st == "accepted" ? "Connected" : "Pending");
            addBtn->setEnabled(false);
            addBtn->setStyleSheet(
                QString("QPushButton { background:%1; color:%2; border-radius:5px;"
                        "padding:0 14px; font-size:12px; font-weight:600; border:none; }")
                    .arg(card2(), st == "accepted" ? ok() : txt2()));
        } else {
            addBtn->setText("+ Follow"); addBtn->setStyleSheet(btnPrimary());
            QString target = uname;
            connect(addBtn, &QPushButton::clicked, this, [=]() {
                QSqlQuery sq;
                sq.prepare("INSERT OR IGNORE INTO friendships(username,friend_username,status)"
                           "VALUES(:u,:f,'pending')");
                sq.bindValue(":u", me); sq.bindValue(":f", target); sq.exec();
                logActivity("sent_request"); onSearchChanged(searchInput->text());
            });
        }
        rl->addWidget(av); rl->addWidget(un); rl->addStretch(); rl->addWidget(addBtn);
        searchResLayout->addWidget(row);
    }
}

void MainWindow::loadPending()
{
    QLayoutItem *it;
    while ((it = pendingLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater(); delete it;
    }
    QSqlQuery q;
    q.prepare("SELECT username FROM friendships WHERE friend_username=:me AND status='pending'");
    q.bindValue(":me", me); q.exec();
    bool any = false;

    while (q.next()) {
        any = true;
        QString from = q.value(0).toString();

        auto *row = new QWidget; row->setStyleSheet("background:transparent;");
        auto *rl  = new QHBoxLayout(row);
        rl->setContentsMargins(0, 4, 0, 4); rl->setSpacing(10);

        auto *av = new QLabel;
        av->setPixmap(makeAvatar(from.isEmpty() ? "?" : QString(from[0]), 34, avatarColor(from)));
        av->setStyleSheet("border:none; background:transparent;");

        auto *lbl = new QLabel("u/" + from + " wants to connect");
        lbl->setStyleSheet(
            QString("color:%1; font-size:13px; background:transparent; border:none;").arg(txt()));

        auto *accBtn = new QPushButton("Accept");
        accBtn->setFixedHeight(30); accBtn->setFixedWidth(80); accBtn->setStyleSheet(btnPrimary());

        auto *rejBtn = new QPushButton("Decline");
        rejBtn->setFixedHeight(30); rejBtn->setFixedWidth(80); rejBtn->setStyleSheet(btnSecondary());

        QString sender = from;
        connect(accBtn, &QPushButton::clicked, this, [=]() {
            QSqlQuery a1, a2;
            a1.prepare("UPDATE friendships SET status='accepted'"
                       " WHERE username=:f AND friend_username=:me");
            a1.bindValue(":f", sender); a1.bindValue(":me", me); a1.exec();
            a2.prepare("INSERT OR IGNORE INTO friendships(username,friend_username,status)"
                       "VALUES(:me,:f,'accepted')");
            a2.bindValue(":me", me); a2.bindValue(":f", sender); a2.exec();
            loadPending(); loadFriends();
        });
        connect(rejBtn, &QPushButton::clicked, this, [=]() {
            QSqlQuery rq;
            rq.prepare("DELETE FROM friendships WHERE username=:f AND friend_username=:me");
            rq.bindValue(":f", sender); rq.bindValue(":me", me); rq.exec(); loadPending();
        });

        rl->addWidget(av); rl->addWidget(lbl); rl->addStretch();
        rl->addWidget(accBtn); rl->addWidget(rejBtn);
        pendingLayout->addWidget(row);
    }
    if (!any) {
        auto *e = new QLabel("No pending requests.");
        e->setStyleSheet(
            QString("color:%1; font-size:12px; padding:4px 0;"
                    "background:transparent; border:none;").arg(txt2()));
        pendingLayout->addWidget(e);
    }
}

void MainWindow::loadFriends()
{
    QLayoutItem *it;
    while ((it = friendsLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater(); delete it;
    }
    QSqlQuery q;
    q.prepare("SELECT friend_username FROM friendships WHERE username=:u AND status='accepted'");
    q.bindValue(":u", me); q.exec();
    bool any = false;

    while (q.next()) {
        any = true;
        QString fn = q.value(0).toString();

        auto *row = new QWidget; row->setStyleSheet("background:transparent;");
        auto *rl  = new QHBoxLayout(row);
        rl->setContentsMargins(0, 4, 0, 4); rl->setSpacing(10);

        auto *av = new QLabel;
        av->setPixmap(makeAvatar(fn.isEmpty() ? "?" : QString(fn[0]), 34, avatarColor(fn)));
        av->setStyleSheet("border:none; background:transparent;");

        auto *lbl = new QLabel("u/" + fn);
        lbl->setStyleSheet(
            QString("color:%1; font-size:13px; font-weight:600;"
                    "background:transparent; border:none;").arg(txt()));

        auto *dot = new QLabel("● Active");
        dot->setStyleSheet(
            QString("color:%1; font-size:11px; background:transparent; border:none;").arg(ok()));

        rl->addWidget(av); rl->addWidget(lbl); rl->addStretch(); rl->addWidget(dot);
        friendsLayout->addWidget(row);
    }
    if (!any) {
        auto *e = new QLabel("No connections yet. Search above to connect with classmates.");
        e->setStyleSheet(
            QString("color:%1; font-size:12px; padding:4px 0;"
                    "background:transparent; border:none;").arg(txt2()));
        friendsLayout->addWidget(e);
    }
}

// ════════════════════════════════════════════════════════════════
//  MESSAGES PANEL
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildMessagesPanel()
{
    auto *panel = new QWidget;
    auto *lay   = new QHBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0); lay->setSpacing(0);

    // ── Contacts sidebar ─────────────────────────────────────────
    msgSidebar = new QFrame; msgSidebar->setFixedWidth(260);
    msgSidebar->setStyleSheet(
        QString("QFrame { background:%1; border-right:1px solid %2; border-radius:0; }")
            .arg(card(), bor()));
    auto *csl = new QVBoxLayout(msgSidebar);
    csl->setContentsMargins(0, 0, 0, 0); csl->setSpacing(0);

    auto *msHeader = new QFrame; msHeader->setFixedHeight(56);
    msHeader->setObjectName("topBar"); // <--- Replaces the inline stylesheet

    auto *mshl = new QHBoxLayout(msHeader); mshl->setContentsMargins(16, 0, 16, 0);
    auto *msTtl = new QLabel("Messages");

    // <--- Removed 'color:%1;'
    msTtl->setStyleSheet("font-size:15px; font-weight:700; background:transparent; border:none;");

    mshl->addWidget(msTtl);
    csl->addWidget(msHeader);

    contactsList = new QListWidget;
    contactsList->setStyleSheet(
        QString("QListWidget { background:transparent; border:none; color:%1; }"
                "QListWidget::item { color:%1; padding:12px 16px; border-radius:0;"
                "  margin:0; border-bottom:1px solid %2; }"
                "QListWidget::item:hover    { background:%3; }"
                "QListWidget::item:selected { background:%3; color:%1;"
                "  border-left:3px solid %4; }")
            .arg(txt(), bor(), card2(), acc()));
    connect(contactsList, &QListWidget::itemClicked, this, &MainWindow::onContactSelected);
    csl->addWidget(contactsList);

    // ── Chat area ────────────────────────────────────────────────
    auto *chatArea = new QWidget;
    // <--- DELETE THIS LINE: chatArea->setStyleSheet(QString("background:%1;").arg(bg()));
    auto *cl = new QVBoxLayout(chatArea);
    cl->setContentsMargins(0, 0, 0, 0); cl->setSpacing(0);

    chatHeader = new QLabel("  Select a conversation");
    chatHeader->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    chatHeader->setFixedHeight(56);
    chatHeader->setStyleSheet(
        QString("color:%1; font-size:14px; font-weight:700; padding:0 20px;"
                "background:%2; border-bottom:1px solid %3; border-radius:0;")
            .arg(txt(), card(), bor()));

    auto *scroll = new QScrollArea; scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border:none; background:transparent; }");
    auto *sw = new QWidget; sw->setStyleSheet("background:transparent;");
    chatLayout = new QVBoxLayout(sw);
    chatLayout->setSpacing(8); chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setContentsMargins(20, 20, 20, 20);
    scroll->setWidget(sw);

    msgInputRow = new QFrame;
    msgInputRow->setStyleSheet(
        QString("QFrame { background:%1; border-top:1px solid %2; border-radius:0; }")
            .arg(card(), bor()));
    auto *irl = new QHBoxLayout(msgInputRow);
    irl->setContentsMargins(16, 12, 16, 12); irl->setSpacing(10);
    msgInput = new QLineEdit;
    msgInput->setPlaceholderText("Write a message...");
    msgInput->setMinimumHeight(40); msgInput->setStyleSheet(FS());
    connect(msgInput, &QLineEdit::returnPressed, this, &MainWindow::onSendMessage);
    auto *sendBtn = new QPushButton("Send");
    sendBtn->setFixedHeight(40); sendBtn->setFixedWidth(80);
    sendBtn->setStyleSheet(btnPrimary());
    connect(sendBtn, &QPushButton::clicked, this, &MainWindow::onSendMessage);
    irl->addWidget(msgInput); irl->addWidget(sendBtn);

    cl->addWidget(chatHeader); cl->addWidget(scroll, 1); cl->addWidget(msgInputRow);
    lay->addWidget(msgSidebar); lay->addWidget(chatArea, 1);
    return panel;
}

void MainWindow::loadContacts()
{
    contactsList->clear(); chatWith.clear();
    chatHeader->setText("  Select a conversation");
    QLayoutItem *it;
    while ((it = chatLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater(); delete it;
    }
    QSqlQuery q;
    q.prepare("SELECT friend_username FROM friendships WHERE username=:u AND status='accepted'");
    q.bindValue(":u", me); q.exec();
    while (q.next()) {
        QString fn  = q.value(0).toString();
        auto *item  = new QListWidgetItem("  u/" + fn);
        item->setForeground(QColor(txt()));
        item->setData(Qt::UserRole, fn);
        contactsList->addItem(item);
    }
    if (contactsList->count() == 0) {
        auto *item = new QListWidgetItem("  No connections yet");
        item->setFlags(Qt::NoItemFlags);
        item->setForeground(QColor(txt2()));
        contactsList->addItem(item);
    }
}

void MainWindow::onContactSelected(QListWidgetItem *item)
{
    QString u = item->data(Qt::UserRole).toString();
    if (u.isEmpty()) return;
    chatWith = u; chatHeader->setText("  u/" + u); loadChat(u);
}

void MainWindow::loadChat(const QString &u)
{
    QLayoutItem *it;
    while ((it = chatLayout->takeAt(0)) != nullptr) {
        if (it->widget()) it->widget()->deleteLater(); delete it;
    }
    QSqlQuery q;
    q.prepare("SELECT sender_username,message_text,sent_at FROM messages "
              "WHERE (sender_username=:me AND receiver_username=:u)"
              "   OR (sender_username=:u  AND receiver_username=:me)"
              " ORDER BY sent_at ASC");
    q.bindValue(":me", me); q.bindValue(":u", u); q.exec();
    bool any = false;
    while (q.next()) {
        any = true;
        bool mine  = q.value(0).toString() == me;
        QString msg = q.value(1).toString();
        QString t   = q.value(2).toString().left(16);

        auto *row    = new QHBoxLayout;
        auto *bubble = new QFrame; bubble->setMaximumWidth(440);
        bubble->setStyleSheet(
            QString("QFrame { background:%1; border-radius:14px; border:none; %2 }")
                .arg(mine ? acc() : card(),
                     mine ? "border-bottom-right-radius:4px;" : "border-bottom-left-radius:4px;"));

        auto *bl = new QVBoxLayout(bubble);
        bl->setContentsMargins(14, 10, 14, 10); bl->setSpacing(4);

        auto *ml = new QLabel(msg); ml->setWordWrap(true);
        ml->setStyleSheet(
            QString("color:%1; font-size:13px; background:transparent; border:none;")
                .arg(mine ? "white" : txt()));
        auto *tl = new QLabel(t);
        tl->setStyleSheet(
            QString("color:%1; font-size:10px; background:transparent; border:none;")
                .arg(mine ? "rgba(255,255,255,0.7)" : txt2()));

        bl->addWidget(ml); bl->addWidget(tl);
        if (mine) { row->addStretch(); row->addWidget(bubble); }
        else       { row->addWidget(bubble); row->addStretch(); }

        auto *w = new QWidget; w->setStyleSheet("background:transparent;");
        auto *wl = new QVBoxLayout(w); wl->setContentsMargins(0, 0, 0, 0); wl->addLayout(row);
        chatLayout->addWidget(w);
    }
    if (!any) {
        auto *e = new QLabel("No messages yet. Start the conversation.");
        e->setAlignment(Qt::AlignCenter);
        e->setStyleSheet(
            QString("color:%1; font-size:13px; padding:40px;"
                    "background:transparent; border:none;").arg(txt2()));
        chatLayout->addWidget(e);
    }
}

void MainWindow::onSendMessage()
{
    if (chatWith.isEmpty() || msgInput->text().trimmed().isEmpty()) return;
    QSqlQuery q;
    q.prepare("INSERT INTO messages(sender_username,receiver_username,message_text)"
              "VALUES(:s,:r,:t)");
    q.bindValue(":s", me); q.bindValue(":r", chatWith); q.bindValue(":t", msgInput->text().trimmed());
    if (q.exec()) { msgInput->clear(); loadChat(chatWith); }
}

// ════════════════════════════════════════════════════════════════
//  PROFILE PANEL
//  FIX: avatar is now placed BELOW the banner, properly centred
// ════════════════════════════════════════════════════════════════
QWidget* MainWindow::buildProfilePanel()
{
    auto *panel = new QWidget;
    auto *lay   = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0); lay->setSpacing(0);

    // Top bar
    auto *topBar = new QFrame; topBar->setFixedHeight(56);
    topBar->setObjectName("topBar"); // <--- Replaces the inline stylesheet

    auto *tbl = new QHBoxLayout(topBar); tbl->setContentsMargins(24, 0, 24, 0);
    auto *title = new QLabel("Profile");

    // <--- Removed 'color:%1;'
    title->setStyleSheet("font-size:16px; font-weight:700; background:transparent; border:none;");
    tbl->addWidget(title); tbl->addStretch();

    // Scroll area
    auto *scroll  = new QScrollArea; scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border:none; background:transparent; }");

    auto *content = new QWidget;
    // <--- DELETE THIS LINE: content->setStyleSheet(QString("background:%1;").arg(bg()));
    auto *cl = new QVBoxLayout(content);
    cl->setContentsMargins(24, 20, 24, 20); cl->setSpacing(12);

    // ── Profile card ─────────────────────────────────────────────
    auto *pcard = new QFrame;
    pcard->setObjectName("cardFrame");
    auto *pcl = new QVBoxLayout(pcard);
    pcl->setContentsMargins(0, 0, 0, 0); pcl->setSpacing(0);

    // Orange gradient banner
    auto *banner = new QFrame; banner->setFixedHeight(100);
    banner->setStyleSheet(
        QString("QFrame { background:qlineargradient(x1:0,y1:0,x2:1,y2:0,"
                "stop:0 %1,stop:1 #FF8717); border-radius:8px; border:none; }").arg(acc()));
    pcl->addWidget(banner);

    // Add physical space to push the text down below the overlapping avatar
    pcl->addSpacing(44);

    // Info section (Text)
    auto *infoWidget = new QWidget; infoWidget->setStyleSheet("background:transparent;");
    auto *infoLay    = new QVBoxLayout(infoWidget);
    infoLay->setContentsMargins(24, 0, 24, 20); infoLay->setSpacing(4);

    profUser = new QLabel;
    profUser->setStyleSheet("font-size:20px; font-weight:700; background:transparent; border:none;");

    profBio = new QLabel; profBio->setWordWrap(true);
    profBio->setStyleSheet("font-size:13px; background:transparent; border:none;");

    profStats = new QLabel;
    profStats->setStyleSheet("font-size:12px; background:transparent; border:none;");

    // Thin divider
    auto *div = new QFrame;
    div->setFrameShape(QFrame::HLine);
    div->setStyleSheet(QString("background:%1; border:none;").arg(bor()));
    div->setFixedHeight(1);

    // Action buttons
    auto *actRow = new QHBoxLayout; actRow->setSpacing(8);

    auto *editBtn = new QPushButton("Edit Bio");
    editBtn->setFixedHeight(36); editBtn->setFixedWidth(120);
    editBtn->setStyleSheet(btnPrimary());
    connect(editBtn, &QPushButton::clicked, this, [=]() {
        bool ok2;
        QString bio = QInputDialog::getMultiLineText(
            this, "Edit Bio", "Tell others about yourself:", profBio->text(), &ok2);
        if (ok2) {
            QSqlQuery q;
            q.prepare("UPDATE users SET bio=:b WHERE username=:u");
            q.bindValue(":b", bio); q.bindValue(":u", me); q.exec();
            refreshProfile();
        }
    });

    auto *expBtn = new QPushButton("Export Posts");
    expBtn->setFixedHeight(36); expBtn->setStyleSheet(btnSecondary());
    connect(expBtn, &QPushButton::clicked, this, [=]() {
        QString fp = QFileDialog::getSaveFileName(
            this, "Save CSV",
            QCoreApplication::applicationDirPath() + "/" + me + "_posts.csv",
            "CSV Files (*.csv)");
        if (fp.isEmpty()) return;
        std::ofstream f(fp.toStdString()); if (!f.is_open()) return;
        f << "ID,Author,Text,Upvotes,Date\n";
        QSqlQuery q;
        q.prepare("SELECT id,text,likes_count,created_at FROM posts WHERE author_username=:u");
        q.bindValue(":u", me); q.exec(); int cnt = 0;
        while (q.next()) {
            cnt++;
            f << q.value(0).toInt() << "," << me.toStdString() << ","
              << "\"" + q.value(1).toString().replace("\"","\"\"").toStdString() + "\","
              << q.value(2).toInt() << "," << q.value(3).toString().toStdString() << "\n";
        }
        f.close();
        QMessageBox::information(this, "Exported", QString("Saved %1 posts.").arg(cnt));
    });

    auto *logBtn = new QPushButton("Activity Log");
    logBtn->setFixedHeight(36); logBtn->setStyleSheet(btnSecondary());
    connect(logBtn, &QPushButton::clicked, this, [=]() {
        QSqlQuery q;
        q.prepare("SELECT action,logged_at FROM activity_log "
                  "WHERE username=:u ORDER BY logged_at DESC LIMIT 20");
        q.bindValue(":u", me); q.exec();
        QString log = "Recent Activity\n\n";
        while (q.next())
            log += q.value(1).toString().left(16) + "   " + q.value(0).toString() + "\n";
        QMessageBox::information(this, "Activity Log", log);
    });

    actRow->addWidget(editBtn); actRow->addWidget(expBtn); actRow->addWidget(logBtn);
    actRow->addStretch();

    infoLay->addWidget(profUser);
    infoLay->addWidget(profBio);
    infoLay->addSpacing(8);
    infoLay->addWidget(profStats);
    infoLay->addSpacing(12);
    infoLay->addWidget(div);
    infoLay->addSpacing(8);
    infoLay->addLayout(actRow);
    // ── Absolute Positioned Avatar ───────────────────────────────
    auto *av = new QLabel(pcard); // Parented directly to pcard for absolute positioning
    av->setObjectName("profileAvatar");
    av->setAlignment(Qt::AlignCenter);

    // Size = 78x78 (72px image + 3px left border + 3px right border)
    av->setFixedSize(78, 78);

    // Move it to overlap exactly (x=24, y = 100 banner height - 39 half height)
    av->move(24, 61);
    av->setStyleSheet(
        QString("border:3px solid %1; border-radius:39px; background:transparent;").arg(card()));

    cl->addWidget(pcard);

    pcl->addWidget(infoWidget);
    scroll->setWidget(content);
    lay->addWidget(topBar); lay->addWidget(scroll);
    return panel;
}

void MainWindow::refreshProfile()
{
    QSqlQuery q;
    q.prepare("SELECT bio,joined_at FROM users WHERE username=:u");
    q.bindValue(":u", me); q.exec();
    QString bio, joined;
    if (q.next()) { bio = q.value(0).toString(); joined = q.value(1).toString().left(10); }

    QSqlQuery pq, fq;
    pq.prepare("SELECT COUNT(*) FROM posts WHERE author_username=:u");
    pq.bindValue(":u", me); pq.exec(); pq.next();
    fq.prepare("SELECT COUNT(*) FROM friendships WHERE username=:u AND status='accepted'");
    fq.bindValue(":u", me); fq.exec(); fq.next();

    profUser->setText("u/" + me);
    profBio->setText(bio.isEmpty() ? "No bio yet. Click Edit Bio to add one." : bio);
    profStats->setText(
        QString("%1 Posts   ·   %2 Connections   ·   Joined %3")
            .arg(pq.value(0).toInt()).arg(fq.value(0).toInt()).arg(joined));

    // FIX: use objectName lookup to update the avatar; also restyle its border for current theme
    auto *avW = contentStack->widget(3)->findChild<QLabel*>("profileAvatar");
    if (avW) {
        avW->setPixmap(makeAvatar(me.isEmpty() ? "?" : QString(me[0]), 72, avatarColor(me)));
        avW->setStyleSheet(
            // Change the border-radius here to 39px as well!
            QString("border:3px solid %1; border-radius:39px; background:transparent;").arg(card()));
    }
}

// ════════════════════════════════════════════════════════════════
//  UTILITIES
// ════════════════════════════════════════════════════════════════
QString MainWindow::hashPw(const QString &p)
{
    return QString(QCryptographicHash::hash(p.toUtf8(), QCryptographicHash::Sha256).toHex());
}