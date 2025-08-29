/**
 * @brief Keyboard Keycode enumeration
 * 
 */
enum class KeyCode {
  kUnknown,
  // Basic ASCII characters
  kReturn, kEscape, kBackspace, kTab, kSpace,
  kExclaim, kDblApostrophe, kHash, kDollar, kPercent, kAmpersand, kApostrophe,
  kLeftParen, kRightParen, kAsterisk, kPlus, kComma, kMinus, kPeriod, kSlash,
  // Numbers
  k0, k1, k2, k3, k4, k5, k6, k7, k8, k9,
  // More symbols
  kColon, kSemicolon, kLess, kEquals, kGreater, kQuestion, kAt,
  // Letters (lowercase)
  kA, kB, kC, kD, kE, kF, kG, kH, kI, kJ, kK, kL, kM, kN, kO, kP,
  kQ, kR, kS, kT, kU, kV, kW, kX, kY, kZ,
  // Brackets and symbols
  kLeftBracket, kBackslash, kRightBracket, kCaret, kUnderscore, kGrave,
  kLeftBrace, kPipe, kRightBrace, kTilde, kDelete,
  // Special characters
  kPlusMinus,
  // Function keys
  kCapsLock, kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8, kF9, kF10, kF11, kF12,
  kF13, kF14, kF15, kF16, kF17, kF18, kF19, kF20, kF21, kF22, kF23, kF24,
  // Navigation and editing keys
  kPrintScreen, kScrollLock, kPause, kInsert, kHome, kPageUp, kEnd, kPageDown,
  kRight, kLeft, kDown, kUp,
  // Keypad keys
  kNumLockClear, kKpDivide, kKpMultiply, kKpMinus, kKpPlus, kKpEnter,
  kKp1, kKp2, kKp3, kKp4, kKp5, kKp6, kKp7, kKp8, kKp9, kKp0, kKpPeriod, kKpEquals,
  // Application keys
  kApplication, kPower, kExecute, kHelp, kMenu, kSelect, kStop, kAgain, kUndo,
  kCut, kCopy, kPaste, kFind, kMute, kVolumeUp, kVolumeDown,
  // Modifier keys
  kLCtrl, kLShift, kLAlt, kLGui, kRCtrl, kRShift, kRAlt, kRGui,
  // Extended keys
  kMode, kSleep, kWake, kChannelIncrement, kChannelDecrement,
  kMediaPlay, kMediaPause, kMediaRecord, kMediaFastForward, kMediaRewind,
  kMediaNextTrack, kMediaPreviousTrack, kMediaStop, kMediaEject, kMediaPlayPause,
  // Application control keys
  kAcNew, kAcOpen, kAcClose, kAcExit, kAcSave, kAcPrint, kAcProperties,
  kAcSearch, kAcHome, kAcBack, kAcForward, kAcStop, kAcRefresh, kAcBookmarks,
  // Additional keys
  kBrightnessDown, kBrightnessUp, kDisplaySwitch, kKbdIllumToggle,
  kKbdIllumDown, kKbdIllumUp, kEject, kSysReq, kCancel, kClear, kPrior,
  kReturn2, kSeparator, kOut, kOper, kClearAgain, kCrSel, kExSel,
  // Mobile keys  
  kSoftLeft, kSoftRight, kCall, kEndCall
};