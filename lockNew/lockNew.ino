#include "globals.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

SSD1306AsciiWire oled;

int mainPIN[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //Main PIN
int pinAddr = 0; //Individual Location
int pinData = 0; //Individual Data
int encType = 0;

char currentSite[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Site Name
char currentUser[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Username
char currentPass[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Password
int16_t siteIndex = 0;

const char keyboard1[ 32 ] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '(', ')', '[', ']', '{', '}' };
const char keyboard2[ 32 ] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<', '>', '|', '\\', '/', '?' };
const char keyboard3[ 32 ] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '!', '@', '#', '$', '%', '^', '&', '*', '_', '+', '`', '~', ';', ':', '"', '\'', ',', '.', ' ', ' ' };

const char randChars[ 92 ] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '(', ')', '[', ']', '{', '}', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '<', '>', '|', '/', '?', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '+', '!', '@', '#', '$', '%', '^', '&', '*', '_', '+', '`', '~', ';', ':', '"', ',', '.' };

byte data[ 64 ]; //CipherText
byte key[ 16 ]; //User Input Key
byte iv[ 16 ]; //Initialization Vector
uint64_t entropy;

int8_t programPosition; //State variable
int editVar = 0;
int keyType = 0;
int letIndex = 0;
int nameIndex = 0;

void setup()
{
  Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Callibri15);

  programPosition = PIN_SCREEN;
  renderScreen(programPosition);

  wdt_disable();

  Serial.begin( 9600 );

  analogReference( INTERNAL );
  pinMode( BUTTON_CENTER_PIN, INPUT_PULLUP );
  pinMode( BUTTON_LEFT_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_RIGHT_PIN,  INPUT_PULLUP );
  pinMode( BUTTON_DOWN_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_UP_PIN,     INPUT_PULLUP );

  randomSeed(analogRead(0));
  //EEPROM.update( 1023, 42 );
  delay(50);
  renderScreen(programPosition);
}

void loop() {
  handleIO();
  /*Slower And Doesn't Respond for now
    if ( handleButtonChecker() ) {
    renderScreen(programPosition);
    }*/
}

void drawUnlockerScreen() {
  oled.clear();
  makeCrease(pinData);


  //handleIO();
}

void makeCrease(int num) {
  switch (pinAddr) {
    case 15:
    case 14:
    case 13:
    case 12:
    case 11:
    case 10:
    case 9:
    case 8:
    case 7:
    case 6:
    case 5:
      if (pinAddr >= 5) oled.clear();
      oledSetCursor(10, 2);
      oled.set2X();
      oled.print(mainPIN[pinAddr - 5]);
    case 4:
      if (pinAddr == 4) oled.clear();
      oledSetCursor(30, 2);
      oled.set2X();
      oled.print(mainPIN[pinAddr - 4]);
    case 3:
      if (pinAddr == 3) oled.clear();
      oledSetCursor(50, 2);
      oled.set2X();
      oled.print(mainPIN[pinAddr - 3]);
    case 2:
      if (pinAddr == 2) oled.clear();
      oledSetCursor(70, 2);
      oled.set2X();
      oled.print(mainPIN[pinAddr - 2]);
    case 1:
      if (pinAddr == 1) oled.clear();
      oledSetCursor(90, 2);
      oled.set2X();
      oled.print(mainPIN[pinAddr - 1]);
    case 0:
      oled.setInvertMode(1);
      for (int i = 0; i <= 6; i = i + 2) {
        oledSetCursor(105, i);
        oled.print("       ");
      }
      oledSetCursor(110, 2);
      oled.set2X();
      oled.print(num);
      oled.setInvertMode(0);
      break;
    default:
      programPosition++;
      renderScreen(programPosition);
      break;
  }
}

void renderScreen(uint8_t state) {
  switch (state) {
    case PIN_SCREEN:
      drawUnlockerScreen();
      break;
    case ENC_SELECTION:
      drawEncType();
      break;
    case PRE_INDEX:
      loaderScreen();
      break;
    case MAIN_SITE:
      showDataScreen();
      break;
    case ERASE_SCREEN:
      showEraseScreen();
      break;
    case EDIT_SCREEN_MENU:
      showEditScreenMenu();
      break;
    case EDIT_KEYBOARD:
      showKeyboardScreen();
      break;
    default :
      break;
  }
}

void loaderScreen() {
  oled.setFont(System5x7);
  siteIndex = 0;
  oled.clear();
  oledSetCursor(32, 4);
  oled.set1X();

  oled.print("Unlocking");

  for (int j = 0; j < 3; j++) {
    delay(200);
    oled.print(".");

  }
  unlock();
  delay(500);
  programPosition++;

  oled.set1X();
  renderScreen(programPosition);
}

void drawEncType() {
  oled.clear();
  delay(100);
  oledSetCursor(22, 0);
  oled.set1X();
  oled.print("Select Encryption");
  //oled.set2X();
  oledSetCursor(40, 3);
  oled.print("AES-CBC  "); 
  oledSetCursor(40, 5);
  oled.print("ChaCha20  ");
  
  switch (encType) {
    case 0:
      oledSetCursor(32, 3);
      invertPrint("> AES-CBC");
      break;
    case 1:
      oledSetCursor(32, 5);
      invertPrint("> ChaCha20");
      break;
    default: break;
  }
  oled.set1X();
}

void showDataScreen() {
  oled.clear();
  oled.set1X();
  //show page number
  oledSetCursor(37, 7);
  oled.print("Page ");
  oled.print(siteIndex + 1);
  oled.print("/20");

  //show page data
  oledSetCursor(0, 0);
  oled.print("Name : ");
  for ( int i = 0; i < 16; i++ ) {
    oled.print(currentSite[i]);
  }
  //Data Input Name
  oledSetCursor(0, 1);
  oled.print("User : ");
  for ( int i = 0; i < 16; i++ ) {
    oled.print(currentUser[i]);
  }
  //Data Input User
  oledSetCursor(0, 2);
  oled.print("Pass : ");
  for ( int i = 0; i < 16; i++ ) {
    oled.print(currentPass[i]);
  }
  //Data Input Pass

  oledSetCursor(0, 4);
  oled.print("Edit");
  oledSetCursor(0, 5);
  oled.print("(UP)");
  oledSetCursor(95, 4);
  oled.print("Erase");
  oledSetCursor(92, 5);
  oled.set1X();
  oled.print("(DOWN)");


  //handleIO();

}

void showEraseScreen() {
  oled.clear();
  oledSetCursor(26, 1);
  oled.print("Are You Sure ?");
  oledSetCursor(35, 3);
  oled.print("YES : Press");
  oledSetCursor(29, 4);
  oled.print("NO : Swipe up");

  //handleIO();
}

void showEditScreenMenu() {
  oled.clear();
  oledSetCursor(10, 0);
  oled.print("Edit which field ?");
  oledSetCursor(35, 2);
  oled.println("Site Name  "); oled.setCol(35);
  oled.println("User Name  "); oled.setCol(35);
  oled.println("Password  ");

  switch (editVar) {
    case 0:
      oledSetCursor(27, 2);
      invertPrint("> Site Name");
      break;
    case 1:
      oledSetCursor(27, 3);
      invertPrint("> User Name");
      break;
    case 2:
      oledSetCursor(27, 4);
      invertPrint("> Password");
      break;
    default: break;
  }

  oledSetCursor(0, 7);
  oled.print("<= Swipe back to Menu");
}

void showKeyboardScreen() {
  oled.clear();
  oledSetCursor(0, 0);
  switch (editVar) {
    case 0:
      oled.print("Name :");
      for (int i = 0; i < 16; i++) oled.print(currentSite[i]);
      break;
    case 1:
      oled.print("User :");
      for (int i = 0; i < 16; i++) oled.print(currentUser[i]);
      break;
    case 2:
      oled.print("Pass :");
      for (int i = 0; i < 16; i++) oled.print(currentPass[i]);
      break;
    default:
      break;
  }

  setupKeyboardLayout();
}

void setupKeyboardLayout() {
  switch (keyType) {
    case 0:
      oledSetCursor(0, 2);
      for (int i = 0; i < 11 ; i++) {
        if (i == letIndex) {
          invertPrint(keyboard1[i]);
        } else {
          oled.print(keyboard1[i]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 3);
      for (int i = 0; i < 11 ; i++) {
        if (i + 11 == letIndex) {
          invertPrint(keyboard1[i + 11]);
        } else {
          oled.print(keyboard1[i + 11]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 4);
      for (int i = 0; i < 10 ; i++) {
        if (i + 22 == letIndex) {
          invertPrint(keyboard1[i + 22]);
        } else {
          oled.print(keyboard1[i + 22]);
        }
        oled.print(' ');
      }
      break;
    case 1:
      oledSetCursor(0, 2);
      for (int i = 0; i < 11 ; i++) {
        if (i == letIndex) {
          invertPrint(keyboard2[i]);
        } else {
          oled.print(keyboard2[i]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 3);
      for (int i = 0; i < 11 ; i++) {
        if (i + 11 == letIndex) {
          invertPrint(keyboard2[i + 11]);
        } else {
          oled.print(keyboard2[i + 11]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 4);
      for (int i = 0; i < 10 ; i++) {
        if (i + 22 == letIndex) {
          invertPrint(keyboard2[i + 22]);
        } else {
          oled.print(keyboard2[i + 22]);
        }
        oled.print(' ');
      }
      break;
    case 2:
      oledSetCursor(0, 2);
      for (int i = 0; i < 11 ; i++) {
        if (i == letIndex) {
          invertPrint(keyboard3[i]);
        } else {
          oled.print(keyboard3[i]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 3);
      for (int i = 0; i < 11 ; i++) {
        if (i + 11 == letIndex) {
          invertPrint(keyboard3[i + 11]);
        } else {
          oled.print(keyboard3[i + 11]);
        }
        oled.print(' ');
      }
      oledSetCursor(0, 4);
      for (int i = 0; i < 10 ; i++) {
        if (i + 22 == letIndex) {
          invertPrint(keyboard3[i + 22]);
        } else {
          oled.print(keyboard3[i + 22]);
        }
        oled.print(' ');
      }
      break;
    default:
      break;
  }

  oledSetCursor(0, 5);
  if (letIndex == 32) invertPrint("> Switch");
  else oled.print("Switch  ");

  oledSetCursor(90, 5);
  if (letIndex == 36) invertPrint("> Del");
  else oled.print("  Del");

  oledSetCursor(0, 6);
  if (letIndex == 33) invertPrint("> Randomize");
  else oled.print("Randomize    ");

  oledSetCursor(0, 7);
  if (letIndex == 34) invertPrint("> Save");
  else oled.print("Save   ");

  oledSetCursor(70, 7);
  if (letIndex == 35) invertPrint("> Cancel");
  else oled.print("  Cancel");
}

void oledSetCursor(int x, int y) {
  oled.setCol(x);
  oled.setRow(y);
}


//IO Modules
/*
  int handleButtonChecker() {

  if ( !digitalRead( BUTTON_CENTER_PIN )) {
   delay( DEBOUNCEDELAY );
    if ( !digitalRead( BUTTON_CENTER_PIN )) {
      delay(500);
      if( !digitalRead( BUTTON_CENTER_PIN )) {
        centerButtonPush();
      } else {
        centerButtonClicked();
      }
      return 1;
    }
  }
  if ( !digitalRead( BUTTON_RIGHT_PIN )) {
    delay( DEBOUNCEDELAY );
    if ( !digitalRead( BUTTON_RIGHT_PIN )) {
      rightButtonClicked();
      return 1;
    }
  }
  if ( !digitalRead( BUTTON_LEFT_PIN )) {
    delay( DEBOUNCEDELAY );
    if ( !digitalRead( BUTTON_LEFT_PIN )) {
      leftButtonClicked();
      return 1;
    }
  }
  if ( !digitalRead( BUTTON_UP_PIN )) {
    delay( DEBOUNCEDELAY );
    if ( !digitalRead( BUTTON_UP_PIN )) {
      upButtonClicked();
      return 1;
    }
  }
  if ( !digitalRead( BUTTON_DOWN_PIN )) {
    delay( DEBOUNCEDELAY );
    if ( !digitalRead( BUTTON_DOWN_PIN )) {
      downButtonClicked();
      return 1;
    }
  }
  return 0;
  } */

void handleIO() {
  int mouseLeft =   pulseIn(BUTTON_LEFT_PIN, LOW,  30000);
  int mouseRight =  pulseIn(BUTTON_RIGHT_PIN, LOW,  30000);
  int mouseUp =  pulseIn(BUTTON_UP_PIN, LOW, 30000);
  int mouseDown =   pulseIn(BUTTON_DOWN_PIN, LOW, 30000);

  if (mouseLeft > 0) {
    mouseLeft = NULL;
    leftButtonClicked();
  }

  if (mouseRight > 0) {
    mouseRight = NULL;
    rightButtonClicked();
  }

  if (mouseUp > 0) {
    mouseUp = NULL;
    upButtonClicked();
  }

  if (mouseDown > 0) {
    mouseDown = NULL;
    downButtonClicked();
  }

  mouseLeft = NULL;
  mouseRight = NULL;
  mouseUp = NULL;
  mouseDown = NULL;

  if (digitalRead(BUTTON_CENTER_PIN) == 0) {
    delay(DEBOUNCEDELAY);
    if (digitalRead(BUTTON_CENTER_PIN) == 0) {
      delay(500);
      if (digitalRead(BUTTON_CENTER_PIN) == 0) {
        centerButtonPush();
      } else {
        centerButtonClicked();
      }
    }
  }
}

void leftButtonClicked() {
  switch (programPosition) {
    case PIN_SCREEN:
      pinData++;
      pinData = pinData % 10;
      makeCrease(pinData);
      break;
    case ENC_SELECTION:
      encType--;
      if (encType < 0) {
        encType = 1;
      }
      renderScreen(programPosition);
      break;
    case PRE_INDEX:
      break;
    case MAIN_SITE:
      programPosition = EDIT_SCREEN_MENU;
      renderScreen(programPosition);
      break;
    case ERASE_SCREEN:
      programPosition = MAIN_SITE;
      renderScreen(programPosition);
      break;
    case EDIT_SCREEN_MENU:
      editVar--;
      if (editVar < 0) {
        editVar = 2;
      }
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      if (letIndex == 36) letIndex = 31;
      else if (letIndex > 31) letIndex -= 1;
      else letIndex -= 11;
      if (letIndex < 0) letIndex = 34;
      setupKeyboardLayout();
      break;
    default:
      break;
  }
}

void rightButtonClicked() {
  switch (programPosition) {
    case PIN_SCREEN:
      pinData--;
      if (pinData < 0) {
        pinData = 9;
      }
      makeCrease(pinData);
      break;
    case ENC_SELECTION:
      encType++;
      encType %= 2;
      renderScreen(programPosition);
      break;
    case PRE_INDEX:
      break;
    case MAIN_SITE:
      programPosition = ERASE_SCREEN;
      renderScreen(programPosition);
      break;
    case ERASE_SCREEN:
      break;
    case EDIT_SCREEN_MENU:
      editVar++;
      editVar %= 3;
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      if (letIndex == 36) letIndex = 33;
      else if (letIndex > 31) letIndex += 1;
      else {
        letIndex += 11;
        if (letIndex > 31) letIndex = 32;
      }
      if (letIndex > 34 and letIndex < 36) letIndex = 0;
      setupKeyboardLayout();
      break;
    default:
      break;
  }
}


void upButtonClicked() {
  switch (programPosition) {
    case PIN_SCREEN:
      break;
    case PRE_INDEX:
      break;
    case MAIN_SITE:
      siteIndex = siteIndex + 1;
      siteIndex = siteIndex % MAXSITES;
      unlock();
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      switch (letIndex) {
        case 32:
          letIndex = 36;
          break;
        case 36:
          letIndex = 32;
          break;
        case 34:
          letIndex = 35;
          break;
        case 35:
          letIndex = 34;
          break;
        default:
          letIndex += 1;
          letIndex = letIndex % 32;
          break;
      }
      setupKeyboardLayout();
      break;
    default:
      break;
  }
}

void downButtonClicked() {
  switch (programPosition) {
    case PIN_SCREEN:
      break;
    case PRE_INDEX:
      break;
    case MAIN_SITE:
      siteIndex = siteIndex - 1;
      if ( siteIndex < 0 ) siteIndex = MAXSITES - 1;
      unlock();
      renderScreen(programPosition);
      break;
    case EDIT_SCREEN_MENU:
      programPosition = MAIN_SITE;
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      switch (letIndex) {
        case 32:
          letIndex = 36;
          break;
        case 36:
          letIndex = 32;
          break;
        case 34:
          letIndex = 35;
          break;
        case 35:
          letIndex = 34;
          break;
        default:
          letIndex -= 1;
          if (letIndex < 0) {
            letIndex = 31;
          }
          break;
      }
      setupKeyboardLayout();
      break;
    default:
      break;
  }
}

void centerButtonClicked() {
  switch (programPosition) {
    case PIN_SCREEN:
      mainPIN[pinAddr] = pinData;
      pinAddr++;
      pinData = 0;
      makeCrease(pinData);
      break;
    case ENC_SELECTION:
      if (encType == 1) {
        for (int i = 0; i < 16; i++) {
          mainPIN[i] = + i;
          if (mainPIN[i] > 20) mainPIN[i] -= 20;
          if (mainPIN[i] > 10) mainPIN[i] -= 10;
        }
      }
      programPosition = PRE_INDEX;
      renderScreen(programPosition);
      break;
    case PRE_INDEX:
      programPosition++;
      renderScreen(programPosition);
      break;
    case MAIN_SITE:
      //lock();
      /*
      Keyboard.begin();
      for (int i = 0; i < 16; i++) {
        if (currentPass[i] != ' ') Keyboard.print(currentPass[i]);
      }
      Keyboard.end(); */
      break;
    case ERASE_SCREEN:
      for (int i = 0; i < 16; i++) currentSite[i] = ' ';
      for (int i = 0; i < 16; i++) currentUser[i] = ' ';
      for (int i = 0; i < 16; i++) currentPass[i] = ' ';
      lock();
      programPosition = MAIN_SITE;
      renderScreen(programPosition);
      break;
    case EDIT_SCREEN_MENU:
      nameIndex = 0;
      letIndex = 0;
      keyType = 0;
      programPosition = EDIT_KEYBOARD;
      switch (editVar) {
        case 0:
          for (int i = 0; i < 16; i++) currentSite[i] = ' ';
          break;
        case 1:
          for (int i = 0; i < 16; i++) currentUser[i] = ' ';
          break;
        case 2:
          for (int i = 0; i < 16; i++) currentPass[i] = ' ';
          break;
        default:
          break;
      }
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      switch (letIndex) {
        case 32:
          keyType++;
          keyType %= 3;
          setupKeyboardLayout();
          break;
        case 33:
          //Rand Code
          switch (editVar) {
            case 0:
              for (int i = 0; i < 16; i++) {
                int randNumber = random(92);
                currentSite[i] = randChars[randNumber];
              }
              break;
            case 1:
              for (int i = 0; i < 16; i++) {
                int randNumber = random(92);
                currentUser[i] = randChars[randNumber];
              }
              break;
            case 2:
              for (int i = 0; i < 16; i++) {
                int randNumber = random(92);
                currentPass[i] = randChars[randNumber];
              }
              break;
            default : break;
          }
          break;
        case 34:
          lock();
          programPosition = MAIN_SITE;
          renderScreen(programPosition);
          break;
        case 35:
          programPosition = MAIN_SITE;
          unlock();
          renderScreen(programPosition);
          break;
        case 36:
          if (nameIndex > 0) nameIndex -= 1;
          switch (editVar) {
            case 0: currentSite[nameIndex] = ' '; break;
            case 1: currentUser[nameIndex] = ' '; break;
            case 2: currentPass[nameIndex] = ' '; break;
            default : break;
          }
          break;
        default:
          switch (keyType) {
            case 0:
              switch (editVar) {
                case 0: currentSite[nameIndex] = keyboard1[letIndex]; break;
                case 1: currentUser[nameIndex] = keyboard1[letIndex]; break;
                case 2: currentPass[nameIndex] = keyboard1[letIndex]; break;
                default : break;
              }
              break;
            case 1:
              switch (editVar) {
                case 0: currentSite[nameIndex] = keyboard2[letIndex]; break;
                case 1: currentUser[nameIndex] = keyboard2[letIndex]; break;
                case 2: currentPass[nameIndex] = keyboard2[letIndex]; break;
                default : break;
              }
              break;
            case 2:
              switch (editVar) {
                case 0: currentSite[nameIndex] = keyboard3[letIndex]; break;
                case 1: currentUser[nameIndex] = keyboard3[letIndex]; break;
                case 2: currentPass[nameIndex] = keyboard3[letIndex]; break;
                default : break;
              }
              break;
            default: break;
          }
          nameIndex++;
          break;
      }
      renderScreen(programPosition);
      break;
    default:
      break;
  }
}

void centerButtonPush() {
  switch (programPosition) {
    case PIN_SCREEN:
      if (pinAddr >= 3) {
        mainPIN[pinAddr] = pinData;
        //oled.setFont(System5x7);
        programPosition = ENC_SELECTION;
        renderScreen(programPosition);
      }
      break;
    case PRE_INDEX:
      programPosition++;
      renderScreen(programPosition);
      break;
    case MAIN_SITE:
      Keyboard.begin();
      for (int i = 0; i < 16; i++) {
        if (currentUser[i] != ' ') Keyboard.print(currentUser[i]);
      }
      Keyboard.press(179);
      Keyboard.release(179);
      for (int i = 0; i < 16; i++) {
        if (currentPass[i] != ' ') Keyboard.print(currentPass[i]);
      }
      delay(20);
      //Keyboard.press(176);
      //Keyboard.release(176);
      Keyboard.end();
      delay(1000);
      break;
    case EDIT_SCREEN_MENU:
      programPosition = MAIN_SITE;
      renderScreen(programPosition);
      break;
    case EDIT_KEYBOARD:
      /*
      */
      break;
    default:
      break;
  }
}


//SECURITY PARTS

void sanitize( byte * s ) {
  for ( int i = 0; i < 48; i++ ) {
    if (( s[ i ] < 32 ) || ( s[ i ] > 126 )) {
      int r = random( 94 );
      s[ i ] = pgm_read_byte_near( randomValues + r );
    }
  }
}

void lock() {      //encrypt currently selected account and store in EEPROM
  for ( int i = 0; i < 16; i++ ) {
    iv[ i ] = entropy >> (( 8 * i ) % 64 ); // increase entropy to 128 bits?
    key[ i ] = mainPIN[ i ];
    data[ i ] = currentSite[ i ];
    data[ i + 16 ] = currentUser[ i ];
    data[ i + 32 ] = currentPass[ i ];
  }
  aes128_cbc_enc( key, iv, data, 48 );
  for ( int i = 0; i < 16; i++ ) data[ 48 + i ] = iv[ i ];
  writeEntry( data );
}

void unlock() {
  readEntry( data );
  for ( int i = 0; i < 16; i++ ) {
    iv[ i ] = data[ i + 48 ];
    key[ i ] = mainPIN[ i ];
  }
  aes128_cbc_dec( key, iv, data, 48 );
  sanitize( data );
  for ( int i = 0; i < 16; i++ ) {
    currentSite[ i ] = data[ i ];
    currentUser[ i ] = data[ i + 16 ];
    currentPass[ i ] = data[ i + 32 ];
  }
}

//EEPROM

uint8_t present() {
  Wire.beginTransmission( 0x50 );
  if ( Wire.endTransmission() == 0 ) return 1;
  return 0;
}

void writeEntry( byte * entry ) {
  int index = siteIndex * 64;
  if ( present() ) {
    for ( int i = 0; i < 64; i++ ) {
      Wire.beginTransmission( 0x50 );
      Wire.write((( index + i ) >> 8 ) & 0xFF );
      Wire.write(( index + i ) & 0xFF );
      Wire.write( entry[ i ] );
      Wire.endTransmission( true );
      delay( 50 );
    }
  }
  else {
    oled.clear();
    oled.print( "Can't connect to memory chip" );

    delay( 200 );
  }
}

void readEntry( byte * entry ) {
  int32_t index = siteIndex * 64;
  if ( present() ) {
    Wire.beginTransmission( 0x50 );
    Wire.write(( index >> 8 ) & 0xFF );
    Wire.write( index & 0xFF );
    Wire.endTransmission( true );
    for ( int i = 0; i < 64; i++ ) {
      Wire.requestFrom( 0x50, 1 );
      entry[ i ] = Wire.read();
    }
    Wire.endTransmission( true );
  }
  else {
    oled.clear();
    oled.print( "Can't connect to memory chip" );
    delay( 200 );
  }
}

//OLED Print

void invertPrint(String a) {
  oled.setInvertMode(1);
  oled.print(a);
  oled.setInvertMode(0);
}

void invertPrint(char a) {
  oled.setInvertMode(1);
  oled.print(a);
  oled.setInvertMode(0);
}
