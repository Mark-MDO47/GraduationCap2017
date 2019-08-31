// Test Buttons

// pushbutton inputs are D4 to D9
// README - the initialization code assumes these are contiguous and in order
#define PSHBTN1 4 // input
#define PSHBTN2 5 // input
#define PSHBTN3 6 // input
#define PSHBTN4 7 // input
#define PSHBTN5 8 // input
#define PSHBTN6 9 // input

#define IAMSYNC 11 // output
#define ALLSYNC 12 // input

#define SERIALPORT 1 // use serial port

#define DEBUG 1 // 1 = debug thru serial port, 0 = no debug
#define DEBUG2 0 // 1 = debug thru serial port, 0 = no debug

#if DEBUG
#define DEBUG_ERRORS_PRINT(param)   Serial.print((param));
#define DEBUG_ERRORS_PRINTLN(param) Serial.println((param));
// #define DEBUG_PRINTLN(param) Serial.println((param));
// #define DEBUG_PRINT(param)   Serial.print((param));
#define DEBUG_PRINTLN(param) // nothing
#define DEBUG_PRINT(param)   // nothing
// #define DEBUG2_PRINTLN(param) Serial.println((param));
// #define DEBUG2_PRINT(param)   Serial.print((param));
// #define DEBUG2_RETURN(p1, p2)  debug2_return((p1), (p2));
#define DEBUG2_PRINTLN(param) // nothing
#define DEBUG2_PRINT(param)   // nothing
#define DEBUG2_RETURN(p1, p2)  // nothing
// #define DEBUG3_PRINTLN(param) Serial.println((param));
// #define DEBUG3_PRINT(param)   Serial.print((param));
#define DEBUG3_PRINTLN(param) // nothing
#define DEBUG3_PRINT(param)   // nothing
#define DEBUG4_PRINTLN(param) Serial.println((param));
#define DEBUG4_PRINT(param)   Serial.print((param));
#endif // DEBUG


#define REAL_BUTTONS 1 // 1 = use buttons for input, 0 = use serial port
#if 0 == REAL_BUTTONS
#define SERIALPORT 1 // use serial port
#endif // REAL_BUTTONS

// iamNotSync() - sets our sync output to FALSE. note: #define
// iamSync()    - sets our sync output to TRUE. note: #define
// val = areWeAllSync() - returns nonzero if we are all sync
// val = iamSyncAreWeAllSync - sets our sync output TRUE and returns nonzero if we are all sync
// 
// setMySync(val) - sets sync to TRUE if val is nonzero, else sets sync to FALSE
//
#define iamSync()    setMySync(1)
#define iamNotSync() setMySync(0)

#if DEBUG
int16_t tmp_DEBUG = 0;
int16_t tmp_DEBUG2 = 0;
#endif // DEBUG


static int8_t   pattern = 1;
static int8_t   oldPattern = 2;
static int8_t   nextPattern = 2;
static int16_t  ptrn_delay = 100; // set by patterns to proper delay
static uint16_t bigCount;  // unsigned 16-bit int
static uint8_t  smallCount;  // unsigned  8-bit int

#define NO_BUTTON_PRESS -1 // when no input from user
#define NO_BUTTON_CHANGE -1 // when no CHANGE in input from user

// ******************************** SETUP ********************************
// setup()
//   initializes FastLED library for our config
//   initializes push button pins
//   initializes serial port
void setup() {
  delay(100); // for debugging & show
  
  // initialize the input pins. Not sure if my nano actually has a pullup...
  //    fortunately the PurseHanger controller board has a 10K pullup
  // README - this code assumes these are contiguous and in order
  for (int16_t thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
    pinMode(thePin, INPUT_PULLUP);
  } // end initialize pushbutton input pins
  // now the ALLSYNC input
  pinMode(ALLSYNC, INPUT_PULLUP);

  // now initialize the IAMSYNC output pin
  pinMode(IAMSYNC, INPUT_PULLUP);
  

#if (DEBUG+SERIALPORT)
  // README if debugging we want serial port
  Serial.begin(9600);
  Serial.println(F("setup grad cap"));
#endif // DEBUG

  pattern = 1; // FIXME - set to 1 when read pattern from buttons
  oldPattern = NO_BUTTON_CHANGE;
  nextPattern = NO_BUTTON_CHANGE;
} // end setup()

// ******************************** LOOP ********************************
void loop() {
  pattern = checkButtons();
  Serial.print(F("checkButtons ")); Serial.println((int16_t) pattern);
  delay(1000);

  /*
  nextPatternFromButtons();
  if ((NO_BUTTON_CHANGE != nextPattern) && (nextPattern != pattern)) {
    pattern = nextPattern;
  }
  nextPattern = NO_BUTTON_CHANGE;
  if (oldPattern != pattern) {
    Serial.print(F("switch to pattern ")); Serial.println((int16_t) pattern);
  }
  oldPattern = pattern;
  doDwell(ptrn_delay, 1);

  smallCount += 1;
  bigCount += 1;
  if (smallCount > 9) smallCount = 0;
  */
} // end loop()

// ******************************** UTILITIES ********************************


// doDwell(int16_t dwell, uint8_t must_be_diff_pattern) - dwell or break out if button press
//   returns TRUE if should switch to different pattern
//   else returns false
#define SMALL_DWELL 20
int16_t doDwell(int16_t dwell, uint8_t must_be_diff_pattern) {
  int16_t numloops = dwell / SMALL_DWELL;
  int16_t i;

  for (i = 0; i < numloops; i++) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(SMALL_DWELL);
  }
  if ((dwell % SMALL_DWELL) != 0) {
    nextPatternFromButtons();
    if ((0 != must_be_diff_pattern) && (nextPattern == pattern)) nextPattern = NO_BUTTON_CHANGE;
    if (nextPattern != NO_BUTTON_CHANGE) return(nextPattern != NO_BUTTON_CHANGE);
    delay(dwell % SMALL_DWELL);
  }
  return(nextPattern != NO_BUTTON_CHANGE);
} // end doDwell()

// doPtrnShowDwell() - break out if button press, then show, then dwell
// Used inside doPatternDraw
// Idea is to not do delays when drawing to target that is not visible
// Two things that matter: value returned and timing
//   value returned:
//     returns TRUE if should switch to different pattern
//     else returns false
//   timing:
//     if draw_target is TARGET_DSPLAY (visible display LEDs), either does entire delay or delays until button press
//     if draw_target is not TARGET_DSPLAY, returns immediately after checking for button press
int16_t doPtrnShowDwell(int16_t draw_target, int16_t dwell, int16_t called_from) {
  nextPatternFromButtons();
  if (nextPattern == pattern) nextPattern = NO_BUTTON_CHANGE;
  if ((nextPattern != pattern) && ( nextPattern != NO_BUTTON_CHANGE)) {
    DEBUG2_PRINTLN(F("  doPtrnShowDwell rtn; pattern != nextPattern"))
    return(nextPattern != NO_BUTTON_CHANGE);
  }
  return(doDwell(dwell, 1));
} // end doPtrnShowDwell()

// getButtonPress() - get next button press, true button or debugging
int16_t getButtonPress() {
#if REAL_BUTTONS
  return(checkButtons());
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  return(checkKeyboard());
#endif // not REAL_BUTTONS
} // end getButtonPress()

#if REAL_BUTTONS
  // checkButtons() - returns number of button pressed (1 through 6) or NO_BUTTON_PRESS
  int16_t checkButtons() {
    uint8_t  val;
    int16_t thePin;
    Serial.print(F("  pin"));
    for (thePin = PSHBTN1; thePin <= PSHBTN6; thePin ++) {
      val = digitalRead(thePin);
      Serial.print(F(" ")); Serial.print((int16_t) thePin); Serial.print(F("=")); Serial.print((int16_t) val);
      // if (LOW == val) break;
    } // end for all pushbuttons
    Serial.println("");
    if (PSHBTN6 < thePin) return(NO_BUTTON_PRESS); // if no button pushed
    else                  return(thePin-PSHBTN1+1);
  } // end checkButtons()
#else // end if REAL_BUTTONS; now NOT REAL_BUTTONS
  // checkKeyboard() - for debugging - serial port buttons
  int16_t checkKeyboard() { // not REAL_BUTTONS
    int8_t received_serial_input;
    int16_t myButton = NO_BUTTON_PRESS;
    if (Serial.available() > 0) {
      received_serial_input = Serial.read();
      switch ((int16_t) received_serial_input) {
        case (int16_t) '1': myButton = 1; break;
        case (int16_t) '2': myButton = 2; break;
        case (int16_t) '3': myButton = 3; break;
        case (int16_t) '4': myButton = 4; break;
        case (int16_t) '5': myButton = 5; break;
        case (int16_t) '6': myButton = 6; break;
        default: myButton = NO_BUTTON_PRESS; break;
      } // end switch on received serial "button"
    } // end if there was serial input ready to read
    return(myButton);
  } // end checkKeyboard()
#endif // not REAL_BUTTONS

// patternFromButtons() - get pattern to use (called from main loop)
// could have button pressed now - do that ignore any earlier press
// could have seen button pressed earlier and just now handling it - do that
// otherwise keep same pattern - no change
int16_t patternFromButtons() {
  int16_t myButton = getButtonPress(); // no change unless we see a change
  if (myButton == NO_BUTTON_PRESS) {
    if (NO_BUTTON_CHANGE != nextPattern) {
      myButton = nextPattern;
    } else {
      myButton = pattern;
    }
  } // end if no button pressed now so process earlier button press
  nextPattern = NO_BUTTON_CHANGE;
  return(myButton);
} // end patternFromButtons()

// nextPatternFromButtons() - store nextPattern if button pressed
//     nextPattern will get used when we get back to the main loop
int16_t nextPatternFromButtons() {
  int16_t myButton = getButtonPress();
  if (myButton != NO_BUTTON_PRESS) {
    nextPattern = myButton;
  }
  return (nextPattern);
} // end nextPatternFromButtons()


// setMySync(val) - sets sync to TRUE if val is nonzero, else sets sync to FALSE
void setMySync(uint8_t yes) {
  if (0 == yes) {
    digitalWrite(IAMSYNC, LOW); // sync = FALSE
  } else {
    digitalWrite(IAMSYNC, HIGH); // sync = TRUE
  }
} // end setMySync()

// val = areWeAllSync() - returns nonzero if we are all sync
int8_t areWeAllSync() {
  int8_t val = digitalRead(ALLSYNC);
  if (LOW == val) return(0);
  else            return(1);
} // end areWeAllSync()

// val = iamSyncAreWeAllSync - sets our sync output TRUE and returns nonzero if we are all sync
int8_t iamSyncAreWeAllSync() {
  iamSync(); // set us as synchronized
  return(areWeAllSync()); // return 1 if we are now all synchronized
} // end iamSyncAreWeAllSync()

// debug2_return(line_from, line_to) - used for debugging
void debug2_return(int16_t rtn_from, int16_t rtn_to) {
  DEBUG2_PRINT(F("returned from doPatternDraw:"))
  DEBUG2_PRINT(rtn_from)
  DEBUG2_PRINT(F(" to line:"))
  DEBUG2_PRINTLN(rtn_to)
}



