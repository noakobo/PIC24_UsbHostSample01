#include "mcc_generated_files/system.h"

#define FCY 16000000UL  //__delay_msを使うために必要
#include <libpic30.h>   //__delay_msを使うために必要

//プロトタイプ宣言
void USB_Initialize(void); //USBの初期化

/********************
 * USB初期化ルーチン
 ********************/
void USB_Initialize()
{
    /*
     * 27.3.4.1 USB ハードウェアの有効化
     * USB モジュールを使用するには、ソフトウェアから
     * USBPWR ビット (U1PWRC<0>) を「1」にセットする必要があります。
     * これは、起動時のブートシーケンスで行う事ができます。
     */
    U1PWRCbits.USBPWR = 1; // Turn power on

    /* 1.
     * HOSTEN ビットをセット (U1CON<3> = 1) してホストモードを有効にする。
     */
    U1CONbits.HOSTEN = 1;

    /* 2.
     * DPPULDNW および DMPULDWN ビットをセット (U1OTGCON<5:4> = 1) して
     * D+ および D- ラインのプルダウン抵抗を有効にする。
     * DPPULUP および DMPULUP ビットをクリア (U1OTGCON<7:6> = 0) して
     * D+ および D- ラインのプルアップ抵抗を無効にする。
     */
    LATBbits.LATB2 = 1;
    U1OTGCONbits.DPPULDWN = 1;
    U1OTGCONbits.DMPULDWN = 1;
    U1OTGCONbits.DPPULUP = 0;
    U1OTGCONbits.DMPULUP = 0;

    /* 3.
     * SOF の生成が開始する。
     * SOF カウンタの値を 12,000 に設定する。
     * SOFEN ビット(U1CON<0>) に「0」を書き込んで SOF パケットの生成を無効にする。
     */
    LATBbits.LATB3 = 1;
    U1SOF = 12000;
    U1CONbits.SOFEN = 0;

    /* 4.
     * ATTACHIE ビット (U1IE<6>) をセットしてデバイス接続割り込みを許可する。
     */
    U1IRbits.ATTACHIF = 1;
    U1IEbits.ATTACHIE = 1;

    /* 5.
     * デバイス接続割り込み ATTACHIF (U1IR<6>) を待つ。
     * これは、USB デバイスが D+ または D- の状態を「0」から「1」(SE0 から JSTATE) に変更する事によって通知される。
     * デバイス接続割り込みが発生したら、デバイスの電源が安定化するまで待つ ( 最小値 10ms、推奨値 100 ms)。
     */
    LATBbits.LATB4 = 1;
    while (U1IRbits.ATTACHIF == 0);
    __delay_ms(100);

    /* 6.
     * USB制御レジスタ(U1CON)のJSTATEおよびSE0ビットの状態をチェックする
     * (JSTATE(U1CON<7>) = 0なら接続デバイスはロースピード、
     * それ以外なら接続デバイスはフルスピード )。
     */
    /* 7.
     * 接続デバイスがロースピードの場合、
     * アドレス レジスタのロースピード イネーブルビット LSPDEN (U1ADDR<7>) と、
     * エンドポイント 0 制御レジスタのロースピード ビットLSPD (U1EP0<7>) をセットする。
     * 接続デバイスがフルスピードの場合、これらのビットをクリアする。
     */
    LATBbits.LATB13 = 1;
    switch (U1CONbits.JSTATE) {
    case 0://LowSpeed
        U1ADDRbits.LSPDEN = 1;
        U1EP0bits.LSPD = 1;
        printf("LowSpeed\n");
        break;
    case 1://FullSpeed
        U1ADDRbits.LSPDEN = 0;
        U1EP0bits.LSPD = 0;
        printf("FullSpeed\n");
        break;
    }

    /* 8.
     * リセット信号 (U1CON<4> = 1) を 50 ms 以上送信して USB デバイスをリセットする。
     * 50ms が経過したら、リセットを終了する (U1CON<4> = 0)。
     */
    LATBbits.LATB14 = 1;
    U1CONbits.RESET = 1;
    __delay_ms(100);
    U1CONbits.RESET = 0;

    /* 9.
     * 接続デバイスがサスペンドモードに移行しないよう、SOFEN ビット (U1CON<0>) をセットして
     *  SOF パケット生成を有効にする。
     */
    LATBbits.LATB15 = 1;
    U1CONbits.SOFEN = 1;

    /* 10.
     * デバイスがリセットから復帰するまで 10 ms 待つ。
     */
    __delay_ms(10);

    /* 11.
     * エニュメレーション (USB 2.0 仕様書第 9 章参照 ) を実行する。
     */
}

/* ここからメイン */
int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    USB_Initialize();

    while (1) {
        // Add your application code
    }

    return 1;
}

