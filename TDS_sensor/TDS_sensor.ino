#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_TDS_SENSOR A1
#define VREF (5.0) // 入力電圧の大きさ
#define SAMPLE_COUNT (25) // データのサンプル数 (奇数で設定)
#define ONE_WIRE_BUS 2 // データ(黄)で使用するポート番号
#define SENSER_BIT    9      // 精度の設定bit

// 要変更
#define COFFEE_MASS 225 // コーヒー液体の質量
#define COFFEE_BEENS_MASS 15 // コーヒー液体の質量

int analogBuffer[SAMPLE_COUNT];
int analogBufferIndex = 0;
float TEMPERATURE;

//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
unsigned long analogSampleTimepoint = 0UL;
unsigned long printTimepoint = 0UL;
String pad = String( "         " );

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temp_sensor(&oneWire);
DeviceAddress temp = {0x28, 0x71, 0xB9, 0x46, 0xD4, 0xE7, 0x7E, 0x9F}; //DS18B20のアドレス

void setup() {
  // LCD の準備
  lcd.begin(16, 2);
  // センサーピンの設定
  pinMode( PIN_TDS_SENSOR, INPUT );

  // サンプル取得と表示のタイミング
  analogSampleTimepoint = millis();
  printTimepoint - millis();

  // 
  temp_sensor.setResolution(SENSER_BIT);
  delay(1000);
}

void loop() {     
  // データ収集 (40ms毎)
  if( millis() - analogSampleTimepoint > 40UL ) {
    // 温度取得要求
    temp_sensor.requestTemperatures();  
    TEMPERATURE = temp_sensor.getTempC(temp);
    analogSampleTimepoint = millis();
    // 40msが25個収集できたら（1s）リセット
    analogBuffer[analogBufferIndex++] = analogRead( PIN_TDS_SENSOR );
    if( SAMPLE_COUNT == analogBufferIndex ) {
      analogBufferIndex = 0;
    }
  
  }

  // 表示 (1秒毎) 
  if( millis() - printTimepoint > 1000UL ) {
    printTimepoint = millis();
    // 収集した25データをソートして中央値を取得
    qsort( analogBuffer, SAMPLE_COUNT, sizeof(int), comparator_ints );
    int median =  analogBuffer[ (SAMPLE_COUNT - 1)/2 ];

    // TDS 値の計算
    //  電圧に変換 Arduino UNO は 0 から 1023 まで
    float averageVoltage = median * (float) VREF / ( 1024.0 - 1.0 );
    //  温度補正
    float compensationCoefficient = 1.0 + 0.02 * ( TEMPERATURE - 25.0 );
    float compensationVolatge = averageVoltage / compensationCoefficient;
    //  電圧から TDS 値に変換
    float tdsValue = ( 133.42 * compensationVolatge * compensationVolatge * compensationVolatge 
      - 255.86 * compensationVolatge * compensationVolatge 
      + 857.39 * compensationVolatge) * 0.5;

    // 収率（EY）の計算
    float EY = tdsValue*0.0001 * COFFEE_MASS / COFFEE_BEENS_MASS;

    // LCD への表示
    // ppm
    String v_p = String( tdsValue, 0 );
    String s_p = String( "TDS:" + v_p + "ppm" + pad );
    lcd.setCursor(0,0);
    lcd.print( s_p );

    // EY
    String v_e = String( EY, 1 );
    String s_e = String( "EY :" + v_e + " %" + pad );
    lcd.setCursor(0,1);
    lcd.print( s_e );
  }
}

// qsort のための比較関数
int comparator_ints( const void *p, const void *q ) {
  int x = *(const int*) p;
  int y = *(const int*) q;
  if( x < y ) {
    return -1;
  }
  else if( x > y ) {
    return 1;
  }
  return 0;
}