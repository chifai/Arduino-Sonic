//2019-07-01: v1.0.0 註解
//以下以更底層的控制方法編寫程式, 可同時啟動多個SRF04超音波感測器
//讀取A0及A1為量測電流/電壓的pin
//將4顆超音波及A0, A1的值以UART形式傳送

const int SonicNo = 4;                      //number of sonic 
const int TRI_PIN[SonicNo] = {4,8,64,128};  //Sonic trigger pins
const int ECH_PIN[SonicNo] = {4,5,8,9};     //Sonic echo pins
const int LP_DEG = 16;                      //Low Pass filter magnitude
const int MsgNo = 7;                        //Total message number
const int MOTOR_SW = 10;
String CVMsg[MsgNo];                        //Message string sent to main controller
byte sonicDist[SonicNo];

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  DDRD = B11001100;
  DDRB = B11111100; 
  pinMode(MOTOR_SW,OUTPUT);
  digitalWrite(MOTOR_SW,1);
}

void loop() {
  static unsigned int loopNo = 0;
  static int curr0 = 0, curr1 = 0, volt0 = 0, volt1 = 0;

  //Sonic 
  int dist = getSonic(loopNo);
  if (dist > 255)
    dist = 255;
  sonicDist[loopNo] = (byte) dist;
  //End
  
  loopNo += 1;
  loopNo %= SonicNo;

  //Read the voltage and current
  curr1 = analogRead(A0);
  volt1 = analogRead(A1);
  curr0 = (curr0 * (LP_DEG - 1) + curr1) / LP_DEG;
  volt0 = (volt0 * (LP_DEG - 1) + volt1) / LP_DEG;
  
  if (loopNo == 0)
  {
    static long time0 = 0, time1 = 0;
    time1 = micros();
    long duration = (time1 - time0)/1000;
    time0 = time1;
    for (int i = 0; i<SonicNo; i++)
      CVMsg[i] = String(sonicDist[i]);
      
    CVMsg[4] = String(curr0);
    CVMsg[5] = String(volt0);
    CVMsg[6] = String(duration);
  for (int i = 0; i<MsgNo; i++)
  {
    Serial.print(CVMsg[i]);
    if (i<MsgNo-1)
      Serial.print(',');
    else
      Serial.print('\n');
  }
  
  }
  
  #ifdef DEBUG
  if (loopNo == 0)
  {
    static long time0 = 0, time1 = 0;
    time0 = time1;
    time1 = micros();
    for (int i = 0; i<SonicNo; i++)
    {
    Serial.print(sonicDist[i]);
    Serial.print('\t');
    }
    Serial.print(curr0);
    Serial.print(',');
    Serial.print(volt0);
    Serial.print('\t');
    for (int i = 0; i<4; i++)
    {
    Serial.print(CVMsg[i]);
    Serial.print('\t');
    }
    Serial.print(time1 - time0);
    Serial.println();
  }
  #endif
  //END of DEBUG
}

int getSonic(int pinNo)                        //return the pointers of sonic distances, BackUp function
{
    int SoniTRI_PIN = TRI_PIN[pinNo];
                                                        // 1. OUTPUT mode (NOT USED)
    PORTD &= ~SoniTRI_PIN;                              // 2. Set the trigger pin to low for 2uS
    delayMicroseconds(2);
    PORTD |= SoniTRI_PIN;                               // 3. Send a 10uS high to trigger
    delayMicroseconds(10);
    PORTD &= ~SoniTRI_PIN;                              // 4. Send pin low again
                                                        // 5. INPUT mode (NOT USED)
                                                        
    long duration = pulseIn(ECH_PIN[pinNo], HIGH, 30000);
    int distance = duration/58;
    return distance;
  }
