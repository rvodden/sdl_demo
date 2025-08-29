/**
 * @brief Keyboard Modifier enumeration
 * 
 */
enum class KeyModifier {
  kNone,     /**< no modifier is applicable. */
  kLShift,   /**< the left Shift key is down. */
  kRShift,   /**< the right Shift key is down. */
  kLevel5,   /**< the Level 5 Shift key is down. */
  kLCtrl,    /**< the left Ctrl (Control) key is down. */
  kRCtrl,    /**< the right Ctrl (Control) key is down. */
  kLAlt,     /**< the left Alt key is down. */
  kRAlt,     /**< the right Alt key is down. */
  kLGui,     /**< the left GUI key (often the Windows key) is down. */
  kRGui,     /**< the right GUI key (often the Windows key) is down. */
  kNum,      /**< the Num Lock key (may be located on an extended keypad) is down. */
  kCaps,     /**< the Caps Lock key is down. */
  kMode,     /**< the !AltGr key is down. */
  kScroll,   /**< the Scroll Lock key is down. */
  kCtrl,     /**< Any Ctrl key is down. */
  kShift,    /**< Any Shift key is down. */
  kAlt,      /**< Any Alt key is down. */
  kGui       /**< Any GUI key is down. */
};