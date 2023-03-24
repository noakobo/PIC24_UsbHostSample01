#include "mcc_generated_files/system.h"

#define FCY 16000000UL  //__delay_ms���g�����߂ɕK�v
#include <libpic30.h>   //__delay_ms���g�����߂ɕK�v

//�v���g�^�C�v�錾
void USB_Initialize(void); //USB�̏�����

/********************
 * USB���������[�`��
 ********************/
void USB_Initialize()
{
    /*
     * 27.3.4.1 USB �n�[�h�E�F�A�̗L����
     * USB ���W���[�����g�p����ɂ́A�\�t�g�E�F�A����
     * USBPWR �r�b�g (U1PWRC<0>) ���u1�v�ɃZ�b�g����K�v������܂��B
     * ����́A�N�����̃u�[�g�V�[�P���X�ōs�������ł��܂��B
     */
    U1PWRCbits.USBPWR = 1; // Turn power on

    /* 1.
     * HOSTEN �r�b�g���Z�b�g (U1CON<3> = 1) ���ăz�X�g���[�h��L���ɂ���B
     */
    U1CONbits.HOSTEN = 1;

    /* 2.
     * DPPULDNW ����� DMPULDWN �r�b�g���Z�b�g (U1OTGCON<5:4> = 1) ����
     * D+ ����� D- ���C���̃v���_�E����R��L���ɂ���B
     * DPPULUP ����� DMPULUP �r�b�g���N���A (U1OTGCON<7:6> = 0) ����
     * D+ ����� D- ���C���̃v���A�b�v��R�𖳌��ɂ���B
     */
    LATBbits.LATB2 = 1;
    U1OTGCONbits.DPPULDWN = 1;
    U1OTGCONbits.DMPULDWN = 1;
    U1OTGCONbits.DPPULUP = 0;
    U1OTGCONbits.DMPULUP = 0;

    /* 3.
     * SOF �̐������J�n����B
     * SOF �J�E���^�̒l�� 12,000 �ɐݒ肷��B
     * SOFEN �r�b�g(U1CON<0>) �Ɂu0�v����������� SOF �p�P�b�g�̐����𖳌��ɂ���B
     */
    LATBbits.LATB3 = 1;
    U1SOF = 12000;
    U1CONbits.SOFEN = 0;

    /* 4.
     * ATTACHIE �r�b�g (U1IE<6>) ���Z�b�g���ăf�o�C�X�ڑ����荞�݂�������B
     */
    U1IRbits.ATTACHIF = 1;
    U1IEbits.ATTACHIE = 1;

    /* 5.
     * �f�o�C�X�ڑ����荞�� ATTACHIF (U1IR<6>) ��҂B
     * ����́AUSB �f�o�C�X�� D+ �܂��� D- �̏�Ԃ��u0�v����u1�v(SE0 ���� JSTATE) �ɕύX���鎖�ɂ���Ēʒm�����B
     * �f�o�C�X�ڑ����荞�݂�����������A�f�o�C�X�̓d�������艻����܂ő҂� ( �ŏ��l 10ms�A�����l 100 ms)�B
     */
    LATBbits.LATB4 = 1;
    while (U1IRbits.ATTACHIF == 0);
    __delay_ms(100);

    /* 6.
     * USB���䃌�W�X�^(U1CON)��JSTATE�����SE0�r�b�g�̏�Ԃ��`�F�b�N����
     * (JSTATE(U1CON<7>) = 0�Ȃ�ڑ��f�o�C�X�̓��[�X�s�[�h�A
     * ����ȊO�Ȃ�ڑ��f�o�C�X�̓t���X�s�[�h )�B
     */
    /* 7.
     * �ڑ��f�o�C�X�����[�X�s�[�h�̏ꍇ�A
     * �A�h���X ���W�X�^�̃��[�X�s�[�h �C�l�[�u���r�b�g LSPDEN (U1ADDR<7>) �ƁA
     * �G���h�|�C���g 0 ���䃌�W�X�^�̃��[�X�s�[�h �r�b�gLSPD (U1EP0<7>) ���Z�b�g����B
     * �ڑ��f�o�C�X���t���X�s�[�h�̏ꍇ�A�����̃r�b�g���N���A����B
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
     * ���Z�b�g�M�� (U1CON<4> = 1) �� 50 ms �ȏ㑗�M���� USB �f�o�C�X�����Z�b�g����B
     * 50ms ���o�߂�����A���Z�b�g���I������ (U1CON<4> = 0)�B
     */
    LATBbits.LATB14 = 1;
    U1CONbits.RESET = 1;
    __delay_ms(100);
    U1CONbits.RESET = 0;

    /* 9.
     * �ڑ��f�o�C�X���T�X�y���h���[�h�Ɉڍs���Ȃ��悤�ASOFEN �r�b�g (U1CON<0>) ���Z�b�g����
     *  SOF �p�P�b�g������L���ɂ���B
     */
    LATBbits.LATB15 = 1;
    U1CONbits.SOFEN = 1;

    /* 10.
     * �f�o�C�X�����Z�b�g���畜�A����܂� 10 ms �҂B
     */
    __delay_ms(10);

    /* 11.
     * �G�j�������[�V���� (USB 2.0 �d�l���� 9 �͎Q�� ) �����s����B
     */
}

/* �������烁�C�� */
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

