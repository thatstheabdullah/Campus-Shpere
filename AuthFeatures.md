# Authentication Features
**Developed by: Abeeha Warda (25L-3073)**  
Module: UI & Authentication

## Features Implemented

### 1. CAPTCHA Verification System
- Randomly generated 5-character alphanumeric code
- Displayed in styled monospace font during login
- User must type code correctly to proceed
- Refresh button regenerates a new code
- Prevents automated/bot access

### 2. Live Password Strength Indicator
- Progress bar updates in real-time as user types
- Scoring criteria:
  - Length ≥ 6 characters: +20 points
  - Length ≥ 10 characters: +20 points
  - Contains uppercase letter: +20 points
  - Contains number: +20 points
  - Contains symbol: +20 points
- Visual feedback: Red (Weak) → Orange (Fair) → Blue (Good) → Green (Strong)
- Minimum score of 40 required to create account

### 3. Real-Time Input Validation
- Empty field detection with red error banners
- Username space/length validation
- Password confirmation matching
- Duplicate username check against database
- Wrong CAPTCHA feedback with auto-refresh

### 4. Secure Password Storage
- SHA-256 hashing via Qt's QCryptographicHash
- Plain text password NEVER stored in database
- Hash comparison on login attempt

## Code Location
All auth logic is in `mainwindow.cpp`:
- `onLoginClicked()` — login validation
- `onSignupClicked()` — registration validation  
- `onPasswordChanged()` — live strength update
- `refreshCaptcha()` — CAPTCHA generation
- `hashPw()` — SHA-256 hashing
