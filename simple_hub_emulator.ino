#include <SoftwareSerial.h>

#include <Adafruit_BMP085_U.h>


#define RESBUF_SIZE 32
#define LED_PIN 5

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
int ledState = 0;
char resBuffer[RESBUF_SIZE];
//#define debugOutput Serial1
#define cmdOutput Serial
SoftwareSerial debugOutput(7, 8);

void fault() {
  while(true) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
}

char* term_resbuf() {
  unsigned int i;
  for(i=0; i<(RESBUF_SIZE-3); ++i) {
    if(resBuffer[i] == 0) {
      resBuffer[i] = '\r';
      resBuffer[i+1] = '\n';
      resBuffer[i+2] = 0;
      break;
    }
  }
  return &resBuffer[i+2];
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  if(!bmp.begin()) fault();
  cmdOutput.setTimeout(2);
  cmdOutput.begin(115200);
  debugOutput.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //declarations
  String cmd;
  sensors_event_t event;
  unsigned int i;
  float bmpTemp = 0.;
  //entrypoint
  while(!cmdOutput.available()) delay(10);
  cmd = cmdOutput.readStringUntil('\n');
  cmd.trim();
  digitalWrite(LED_BUILTIN, HIGH);
  debugOutput.print("^");
  debugOutput.print(cmd);
  debugOutput.println("$");
  if (cmd.startsWith("ISTI")) {
    cmdOutput.write("TISTEM\r\n");
  }
  else if(cmd.startsWith("SET LED ") && cmd.length() > 8) {
    if(cmd.substring(8, 9).equals("1")) {
      ledState = 1;
      digitalWrite(LED_PIN, HIGH);
      cmdOutput.write("1\r\n");
    }
    else if(cmd.substring(8, 9).equals("0")) {
      ledState = 0;
      digitalWrite(LED_PIN, LOW);
      cmdOutput.write("0\r\n");
    }
  }
  else if(cmd.startsWith("GET ") && cmd.length() >= 7) {
    // clear the result buffer in case we need it
    for(i=0; i<RESBUF_SIZE; ++i) resBuffer[i] = 0;
    if(cmd.substring(4, 8).equals("LED")) {
      if(ledState) cmdOutput.write("1\r\n");
      else cmdOutput.write("0\r\n");
    }
    else if(cmd.substring(4, 8).equals("BAR")) {
      bmp.getEvent(&event);
      if(!event.pressure) fault();
      dtostrf(event.pressure, 3, 1, resBuffer);
      term_resbuf();
      cmdOutput.write(resBuffer);
    }
    else if(cmd.substring(4, 8).equals("THM")) {
      bmp.getTemperature(&bmpTemp);
      dtostrf(bmpTemp, 3, 2, resBuffer);
      term_resbuf();
      cmdOutput.write(resBuffer);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}
