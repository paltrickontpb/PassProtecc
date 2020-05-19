import controlP5.*;
import processing.serial.*;

Serial port;

ControlP5 cp5;
PFont fontHead;
PFont fontSubHead;
PFont fontRest;
Textarea consoleText;
PImage img;
PrintWriter output;

String consoleOut = "> Waiting for Device....\n";

byte[] backupData = {};

void setup() {
  size(1024, 620); 
  String[] comPorts = Serial.list();
  printArray(comPorts);
  if (comPorts.length != 0) {
    port = new Serial(this, Serial.list()[0], 115200);
  } 



  cp5 = new ControlP5(this);

  fontHead = createFont("Times New Roman Bold", 72); 
  fontSubHead = createFont("Calibri Bold", 36); 
  fontRest = createFont("Calibri Bold", 20); 

  consoleText = cp5.addTextarea(consoleOut)
    .setText(consoleOut)
    .setPosition(100, 130)
    .setColorValue(0x00000000)
    .setFont(createFont("Georgia", 16))
    .setSize(300, 450)
    .setLineHeight(24)
  ;

  cp5.addButton("Connect") 
    .setPosition(500, 130)  
    .setSize(200, 70)     
    .setFont(fontRest)
    ;
    
  cp5.addButton("Disconnect") 
    .setPosition(750, 130)  
    .setSize(200, 70)     
    .setFont(fontRest)
    ;

  cp5.addButton("Backup")
    .setPosition(500, 250)  
    .setSize(450, 70)     
    .setFont(fontRest)
    ;

  cp5.addButton("Restore")
    .setPosition(500, 370)  
    .setSize(450, 70)     
    .setFont(fontRest)
    ;

  img = createImage(300, 450, ARGB);
  for (int i = 0; i < img.pixels.length; i++) {
    img.pixels[i] = color(255, 255, 255, 255);
  }
}

void draw() { 

  background(204, 210, 227);
  fill(0, 0, 0);    
  textFont(fontHead);
  text("Pass-Protecc Interface", 180, 75);
  image(img, 100, 130, 300, 450);
}

void Connect() {
  verifyCon();
  port.clear();
  port.write("c");
  delay(200);
  consoleOut = port.readString();
  consoleText.append("> ");
  consoleText.append(consoleOut);
  consoleText.append("\n"); 
}

void verifyCon() {
  port.clear();
  port.write("*@");
  delay(500);
  consoleOut = port.readString();
  consoleText.append("> ");
  consoleText.append(consoleOut);
  consoleText.append("\n");
}

void Backup(){
  consoleText.append("> ");
  consoleText.append("Backing Up Device. Please Wait..");
  consoleText.append("\n");
  port.clear();
  port.write("b");
  delay(2000);
  backupData = port.readBytes();
  saveBytes("backup.dat", backupData);
  consoleText.append("> ");
  consoleText.append("Backup Complete!");
  consoleText.append("\n");
}

void Restore(){
  int ctr = 0;
  byte restoreData[] = loadBytes("backup.dat"); 
  consoleText.append("> ");
  consoleText.append("Restoring " + str(restoreData.length) + " Bytes of data to Pass-Protecc Device");
  consoleText.append("\n");
  port.clear();
  port.write('u');
  for (int i =0; i<(7*64) ; i++){ //i<restoreData.length
    port.write(restoreData[i]);
    ctr++;
    if(ctr == 64){
      delay(500);
      ctr = 0;
    }
    delay(20);
  }
  consoleText.append("> ");
  consoleText.append("Restoration completed");
  consoleText.append("\n");
} 

void Disconnect(){
  port.clear();
  port.write('r'); 
  delay(500);
  consoleText.clear();
  consoleText.append("> ");
  consoleText.append("Device Reset Complete. Please reconnect to continue.");
  consoleText.append("\n");
  consoleText.append("> ");
  consoleText.append("Waiting for Device....\n");
}
