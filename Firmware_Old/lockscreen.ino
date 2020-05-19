#include "globals.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);
const byte address[6] = "00001";


Adafruit_SSD1306 display(-1);

int mainPIN[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //Main PIN
int pinAddr = 0; //Individual Location
int pinData = 0; //Individual Data

char currentSite[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Site Name
char currentUser[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Username
char currentPass[ 16 ] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' }; //Password
int16_t siteIndex = 0;
/*
char cSite[ 16 ] = { 'P', 'r', 'o', 'j', 'j', 'a', 'l', 's', ' ', 'G', 'i', 't', ' ', ' ', ' ', ' ' }; //Site Name
char cUser[ 16 ] = { 'p', 'a', 'l', 't', 'r', 'i', 'c', 'k', 'o', 'n', 't', 'p', 'b', ' ', ' ', ' ' }; //Username
char cPass[ 16 ] = { 'P', 'g', '@', '1', '0', '0', '9', '1', '9', '9', '8', ' ', ' ', ' ', ' ', ' ' }; //Password
*/
byte data[ 64 ]; //CipherText
byte key[ 16 ]; //User Input Key
byte iv[ 16 ]; //Initialization Vector
uint64_t entropy;

int8_t programPosition; //State variable

void setup()   
{   
      
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  programPosition = PIN_SCREEN;
  renderScreen(programPosition);
  
  wdt_disable();

  Serial.begin( 9600 );   

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
         
  analogReference( INTERNAL );                  
  pinMode( BUTTON_CENTER_PIN, INPUT_PULLUP );  
  pinMode( BUTTON_LEFT_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_RIGHT_PIN,  INPUT_PULLUP );
  pinMode( BUTTON_DOWN_PIN,   INPUT_PULLUP );
  pinMode( BUTTON_UP_PIN,     INPUT_PULLUP );

  Wire.begin();
  //EEPROM.update( 1023, 42 );
  delay(50);
  renderScreen(programPosition);
}

void loop() {
  handleIO();  
  /* Slower And Doesn't Respond for now
  if ( handleButtonChecker() ) {
    renderScreen(programPosition);
  } */
}

void drawUnlockerScreen(){
  display.clearDisplay();
  makeCrease(pinData);
  display.display(); 
  
  //handleIO();  
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

void renderScreen(uint8_t state) {
  switch(state){
    case PIN_SCREEN:
      drawUnlockerScreen();
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
    default :
      break;
  }
}

void loaderScreen(){
  delay(100);
  siteIndex = 0;
  display.clearDisplay();
  display.setCursor(32,28);
  display.setTextSize(1.5);
  display.setTextColor(WHITE);
  display.print("Unlocking");
  display.display();
  for(int j=0; j<3; j++){
    delay(200);
    display.print(".");
    display.display();
  }
  unlock();
  delay(500);
  programPosition++;
  display.setTextColor(WHITE);
  display.setTextSize(1);
  renderScreen(programPosition);
}

void showDataScreen(){
  display.clearDisplay();
  
  //show page number
  display.setCursor(37,56);
  display.print("Page ");
  display.print(siteIndex+1);
  display.print("/20");
  
  //show page data
  display.setCursor(0,0);
  display.print("Name : ");
  for ( int i = 0; i < 14; i++ ){
    display.print(currentSite[i]);
  }
  //Data Input Name
  display.setCursor(0,8);
  display.print("User : ");
  for ( int i = 0; i < 14; i++ ){
    display.print(currentUser[i]);
  }
  //Data Input User
  display.setCursor(0,16);
  display.print("Pass : ");
  for ( int i = 0; i < 14; i++ ){
    display.print(currentPass[i]);
  }
  //Data Input Pass

  display.setCursor(0,32);
  display.print("Edit");
  display.setCursor(0,40);
  display.print("(UP)");
  display.setCursor(95,32);
  display.print("Erase");
  display.setCursor(92,40);
  display.setTextSize(1);
  display.print("(DOWN)");
  
  display.display();  
  //handleIO();
  
}

void showEraseScreen(){
  display.clearDisplay();
  display.setCursor(26,8);
  display.print("Are You Sure ?");
  display.setCursor(35,24);
  display.print("YES : Press");
  display.setCursor(29,34);
  display.print("NO : Swipe up");
  display.display();
  //handleIO();
}

void showEditScreenMenu(){
  display.clearDisplay();
  display.display();
/*
  for(int i=0; i<16; i++) currentSite[i] = cSite[i];
  for(int i=0; i<16; i++) currentUser[i] = cUser[i];
  for(int i=0; i<16; i++) currentPass[i] = cPass[i];
  lock();
*/
  display.setCursor(20,28);
  display.print("Work in Progress");
  display.display();
  delay(1500);
  programPosition = MAIN_SITE;
  renderScreen(programPosition);
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

void handleIO(){
    int mouseLeft =   pulseIn(BUTTON_LEFT_PIN, LOW,  30000);
    int mouseRight =  pulseIn(BUTTON_RIGHT_PIN, LOW,  30000);
    int mouseUp =  pulseIn(BUTTON_UP_PIN, LOW, 30000);
    int mouseDown =   pulseIn(BUTTON_DOWN_PIN, LOW, 30000);
        
    if (mouseLeft > 0){
      mouseLeft = NULL;
      leftButtonClicked();
    }

    if (mouseRight > 0){
      mouseRight = NULL;
      rightButtonClicked();
    }

    if (mouseUp > 0){
      mouseUp = NULL;
      upButtonClicked();
    }

    if (mouseDown > 0){
      mouseDown = NULL;
      downButtonClicked();
    }

    mouseLeft = NULL;
    mouseRight = NULL;
    mouseUp = NULL;
    mouseDown = NULL;

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

void leftButtonClicked(){
  switch(programPosition){
    case PIN_SCREEN:
      pinData++;
      pinData = pinData % 10;
      makeCrease(pinData);
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
    default:
      break;
  }
}

void rightButtonClicked(){
  switch(programPosition){
    case PIN_SCREEN:
      pinData--;
      if (pinData < 0){
        pinData = 9;
      }
      makeCrease(pinData);
      break;
    case PRE_INDEX:
      break;
    case MAIN_SITE:
      programPosition = ERASE_SCREEN;
      renderScreen(programPosition);
      break;
    case ERASE_SCREEN:
      break;
    default:
      break;
  }
}


void upButtonClicked(){
  switch(programPosition){
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
    default:
      break;
  }
}

void downButtonClicked(){
  switch(programPosition){
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
    default:
      break;
  }
}

void centerButtonClicked(){
  switch(programPosition){
    case PIN_SCREEN:
      mainPIN[pinAddr] = pinData;
      pinAddr++;
      pinData = 0;
      makeCrease(pinData);
      break;
    case PRE_INDEX:
      programPosition++;
      renderScreen(programPosition);
      break;
    case MAIN_SITE:
      //lock();
      Keyboard.begin();
      for(int i=0; i<16; i++) {
        if (currentPass[i] != ' ') Keyboard.print(currentPass[i]);
      }
      Keyboard.end();
      break;
    case ERASE_SCREEN:
      for(int i=0; i<16; i++) currentSite[i] = ' ';
      for(int i=0; i<16; i++) currentUser[i] = ' ';
      for(int i=0; i<16; i++) currentPass[i] = ' ';
      lock();
      programPosition = MAIN_SITE;
      renderScreen(programPosition);
    default:
      break;
  }
}

void centerButtonPush(){
  switch(programPosition){
    case PIN_SCREEN:
      if(pinAddr>=3){
        mainPIN[pinAddr] = pinData;
        programPosition++;
        renderScreen(programPosition);
      }
      break;
    case PRE_INDEX:
      programPosition++;
      renderScreen(programPosition);
      break;
    case MAIN_SITE:
      Keyboard.begin();
      for(int i=0; i<16; i++) {
        if (currentUser[i] != ' ') Keyboard.print(currentUser[i]);
      }
      Keyboard.press(179);
      Keyboard.release(179);
      for(int i=0; i<16; i++) {
        if (currentPass[i] != ' ') Keyboard.print(currentPass[i]);
      }
      delay(20);
      Keyboard.press(176);
      Keyboard.release(176);
      Keyboard.end();
      delay(1000);
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

void writeEntry( byte *entry ) {
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
    display.clearDisplay();
    display.print( "Can't connect to memory chip" );
    display.display();
    delay( 200 );
  }
}

void readEntry( byte *entry ) {
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
    display.clearDisplay();
    display.print( "Can't connect to memory chip" );
    display.display();
    delay( 200 );
  }
}

//Keyboard
