#include "globals.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(-1);

int mainPIN[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Main PIN
int pinAddr = 0; //Individual Location
int pinData = 0; //Individual Data

char currentSite[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Site Name
char currentUser[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Username
char currentPass[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Password
int16_t siteIndex;  

byte data[ 64 ]; //CipherText
byte key[ 16 ]; //User Input Key
byte iv[ 16 ]; //Initialization Vector
uint64_t entropy;

int programPosition; //State variable

void setup()   
{          
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  programPosition = SPLASHSCREEN;
  renderScreen(programPosition);
  
  wdt_disable();

  Serial.begin( 9600 );                
  analogReference( INTERNAL );                  
  pinMode( BUTTON_CENTER_PIN, INPUT_PULLUP );  
  pinMode( BUTTON_LEFT_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_RIGHT_PIN,  INPUT_PULLUP );
  pinMode( BUTTON_DOWN_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_UP_PIN,     INPUT_PULLUP );

  Wire.begin();
  EEPROM.update( 1023, 42 );
  attachInterrupt( 2, wakeInterrupt, FALLING );
  
  programPosition = PIN_SCREEN; 
  delay(3000);
}

void loop() {
  renderScreen(programPosition);
}

void drawLock(int a){
  display.clearDisplay();
  switch(a){
    case 0:
      display.drawBitmap(0, 0, lockClose, 128, 64, WHITE);
      break;
    case 1:
      display.drawBitmap(0, 0, lockOpen, 128, 64, WHITE);
      break;
    default:
      display.drawBitmap(0, 0, lockClose, 128, 64, WHITE);
      break;
  }
  display.display();
  delay(500);
}

void drawUnlockerScreen(){
  display.clearDisplay();
  makeCrease(pinData);
  display.display(); 
  
  handleIO();  
}

void makeCrease(int num){
  switch(pinAddr){
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
    case 4:
    case 3:
      if (pinAddr>=3) display.clearDisplay();
      display.setCursor(10,23);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.print(mainPIN[pinAddr-3]);
    case 2:
      if (pinAddr==2) display.clearDisplay();
      display.setCursor(43,23);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.print(mainPIN[pinAddr-2]);
    case 1:
      if (pinAddr==1) display.clearDisplay();
      display.setCursor(76,23);
      display.setTextSize(3);
      display.setTextColor(WHITE);
      display.print(mainPIN[pinAddr-1]);
    case 0:
      display.fillCircle(127, 32, 28, WHITE);
      display.setCursor(110,23);
      display.setTextSize(3);
      display.setTextColor(BLACK);
      display.println(num);
      display.display(); 
      break;
    default:
      programPosition++;
      renderScreen(programPosition);
      break;
  }
}

void wakeInterrupt() {}


void renderScreen(uint8_t state) {
  switch(state){
    case SPLASHSCREEN:
      drawLock(0);
      break;
    case PIN_SCREEN:
      drawUnlockerScreen();
      break;
    case PRE_INDEX:
      drawLock(1);
      break;
    default :
      break;
  }
}

void handleIO(){
  while(true) {
    int mouseLeft =   pulseIn(BUTTON_LEFT_PIN, LOW,  40000);
    int mouseRight =  pulseIn(BUTTON_RIGHT_PIN, LOW,  40000);
    int mouseUp =  pulseIn(BUTTON_UP_PIN, LOW, 40000);
    int mouseDown =   pulseIn(BUTTON_DOWN_PIN, LOW, 40000);
        
    if (mouseLeft > 0){
      leftButtonClicked();
    }

    if (mouseRight > 0){
      rightButtonClicked();
    }

    if (mouseUp > 0){
      upButtonClicked();
    }

    if (mouseDown > 0){
      downButtonClicked();
    }

    if (digitalRead(BUTTON_CENTER_PIN)==0){
      delay(DEBOUNCEDELAY);
      if (digitalRead(BUTTON_CENTER_PIN)==0){
        delay(500);
        if (digitalRead(BUTTON_CENTER_PIN)==0){
          centerButtonPush();
        } else {
          centerButtonClicked();
        }
      }
    }
  }
}

void leftButtonClicked(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      pinData++;
      pinData = pinData % 10;
      makeCrease(pinData);
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}

void rightButtonClicked(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      pinData--;
      if (pinData < 0){
        pinData = 9;
      }
      makeCrease(pinData);
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}


void upButtonClicked(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}

void downButtonClicked(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}

void centerButtonClicked(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      mainPIN[pinAddr] = pinData;
      pinAddr++;
      pinData = 0;
      makeCrease(pinData);
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}

void centerButtonPush(){
  switch(programPosition){
    case SPLASHSCREEN:
      break;
    case PIN_SCREEN:
      if(pinAddr>3){
        programPosition++;
        renderScreen(programPosition);
      }
      break;
    case PRE_INDEX:
      break;
    default:
      break;
  }
}


//SECURITY PARTS

void sanitize( byte *s ) {
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

void unlock() {      //decrypt currently selected account and store in SRAM
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

void writeEntry( byte *entry ) {
  int index = siteIndex * 64;
  if ( present() ) { //so far this has eluded my attempts to change to page write...
    for ( int i = 0; i < 64; i++ ) {
      Wire.beginTransmission( 0x50 );
      Wire.write((( index + i ) >> 8 ) & 0xFF );
      Wire.write(( index + i ) & 0xFF );
      Wire.write( entry[ i ] );
      Wire.endTransmission( true );
      delay( 5 );
    }
  }
  else {
    display.clearDisplay();
    display.print( "Can't connect to memory chip" );
    display.display();
    delay( 2000 );
  }
}

void readEntry( byte *entry ) {
  int index = siteIndex * 64;
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
    display.clearDisplay();
    display.print( "Can't connect to memory chip" );
    display.display();
    delay( 2000 );
  }
}
