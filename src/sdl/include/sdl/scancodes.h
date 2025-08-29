/**
 * @brief Keyboard Scancode enumeration
 * 
 */
enum class ScanCode {
  kUnknown,
  // Letters
  kA, kB, kC, kD, kE, kF, kG, kH, kI, kJ, kK, kL, kM, kN, kO, kP,
  kQ, kR, kS, kT, kU, kV, kW, kX, kY, kZ,
  // Numbers
  k1, k2, k3, k4, k5, k6, k7, k8, k9, k0,
  // Common keys
  kReturn, kEscape, kBackspace, kTab, kSpace, kMinus, kEquals, 
  kLeftBracket, kRightBracket, kBackslash, kNonUsHash, kSemicolon, 
  kApostrophe, kGrave, kComma, kPeriod, kSlash, kCapsLock,
  // Function keys
  kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8, kF9, kF10, kF11, kF12,
  kF13, kF14, kF15, kF16, kF17, kF18, kF19, kF20, kF21, kF22, kF23, kF24,
  // Navigation and editing keys
  kPrintScreen, kScrollLock, kPause, kInsert, kHome, kPageUp, kDelete, 
  kEnd, kPageDown, kRight, kLeft, kDown, kUp,
  // Keypad keys
  kNumLockClear, kKpDivide, kKpMultiply, kKpMinus, kKpPlus, kKpEnter,
  kKp1, kKp2, kKp3, kKp4, kKp5, kKp6, kKp7, kKp8, kKp9, kKp0, kKpPeriod, kKpEquals,
  // Extended keypad keys
  kKp00, kKp000, kThousandsSeparator, kDecimalSeparator, kCurrencyUnit, kCurrencySubunit,
  kKpLeftParen, kKpRightParen, kKpLeftBrace, kKpRightBrace, kKpTab, kKpBackspace,
  kKpA, kKpB, kKpC, kKpD, kKpE, kKpF, kKpXor, kKpPower, kKpPercent, kKpLess, kKpGreater,
  kKpAmpersand, kKpDblAmpersand, kKpVerticalBar, kKpDblVerticalBar, kKpColon, kKpHash,
  kKpSpace, kKpAt, kKpExclam, kKpMemStore, kKpMemRecall, kKpMemClear, kKpMemAdd,
  kKpMemSubtract, kKpMemMultiply, kKpMemDivide, kKpPlusMinus, kKpClear, kKpClearEntry,
  kKpBinary, kKpOctal, kKpDecimal, kKpHexadecimal, kKpComma, kKpEqualsAs400,
  // Special keys
  kNonUsBackslash, kApplication, kPower, kExecute, kHelp, kMenu, kSelect, kStop, 
  kAgain, kUndo, kCut, kCopy, kPaste, kFind, kMute, kVolumeUp, kVolumeDown, 
  kAlterErase, kSysReq, kCancel, kClear, kPrior, kReturn2, kSeparator, kOut, 
  kOper, kClearAgain, kCrSel, kExSel,
  // International keys
  kInternational1, kInternational2, kInternational3, kInternational4, kInternational5,
  kInternational6, kInternational7, kInternational8, kInternational9,
  // Language keys
  kLang1, kLang2, kLang3, kLang4, kLang5, kLang6, kLang7, kLang8, kLang9,
  // Modifier keys
  kLCtrl, kLShift, kLAlt, kLGui, kRCtrl, kRShift, kRAlt, kRGui, kMode,
  // Media and consumer keys
  kSleep, kWake, kChannelIncrement, kChannelDecrement, kMediaPlay, kMediaPause,
  kMediaRecord, kMediaFastForward, kMediaRewind, kMediaNextTrack, kMediaPreviousTrack,
  kMediaStop, kMediaEject, kMediaPlayPause, kMediaSelect,
  // Application control keys
  kAcNew, kAcOpen, kAcClose, kAcExit, kAcSave, kAcPrint, kAcProperties, kAcSearch,
  kAcHome, kAcBack, kAcForward, kAcStop, kAcRefresh, kAcBookmarks,
  // Mobile keys
  kSoftLeft, kSoftRight, kCall, kEndCall,
  // Reserved and meta
  kReserved, kCount
};
