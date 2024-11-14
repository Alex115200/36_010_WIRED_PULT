/************************** (C) Copyright 2013 Triol ***************************
* File Name          : Message.c
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.26
* TS Version         : 2013.04.26
* Description        : Function for work with message
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "globalIncludes.h"
#include <stdlib.h>


/* Private define ------------------------------------------------------------*/
// глобальная переменная для работы с сообщениями
// ВНИМАНИЕ! с данной переменной допускается работать
// только через функции данного модуля
MessageFlash_type MessageGlobal;

char MasMess[COUNT_MESS][LEN_MESS];
char MasTitle[COUNT_TITLE][LEN_TITLE];
char MasFlash[COUNT_FLASH][LEN_FLASH];

const char * msgStrings[] = 
{
  "",
};

const char * msgStringsAlarm[] = 
{
    "Команда СТОП\nне прошла!",
    "АВАРИЯ!!!\nОшибка силового\nключа, фаза U",    // 1
    "АВАРИЯ!!!\nОшибка силового\nключа, фаза V",    // 2
    "АВАРИЯ!!!\nОшибка силового\nключа, фаза W",    // 3
    "АВАРИЯ!!!\nОшибка тормоз-\nного ключа",        // 4
    "АВАРИЯ!!!\nМТЗ\nпо компаратору,\nфаза U",      // 5
    "АВАРИЯ!!!\nМТЗ\nпо компаратору,\nфаза V",      // 6
    "АВАРИЯ!!!\nМТЗ\nпо компаратору,\nфаза W",      // 7
    "АВАРИЯ!!!\nМТЗ\nпрограммное,\nфаза U",         // 8
    "АВАРИЯ!!!\nМТЗ\nпрограммное,\nфаза V",         // 9
    "АВАРИЯ!!!\nМТЗ\nпрограммное,\nфаза W",         // 10
    "АВАРИЯ!!!\nНизкое Ud",                         // 11
    "АВАРИЯ!!!\nВысокое Ud",                        // 12
    "АВАРИЯ!!!\nВремени\nзаряда Ud",                // 13
    "АВАРИЯ!!!\nПерегрев сило-\nвых ключей",        // 14
    "АВАРИЯ!!!\nМТЗ тока утечки",                   // 15
    "АВАРИЯ!!!\nЗапрет пуска МТЗ",                  // 16
    "АВАРИЯ!!!\nНе определена",                     // 17
    "АВАРИЯ!!!\nНе определена",                     // 18
    "АВАРИЯ!!!\nНе определена",                     // 19
    "АВАРИЯ!!!\nНе определена",                     // 20
    "АВАРИЯ!!!\nНе определена",                     // 21
    "АВАРИЯ!!!\nНе определена",                     // 22
    "АВАРИЯ!!!\nНе определена",                     // 23
    "АВАРИЯ!!!\nНе определена",                     // 24
    "АВАРИЯ!!!\nНе определена",                     // 25
    "АВАРИЯ!!!\nНе определена",                     // 26
    "АВАРИЯ!!!\nНе определена",                     // 27
    "АВАРИЯ!!!\nНе определена",                     // 28
    "АВАРИЯ!!!\nНе определена",                     // 29
    "АВАРИЯ!!!\nНе определена",                     // 30
    "АВАРИЯ!!!\nНе определена",                     // 31
    "АВАРИЯ!!!\nНажата кнопка\nАВАРИЙНЫЙ СТОП",     // 32
    "АВАРИЯ!!!\nОбрыв входных фаз",                 // 33
    "АВАРИЯ!!!\nКонтактора",                        // 34
    "АВАРИЯ!!!\nТормозного\nсопротивления",         // 35
    "АВАРИЯ!!!\n Ошибка \nфазировки",               // 36
    "АВАРИЯ!!!\nВремени заряда",                    // 37
    "АВАРИЯ!!!\nПерегруз\nдвигателя",               // 38
    "АВАРИЯ!!!\nАналоговый\nвход 1",                // 39
    "АВАРИЯ!!!\nАналоговый\nвход 2",                // 40
    "АВАРИЯ!!!\nВыходная\nчастота больше\nF_MAX",   // 41
    "АВАРИЯ!!!\nВыходная\nчастота меньше\nF_MIN",   // 42
    "АВАРИЯ!!!\nОбрыв фазы U",                      // 43
    "АВАРИЯ!!!\nОбрыв фазы V",                      // 44
    "АВАРИЯ!!!\nОбрыв фазы W",                      // 45
    "АВАРИЯ!!!\nПерегрев\nдвигателя",               // 46
    "АВАРИЯ!!!\nОшибка ModBus",                     // 47
    "АВАРИЯ!!!\nРеверс запрещен",                   // 48
    "АВАРИЯ!!!\nПерегрев\nохладителя",              // 49
    "АВАРИЯ!!!\nНет воды",                          // 50
    "АВАРИЯ!!!\n Ошибка \nАвтовентель",             // 51
    "АВАРИЯ!!!\nОшибка \nЗапрещенные \nчастоты",    // 52
    "АВАРИЯ!!!\nНедогруз \nдвигателя",              // 53
    "АВАРИЯ!!!\nКрайняя \nточка",                   // 54
    "АВАРИЯ!!!\nОбрыв \nремня",                     // 55
    "АВАРИЯ!!!\nОтсутствие \n потока",              // 56
    "АВАРИЯ!!!\nСухой \nход",                       // 57
    "АВАРИЯ!!!\nКороткий \nцикл",                   // 58
    "АВАРИЯ!!!\nДисбаланс \nтоков",                 // 59
    "АВАРИЯ!!!\nАналоговый\nвход 3",                // 60
    "АВАРИЯ!!!\nАналоговый\nвход 4",                // 61
    "АВАРИЯ!!!\nАналоговый\nвход 5",                // 62
    "АВАРИЯ!!!\nРезерв",                            // 63    
    "АВАРИЯ!!!\nПерегрев\nтрансформатора",          // 64 - длЯ АМЗ. Шкаф ШТ
    "АВАРИЯ!!!\nНеисправен\nвентилятор 1",           // 65 - длЯ АМЗ. Шкаф ШТ    
    "АВАРИЯ!!!\nНеисправен\nвентилятор 2",           // 66 - длЯ АМЗ. Шкаф ШТ        
    "АВАРИЯ!!!\nНеисправен\nвентилятор 3",           // 67 - длЯ АМЗ. Шкаф ШТ        
    "АВАРИЯ!!!\nНеисправен\nвентилятор 4",           // 68 - длЯ АМЗ. Шкаф ШТ        
    "АВАРИЯ!!!\nОткрыта дверь\nтрансформатора",      // 69 - длЯ АМЗ. Шкаф ШТ        
};
const char * msgStringsTitle[] = 
{
    "Статус",
    "РУЧНОЙ",
    "АВТОМАТ",
    "мин.",
    "макс.",
    "Ввод №параметра",
    "Параметр №:",
    "Редакт. статус",
    "Да",
    "Нет",
    "‘Отм’ - выход"

};
const char * msgStringsMess[] = 
{
    "Installation error!",
    "Connection error!",
    "Passwords are not available!",
    "Файл пуст!",
    "Файл занят.\nПерезаписать?",
    "Ошибка записи!",
    "Ошибка проверки совместимости!",
    "Ошибка чтения!",
    "Чтение уставок:",
    "Запись уставок:",
    "Installation:",
    "Подсказки нет",
    "Параметр редактируется в останове!",
    "Параметр не редактируемый!",
    "Нет доступа!",
    "Нет журнала!",
    "Привод не установлен!",
    "Привод не прошел проверку!",
    "Повторить установку привода?",
    "Не возможно установить больше приводов",
    "Параметр не существует или нет доступа!",
    "No connect",
    "Не задано функциональное назначение ПЧ!"
};
//===================================================================
//          Расстановка знаков пунктуации
//===================================================================
extern u32 FindSpace(const char *str);
//===================================================================
//          Нахождение знаков пунктуации
//===================================================================
extern u32 FindHyphen(const char *str);

// Возвращает текст соответствующий определенному индексу
const char * GetString(u16 IndexString)
{
    return (IndexString < COUNT_STRINGS) ? msgStrings[IndexString] : 0;
}

// Возвращает текст аварии соответствующий определенному индексу
const char * GetStringAlarm(u16 IndexString)
{
    return (IndexString < COUNT_FLASH) ? &MasFlash[IndexString][0] : 0;
}

// Возвращает текст подписей, статус, РУЧНОЙ, АВТОМАТ и т.д.
char * GetStringTitle(MessageTitul_type IndexString)
{
    return (IndexString < COUNT_TITLE) ? &MasTitle[IndexString][0] : 0;
}

// Возвращает текст сообщений
const char * GetStringMess(u16 IndexString)
{
    return (IndexString < COUNT_MESS) ? &MasMess[IndexString][0] : 0;
}
/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
// Функция установки стандартного сообщения 
MessageFlashErr_type MessageSet(MessageFlashFlag_type Flag, u16 Time)
{
    MessageFlashErr_type Err;
    
    Err = MessageFlashErr_OK;
    MessageGlobal.MessageFlag = Flag;
    MessageGlobal.MessageString = GetString(Flag);
    MessageGlobal.TimeStart = GetSecCount();
    MessageGlobal.TimeStop = Time;
    return Err;
}
// Функция установки аварийного сообщения 
MessageFlashErr_type MessageSetAlarm(MessageFlashFlag_type Flag, u16 Time)
{
    MessageFlashErr_type Err;
    
    Err = MessageFlashErr_OK;
    MessageGlobal.MessageFlag = Flag;
    MessageGlobal.MessageString = GetStringAlarm(Flag);
    MessageGlobal.TimeStart = GetSecCount();
    MessageGlobal.TimeStop = Time;
    return Err;
}
// Функция установки сообщения 
MessageFlashErr_type MessageSetString(MessageFlashFlag_type Flag, u8 *String)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// Функция установки стандартного сообщения в очередь
MessageFlashErr_type MessageSetInQueue(MessageFlashFlag_type Flag)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// Функция установки сообщения в очередь
MessageFlashErr_type MessageSetInQueueString(MessageFlashFlag_type Flag, u8 *String)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// Функция вывода сообщения
MessageFlashErr_type MessageShow(keyName_type Key)
{
    MessageFlashErr_type Err;
    
    Err = MessageFlashErr_OK;
    if (Key != NON_KEY)
    {
        MessageClear(0);
        Err = MessageFlashErr_ERR;
        return Err;
    }
    if (MessageGlobal.MessageFlag != MessageFlashFlag_NONE && (MessageGlobal.TimeStop > GetSecLeft(MessageGlobal.TimeStart)))
    {
        Format_MessageFlash(MessageGlobal.MessageString);
    }
    else
    {
        MessageClear(0);
    }
    return Err;
}
// Функция инициализации текущего сообщения
MessageFlashErr_type MessageInit(MessageFlashFlag_type Flag, u8 *String, u8 Num)
{
    MessageFlashErr_type Err;
    
    Err = MessageFlashErr_OK;
    return Err;
}
// функция стирания выводимого сообщения
MessageFlashErr_type MessageClear(u8 Num)
{
    MessageFlashErr_type Err;
    MessageGlobal.MessageFlag = MessageFlashFlag_NONE;
    MessageGlobal.MessageString = GetString(0);
    MessageGlobal.TimeStart = 0;
    MessageGlobal.TimeStop = 0;
    Err = MessageFlashErr_OK;
    return Err;
}

// Функция определения активности сообщения
MessageFlashErr_type MessageActiv(void)
{
    MessageFlashErr_type Err;
    
    if (MessageGlobal.MessageFlag != MessageFlashFlag_NONE && (MessageGlobal.TimeStop > GetSecLeft(MessageGlobal.TimeStart)))
    {
        Err = MessageFlashErr_OK;
    }
    else
    {
        Err = MessageFlashErr_ERR;
    }
    return Err;
}

// Вывод сообщения в буфер
u16 Format_MessageFlash(const char *const Msg)
{
    const char  *pStr;
    u32         posFrmt;
    u16         szMsg, szCopy, skipStr, stop;
    char        strtmp[4][24];
    char        c;
    char        i;


    pStr = Msg;
    if (pStr == NULL) return 0;

    szMsg = strlen(pStr);
    skipStr = 0;
    stop = 0;
    
    ClearRectangle(2, 50, 155, 116);
    DrawRectangle(2, 50, 155, 116);
    while (szMsg != 0 && skipStr < MAX_STR_COUNT_MESSAGE_FLASH) 
    {
        if (szMsg > MAX_STR_MESSAGE_FLASH) 
        {
            memcpy(&strtmp[skipStr][0], pStr, MAX_STR_MESSAGE_FLASH);
            strtmp[skipStr][MAX_STR_MESSAGE_FLASH] = 0;
            szCopy = 0;
            posFrmt = FindSpace(&strtmp[skipStr][0]);
            if (!posFrmt && strtmp[skipStr][posFrmt] == '\n')
            {  
                szCopy = posFrmt + 1;
                szMsg -= szCopy;
                pStr++;
                continue;
            }
            if (posFrmt > MAX_STR_MESSAGE_FLASH - 5 && (strtmp[skipStr][posFrmt] != '\n' && strtmp[skipStr][posFrmt] != '\r')) 
            {
                szCopy = posFrmt + 1;
                strtmp[skipStr][szCopy] = 0;
                szMsg -= szCopy;
            }
            else if (strtmp[skipStr][posFrmt] != '\n' && strtmp[skipStr][posFrmt] != '\r')
            {
                c = strtmp[skipStr][MAX_STR_MESSAGE_FLASH - 2];
                strtmp[skipStr][MAX_STR_MESSAGE_FLASH - 2] = 0;
                posFrmt =  FindHyphen(&strtmp[skipStr][0]);
                strtmp[skipStr][MAX_STR_MESSAGE_FLASH - 2] = c;
                if (posFrmt)
                {
                    szCopy = posFrmt + 1;
                    strtmp[skipStr][szCopy] = '-';
                    strtmp[skipStr][szCopy + 1] = 0;
                    szMsg -= szCopy;
                }
                else
                {
                    szMsg -= MAX_STR_MESSAGE_FLASH;
                    szCopy = MAX_STR_MESSAGE_FLASH;
                }
            }
            else
            {
                szCopy = posFrmt + 1;
                strtmp[skipStr][posFrmt] = 0;
                szMsg -= szCopy;
            }
        }
        else
        {
            memcpy(&strtmp[skipStr][0], pStr, szMsg);
            posFrmt = FindSpace(&strtmp[skipStr][0]);
            if (!posFrmt && strtmp[skipStr][posFrmt] == '\n')
            {  
                pStr++;
                szCopy = posFrmt + 1;
                szMsg -= szCopy;
            }
            memcpy(&strtmp[skipStr][0], pStr, szMsg);
            strtmp[skipStr][szMsg] = 0;
            szCopy = szMsg;
            szMsg = 0;
            stop = 1;
        }
        pStr += szCopy;
        skipStr++;
    }
    for (i = 0; i < skipStr; i++) 
    {
        StrOutFont(((10*(MAX_STR_MESSAGE_FLASH - strlen(&strtmp[i][0]))) / 2), i * 16 + 50 + (MAX_STR_COUNT_MESSAGE_FLASH - skipStr) * 8, NO_INV, &strtmp[i][0], &fontNormal, leftAlign );
    }
    if (!stop)
    {
        DrawIcon16x16(65, 135, downSmall);
    }

    return stop;
}

// Стирает прямоугольник
void ClearRectangle(u16 xPos, u16 yPos, u16 xPos2, u16 yPos2)
{
    u8      *cPnt;
    u8      xOfs, i, j;
    u8      xOfs2;
    u8      xStart, xStop;

    cPnt = &GrafBff[(xPos / 8) + yPos * (YPIXELS / 8)];    //Вычисление указателя в буфере по заданным координатам
    xOfs = xPos & 7;                                          //Смещение в битах
    xOfs2 = xPos2 & 7;
    xStart = xPos/8;
    xStop = xPos2/8;
    
    for(i = 0; i < (yPos2 - yPos); i++, cPnt += YPIXELS / 8)
    {
        for (j = 0; j < (xStop - xStart) + 1; j++) 
        {
            if (j == 0 && j != (xStop - xStart))
            {
                cPnt[j] &= 0xFF << (8 - xOfs);
            }
            else if (j == 0 && j == (xStop - xStart))
            {
                cPnt[j] &= 0xFF << (8 - xOfs);
                cPnt[j] &= 0xFF >> xOfs2;
            }
            else if (j == (xStop - xStart))
            {
                cPnt[j] &= 0xFF >> xOfs2;
            }
            else
            {
                cPnt[j] &= 0;
            }
        }
    }
}

/************************** (C) Copyright 2013 Triol **************************/