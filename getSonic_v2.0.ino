//2019-07-09: v1.0.0 註解
//以下以更底層的控制方法編寫程式, 可同時啟動多個SRF04超音波感測器
//讀取A0及A1為量測電流/電壓的pin
//將6顆超音波及A0, A1的值以UART形式傳送
//更改TRI_PIN , ECH_PIN ,DDRD,CV_Msg print
const int SonicNo = 6;
const int TRI_PIN[SonicNo] = {4,8,16,32,64,128};
const int ECH_PIN[SonicNo] = {8,9,10,11,12,13};
const int LP_DEG = 16;
const int MsgNo = 9;
String CVMsg[MsgNo];
byte sonicDist[SonicNo];

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  DDRD = B11111111;
  DDRB = B11000000; 
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
      
    CVMsg[6] = String(curr0);
    CVMsg[7] = String(volt0);
    CVMsg[8] = String(duration);
  for (int i = 0; i<MsgNo; i++)
  {
    Serial.print(CVMsg[i]);
    if (i<MsgNo-1)
      Serial.print(',');
    else
      Serial.print('\n');
  }
  
  }

//  CVMsg[0] = curr0 >> 5;
//  CVMsg[1] = curr0 & B00011111;
//  CVMsg[2] = volt0 >> 5;
//  CVMsg[3] = volt0 & B00011111;  

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
    for (int i = 0; i<6; i++)
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
