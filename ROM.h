class ROM {
public:

  const byte SD_SELECT = 4;

  void loadBasicBootloader() {
    byte program[] = {   //4k basic 3.2 bootstrap loader
      //LOOP
      0x21,0xAE,0x0F,   //lxi     h,0fae h    ;4K BASIC V3.2
      0x31,0x12,0x00,   //lxi     sp,STACK    ;init SP so a RET jumps to loop
      0xDB,0x00,        //in      00h         ;get sio status
      0x0F,             //rrc                 ;new byte available?
      0xD8,             //rc                  ;no (jumps back to loop)
      0xDB,0x01,        //in      01h         ;get the byte
      0xBD,             //cmp     l           ;new byte = leader byte? AE
      0xC8,             //rz                  ;yes (jumps back to loop)
      0x2D,             //dcr     l           ;not leader, decrement address
      0x77,             //mov     m,a  = mov [hl],a       ;store the byte (reverse order)
      0xC0,             //rnz                 ;loop until L = 0
      //0xE9,             //pchl                ;jump to code just downloaded   
      0x76, //HALT 
      //STACK
      0x03,0x00        //dw LOOP
    };
    
    load(program,sizeof(program),0);
  }

  void dir() {

  }

  void loadFile(char *filename, int offset = 0) {
    if (!SD.begin(SD_SELECT)) {
      Serial.println(F("ERROR: SD NOT ready"));
      SPI.end();
      return;
    }

    File file = SD.open(filename);
    if (!file) {
      Serial.println(F("ERROR: Cannot load file"));
      SPI.end();
      return;
    }

    //LOG("Loading %s... %d bytes",filename,file.size());
    Serial.println(F("Loading..."));
    uint16_t counter = offset;
    while (file.available()) {
      byte b = file.read();
      SPI.end(); //tmp disable
      RAM.write(counter++, b);
      SPI.begin(); //re-enable
    }
    LOG("done",0);

    file.close();
    SPI.end();
  }

  void loadBasicFromSD() {
    loadFile("4KBAS32.BIN",0);
  }

  void load(byte *program, int s, int offset = 0) {
    //LOG("Loading program %d bytes",s);
    Serial.println(F("Loading..."));
    for (uint16_t i=0; i<s; i++) {
      RAM.write(i+offset,program[i]);
    }
    Serial.println(F("done"));
  }
 

} ROM;


//0xAE bytes of second bootloader
//byte program[] = { 0xF3,0xC3,0x21,0x0D,0x90,0x04,0xF9,0x07,0x7E,0xE3,0xBE,0x23,0xE3,0xC2,0xD0,0x01,0x23,0x7E,0xFE,0x3A,0xD0,0xC3,0x5E,0x04,0xF5,0x3A,0x27,0x00,0xC3,0x6E,0x03,0x00,0x7C,0x92,0xC0,0x7D,0x93,0xC9,0x01,0x00,0x3A,0x72,0x01,0xB7,0xC2,0xDA,0x09,0xC9,0xE3,0x22,0x3B,0x00,0xE1,0x4E,0x23,0x46,0x23,0xC5,0xC3,0x3A,0x00,0xE4,0x09,0xA2,0x0A,0xF8,0x09,0x98,0x04,0x21,0x0C,0x5F,0x0C,0x95,0x0C,0x79,0x10,0x08,0x79,0x0A,0x08,0x7C,0xE3,0x08,0x7C,0x2F,0x09,0x45,0x4E,0xC4,0x46,0x4F,0xD2,0x4E,0x45,0x58,0xD4,0x44,0x41,0x54,0xC1,0x49,0x4E,0x50,0x55,0xD4,0x44,0x49,0xCD,0x52,0x45,0x41,0xC4,0x4C,0x45,0xD4,0x47,0x4F,0x54,0xCF,0x52,0x55,0xCE,0x49,0xC6,0x52,0x45,0x53,0x54,0x4F,0x52,0xC5,0x47,0x4F,0x53,0x55,0xC2,0x52,0x45,0x54,0x55,0x52,0xCE,0x52,0x45,0xCD,0x53,0x54,0x4F,0xD0,0x50,0x52,0x49,0x4E,0xD4,0x4C,0x49,0x53,0xD4,0x43,0x4C,0x45,0x41,0xD2,0x4E,0x45,0xD7,0x54,0x41,0x42,0xA8,0x54,0xCF,0x54 };



//byte program[] = {   //Serial echo with interrupt PART 1
//  0x31,0x00,0x01,   //lxi     sp,0100h
//  0x3E,0x01,        //mvi     a,01h
//  0xD3,0x00,        //out     00h
//  0xFB,             //ei
//  //LOOP
//  0x00,             //nop
//  0x00,
//  0x00,
//  0xC3,0x08,0x00    //jmp LOOP
//};
//
//byte hook[] = { //@38h  - Serial echo with interrupt PART 2
//  0xF5,             //push a
//  0xDB,0x01,        //in 1
//  0xD3,0x01,        //out 1
//  0xF1,             //pop a
//  0xFB,             //ei
//  0xC9              //ret
//};

//byte program[] = {   //Serial echo 
//  0xdb,0x00,          //inp 00h ; a = Serial.available()
//  0x0f,               //rrc     ; shift right
//  0xda,0x00,0x00,     //jc 0    ; if carry jump to begin
//  0xdb,0x01,          //inp 01h ; a = Serial.read();
//  0xd3,0x01,          //out 01h ; Serial.write(a)
//  0xc3,0x00,0x00      //jmp 0   ; jmp 0
//};

//byte program[] = { //show sense switches on high part of address leds
//  0xdb,0xff,   //in 0xff (sense switches)
//  0x57,        //mov d,a
//  0x1a,        //ldax d
//  0x1a,
//  0x1a,
//  0x1a,
//  0xc3,0x00,0x00 //jmp 0
//};
  
//byte program[] = { //killbits
//  0x21,0x00,0x00,
//  0x16,0x80,
//  0x01,0x00,0x44,
//  0x1a,
//  0x1a,
//  0x1a,
//  0x1a,
//  0x09,
//  0xd2,0x08,0x00,
//  0xdb,0xff,
//  0xaa,
//  0x0f,
//  0x57,
//  0xc3,0x08,0x00 };