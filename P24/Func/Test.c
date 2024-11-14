#include "include.h"
#ifdef TEST_P24
extern void SetModeKey(u16 mode);

enum{
    enumTestDisplay      ,
    enumTestKey          ,
    enumTestAT45         ,
    enumTestFRAM         ,
    enumTestLED         ,
    enumReport          ,

    kolv_test        ,
};
/*============================================================================*/
#define tstFn(num, str, fun)
#include "tstP24_macro.h"
#undef tstFn

#define tstFn(num, str, fun) str,
const char *NamePerif[kolv_test]={
#include "tstP24_macro.h"
};
#undef tstFn

#define tstFn(num, str, fun) fun,
const test_fun tstFunPerif[kolv_test]={
#include "tstP24_macro.h"
};
#undef tstFn

#define tstFn(num, str, fun) N##fun,
enum{
#include "tstP24_macro.h"
abracadabra
};
#undef tstFn


u16 testRprt[kolv_test];        // Для хранения результатов тестирования

#define TEST_falure     0       // Тест не пройден
#define TEST_pass       1       // Тест пройден



void Test(void);

/*****************************************************
Тестовое меню кнопок
******************************************************/
const char testQuestion[] = "Тест пройден? <Y> <N>";
const char txtOk[] = "Тест пройден";
const char txtErr[] = "Тест не  пройден";


const char delimiter[] = "\r\n**********************************\r\n";
const char testOk[] = "\r\nТест: НОРМА\r\n";
const char testFail[] = "\r\nТест: БРАК\r\n";

void TestDisplay(void)
{
    u16 numTest;
    char c;

    numTest = 0;

    sprintf(&StrOut[0], "Проверка целостности экрана\r\n");
    testSendString();
    sprintf(&StrOut[0], "Контролировать качество отбражения на экране пульта\r\n");
    testSendString();




    while(1){
        GrafBffClear();
        switch (numTest) {
        case 0:
            //MessageCancel("Нажимайте любую кнопку для пролистывания картинок");
            //numTest++;
            sprintf(&StrOut[0], "\r\nКонтролировать отсутствие незасвеченных пикселей\r\n");
            testSendString();
            memset(GrafBff, 0xff, sizeof(GrafBff) / 2);
            break;
        case 1:
            sprintf(&StrOut[0], "\r\nКонтролировать отсутствие засвеченных пикселей\r\n");
            testSendString();
            memset(GrafBff, 0x00, sizeof(GrafBff) / 2);
            break;
        case 2:
            sprintf(&StrOut[0], "\r\nКонтролировать отображение картинки\r\n");
            testSendString();
            memcpy(GrafBff, Terminator, sizeof(GrafBff) / 2);
            break;
        case 3:
            sprintf(&StrOut[0], "\r\nКонтролировать отображение картинки\r\n");
            testSendString();
            memcpy(GrafBff, Triol, sizeof(GrafBff) / 2);
            break;
        case 4:
            sprintf(&StrOut[0], "%s", testQuestion);
            testSendString();
            while(1){
                c = WaitRx();
                if (c == 'Y' || c == 'y'){
                    testRprt[enumTestDisplay] = 1;
                    sprintf(&StrOut[0], testOk);
                    testSendString();
                    return;
                }
                else if (c == 'N' || c == 'n'){
                    testRprt[enumTestDisplay] = 2;
                    sprintf(&StrOut[0], testFail);
                    testSendString();
                    return;
                }
            }
        }

        GrfOut();

        if (numTest < 3){
            sprintf(&StrOut[0], "Нажмите любую кнопку для показа следующего изображения\r\n");
            testSendString();

            WaitRx();
        }
        numTest++;


    }
}

const char *const txtKey[] = {
    "   ",
    "Вверх",
    "Вниз",
    "Влево",
    "Вправо",
    "Ввод",
    "Отм./?",
    "Меню",
    "Старт",
    "Стоп",
};


void TestKey(void)
{
    keyName_type        Key;
    u16                 outPress, numKey;
    u16                 cntOk, flagNewTest, flagFailure;
    char                c;


    SetModeKey(1);
    cntOk = 0;
    outPress = 0;
    numKey = 1;
    flagNewTest = 1;
    flagFailure = 0;


    while(1){
        GrafBffClear();

        if (flagNewTest){
            flagNewTest = 0;
            sprintf(&StrOut[0], "Нажмите кнопку пульта [%s]\r\n", txtKey[numKey]);
            testSendString();
            sprintf(&StrOut[0], "Нажмите кнопку ПК [N], если кнопка не нажимается\r\n");
            testSendString();
        }

        Key = WaitKey();

        if (Key == KEY_UP){
            DrawIcon16x16(65, 50, up2);
        }
        else if (numKey == KEY_UP){
            DrawIcon16x16(65, 50, targetKey);
        }
        else{
            DrawIcon16x16(65, 50, up1);
        }

        if (Key == KEY_DOWN){
            DrawIcon16x16(65, 100, down2);
        }
        else if (numKey == KEY_DOWN){
            DrawIcon16x16(65, 100, targetKey);
        }
        else{
            DrawIcon16x16(65, 100, down1);
        }

        if (Key == KEY_RIGHT){
            DrawIcon16x16(95, 75, right2);
        }
        else if (numKey == KEY_RIGHT){
            DrawIcon16x16(95, 75, targetKey);
        }
        else{
            DrawIcon16x16(95, 75, right1);
        }
        if (Key == KEY_LEFT){
            DrawIcon16x16(35, 75, left2);
        }
        else if (numKey == KEY_LEFT){
            DrawIcon16x16(35, 75, targetKey);
        }
        else{
            DrawIcon16x16(35, 75, left1);
        }

        if (Key == KEY_ENTER){
            DrawIcon16x16(65, 75, anyKey2);
        }
        else if (numKey == KEY_ENTER){
            DrawIcon16x16(65, 75, targetKey);
        }
        else{
            DrawIcon16x16(65, 75, anyKey1);
        }

        if (Key == KEY_ESC){
            DrawIcon16x16(95, 50, anyKey2);
        }
        else if (numKey == KEY_ESC){
            DrawIcon16x16(95, 50, targetKey);
        }
        else{
            DrawIcon16x16(95, 50, anyKey1);
        }
        if (Key == KEY_FUNC){
            DrawIcon16x16(35, 50, anyKey2);
        }
        else if (numKey == KEY_FUNC){
            DrawIcon16x16(35, 50, targetKey);
        }
        else{
            DrawIcon16x16(35, 50, anyKey1);
        }

        if (Key == KEY_START){
            DrawIcon16x16(45, 120, anyKey2);
        }
        else if (numKey == KEY_START){
            DrawIcon16x16(45, 120, targetKey);
        }
        else{
            DrawIcon16x16(45, 120, anyKey1);
        }

        if (Key == KEY_STOP){
            DrawIcon16x16(85, 120, anyKey2);
        }
        else if (numKey == KEY_STOP){
            DrawIcon16x16(85, 120, targetKey);
        }
        else{
            DrawIcon16x16(85, 120, anyKey1);
        }

        if (Key == numKey && Key != outPress){
            sprintf(&StrOut[0], "Нажата кнопка %s\r\n", txtKey[Key]);
            testSendString();
            outPress = Key;
        }
        else{
            if (outPress && Key == 0){
                cntOk++;
                if (cntOk < 10){
                    sprintf(&StrOut[0], "Отжата кнопка %s\r\n\r\n", txtKey[outPress]);
                    testSendString();
                    //masKey[outPress] = 1;
                    numKey++;
                    flagNewTest = 1;
                }
                outPress = 0;
            }

        }

        if (numKey > NUM_of_KEY){
            SetModeKey(0);
            if(flagFailure == 0){
                sprintf(&StrOut[0], testOk);
                testSendString();
                testRprt[enumTestKey] = 1;
                return;
            }
            else{
                sprintf(&StrOut[0], testFail);
                testSendString();
                testRprt[enumTestKey] = 2;
                return;
            }
        }


        c = WaitRxWoPend();
        if (c == 'N' || c == 'n'){
            flagNewTest = 1;
            sprintf(&StrOut[0], "Кнопка [%s] не работает\r\n\r\n", txtKey[numKey]);
            testSendString();
            numKey++;
            flagFailure = 1;
        }

        GrfOut();
        Key = 0;
    }

}

void TestAT45(void)
{
    char BufWrAA[528];
    char BufWr55[528];
    char BufRd[528];
    u8   err;
    void *AdrAT45;
    u16  procPrev, procNew, dif;

    procPrev = 0;
    procNew = 0;

    err = 0;
    memset(&BufWrAA[0], 0xaa, sizeof(BufWrAA));
    memset(&BufWr55[0], 0x55, sizeof(BufWr55));
    sprintf(&StrOut[0], "Начат процесс тестирования. Ожидайте...\r\n");
    testSendString();
    for(AdrAT45 = (void *)0; AdrAT45 < (void *)(2162688); AdrAT45 = (void *)((u32)AdrAT45 + sizeof(BufWrAA))){
        AT45WR(AdrAT45, BufWrAA, sizeof(BufWrAA));
        AT45RD(BufRd, AdrAT45, sizeof(BufRd));
        if(strncmp(BufRd, BufWrAA, sizeof(BufRd))){
           err = 1;
           break;
        }
        AT45WR(AdrAT45, BufWr55, sizeof(BufWrAA));
        AT45RD(BufRd, AdrAT45, sizeof(BufRd));
        if(strncmp(BufRd, BufWr55, sizeof(BufRd))){
           err = 1;
           break;
        }
        procNew = (u32)AdrAT45 / 21626;
        dif = procNew - procPrev;
        if (dif >= 1){
            procPrev = procNew;
            memset(&StrOut[0], '#', dif);
            StrOut[dif] = 0;
            testSendString();
        }
    }
    if (err){
        sprintf(&StrOut[0], "\r\n%s - %d\r\n", "Ошибка страницы", (u32)AdrAT45 / 528);
        testSendString();
        sprintf(&StrOut[0], testFail);
        testSendString();
        testRprt[enumTestAT45] = 2;
        return;
    }

    sprintf(&StrOut[0], testOk);
    testSendString();

    testRprt[enumTestAT45] = 1;
}

void TestFRAM(void)
{
    char BufWrAA[512];
    char BufWr55[512];
    char BufRd[512];
    u8   err;
    void *AdrFRAM;
    u16  procPrev, procNew, dif;

    procPrev = 0;
    procNew = 0;
    err = 0;

    memset(&BufWrAA[0], 0xaa, sizeof(BufWrAA));
    memset(&BufWr55[0], 0x55, sizeof(BufWr55));
    for(AdrFRAM = (void *)0; AdrFRAM < (void *)(32768); AdrFRAM = (void *)((u32)AdrFRAM + sizeof(BufWrAA))){
        GrafBffClear();
        FM25WR(AdrFRAM, BufWrAA, sizeof(BufWrAA));
        FM25RD(BufRd, AdrFRAM, sizeof(BufRd));
        if(strncmp(BufRd, BufWrAA, sizeof(BufRd))){
           err = 1;
           break;
        }
        FM25WR(AdrFRAM, BufWr55, sizeof(BufWrAA));
        FM25RD(BufRd, AdrFRAM, sizeof(BufRd));
        if(strncmp(BufRd, BufWr55, sizeof(BufRd))){
           err = 1;
           break;
        }
        procNew = (u32)AdrFRAM / 327;
        dif = procNew - procPrev;
        if (dif >= 1){
            procPrev = procNew;
            memset(&StrOut[0], '#', dif);
            StrOut[dif] = 0;
            testSendString();
//            StrOut[0] = '/a;
//            StrOut[1] = 0;
/*            testSendString();
            sprintf(&StrOut[0], "\xA\xA%d%%\xA\xA", procNew);
            testSendString();*/

        }
    }

/*    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();*/
    if (err){
        sprintf(&StrOut[0], "\r\n%s - %d\r\n", "Ошибка страницы", (u32)AdrFRAM / 512);
        testSendString();
        sprintf(&StrOut[0], testFail);
        testSendString();
        testRprt[enumTestFRAM] = 2;
        return;
    }
    sprintf(&StrOut[0], testOk);
    testSendString();

    testRprt[enumTestFRAM] = 1;
}

const char *const txtLed[] = {
    "Авария",
    "Готовность",
    "Работа",
};

void TestLED(void)
{
    u16 i, flagFailure;
    char c;

    flagFailure = 0;

    for (i = 0; i < 3; i++){

        if (i == 0){
            setCrashIndicator();
            clrReadyIndicator();
            clrWorkIndicator();
        }
        else if (i == 1){
            clrCrashIndicator();
            setReadyIndicator();
            clrWorkIndicator();
        }
        else if (i == 2){
            clrCrashIndicator();
            clrReadyIndicator();
            setWorkIndicator();
        }
        sprintf(&StrOut[0], "Контролировать свечение индикатора [%s]\r\n", txtLed[i]);
        testSendString();
        sprintf(&StrOut[0], "%s", testQuestion);
        testSendString();

        while(1){
            c = WaitRx();
            if (c == 'Y' || c == 'y'){
                sprintf(&StrOut[0], "Индикатор [%s] НОРМА\r\n\r\n", txtLed[i]);
                testSendString();
                break;
            }
            else if (c == 'N' || c == 'n'){
                sprintf(&StrOut[0], "Индикатор [%s] БРАК\r\n\r\n", txtLed[i]);
                testSendString();
                flagFailure = 1;
                break;
            }
        }
    }

    if (flagFailure){
        sprintf(&StrOut[0], testFail);
        testSendString();
        testRprt[enumTestLED] = 2;
    }
    else{
        sprintf(&StrOut[0], testOk);
        testSendString();
        testRprt[enumTestLED] = 1;
    }

    clrCrashIndicator();
    clrReadyIndicator();
    clrWorkIndicator();

}

void TestAll(void)
{
    u16 i;
    for (i = 0; i < kolv_test - 2; i++){
        if (tstFunPerif[i] != NULL)
            sprintf(&StrOut[0], &delimiter[0]);
            testSendString();
            sprintf(&StrOut[0], "%s\r\n", NamePerif[i]);
            testSendString();
            tstFunPerif[i]();
            GrafBffClear();
            memcpy(GrafBff, Triol, sizeof(GrafBff) / 2);
            GrfOut();
    }
}

void Report(void)
{
    u16 i, cnt;
    char c;

    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();
    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();

    for (i = 0, cnt = 0; i < kolv_test - 2; i++){
        if (testRprt[i] == 1){
            sprintf(&StrOut[0], "%d - %s НОРМА\r\n", i, NamePerif[i]);
            testSendString();
            cnt++;
        }
        else if (testRprt[i] == 2){
            sprintf(&StrOut[0], "%d - %s БРАК\r\n", i, NamePerif[i]);
            testSendString();
        }
        else{
            sprintf(&StrOut[0], "%d - %s НЕ ПРОВОДИЛСЯ\r\n", i, NamePerif[i]);
            testSendString();
        }
        testRprt[i] = 0;
    }

/*    sprintf(&StrOut[0], "Выключить питание пульта\r\n");
    testSendString();*/
    if (cnt >= kolv_test - 2){
        sprintf(&StrOut[0], "\r\nТЕСТ ПРОЙДЕН\r\n");
        testSendString();
    }
    else{
        sprintf(&StrOut[0], "\r\nТЕСТ НЕПРОЙДЕН\r\n");
        testSendString();
    }

    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();
    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();

    sprintf(&StrOut[0], "[R] - повторить тесты");
    testSendString();


    do{
        c = WaitRx();
        c++;
        c--;
        if ((c == 'R') || (c == 'r')) break;
    }while(1);

}


u16 MenuTest(void)
{
    u16 i;
    char c;


    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();
    for (i = 0; i < kolv_test; i++){
        if (tstFunPerif[i] == NULL)
            break;

        sprintf(&StrOut[0], "%d - %s\r\n", i, NamePerif[i]);
        testSendString();


    }
    sprintf(&StrOut[0], "\r\n");
    testSendString();
    for (i = 0; i < kolv_test - 2; i++){
        if (testRprt[i] == 1){
            sprintf(&StrOut[0], "%d - %s НОРМА\r\n", i, NamePerif[i]);
            testSendString();
        }
        else if (testRprt[i] == 2 && i < kolv_test - 1){
            sprintf(&StrOut[0], "%d - %s БРАК\r\n", i, NamePerif[i]);
            testSendString();
        }
        else{
            sprintf(&StrOut[0], "%d - %s НЕ ПРОВОДИЛСЯ\r\n", i, NamePerif[i]);
            testSendString();
        }

    }
    sprintf(&StrOut[0], "\r\nВыберите номер теста\r\n");
    testSendString();
    sprintf(&StrOut[0], &delimiter[0]);
    testSendString();
    while(1){
        c = WaitRx();
        if (c >= 0x30 && c < kolv_test + 0x30)
            break;
    }
    return c - 0x30;
}



void Test123(void)
{
    u16 i;
    char c;

    StrOut[0] = 0;
    testSendString();


    sprintf(&StrOut[0], "\r\n\r\n\r\n\r\nДля начала тестирования нажмите [s]\r\n");
    testSendString();
    while(1){
        c = WaitRx();
        if (c == 's' || c == 'S')
            break;
    }

        while(1){
            i = MenuTest();
            sprintf(&StrOut[0], "Выбран - %s\r\n", NamePerif[i]);
            testSendString();
            tstFunPerif[i]();
            GrafBffClear();
            memcpy(GrafBff, Triol, sizeof(GrafBff) / 2);
            GrfOut();
        }

}

#endif
