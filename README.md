# arduino
Arduinoを用いたデバイス用ソースコードを記載する

## TDS_sensor
趣味であるコーヒーの濃度の定量的評価に用いられる  
「TDS」と「収率（Extraction Yield）」を測定するデバイスのソースコード  

###＜計測方法＞
抽出したコーヒーに対して温度計（DS18B20）とTDS計（keystudio）を入れる  
それによってプログラム内でTDSと収率を算出し，LCDディスプレイに表示する

###＜改善点＞
問題として，購入したTDS計の計測範囲が0~1000ppmであったのに対し，  
コーヒーの評価には約15000ppmまで計測する必要があった．  

そこでTDS計の基盤上の整流機構を改造することによって，目的精度を達成した

<参考文献>  
keystudio公式wiki，https://wiki.keyestudio.com/KS0429_keyestudio_TDS_Meter_V1.0
