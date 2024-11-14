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
// ���������� ���������� ��� ������ � �����������
// ��������! � ������ ���������� ����������� ��������
// ������ ����� ������� ������� ������
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
    "������� ����\n�� ������!",
    "������!!!\n������ ��������\n�����, ���� U",    // 1
    "������!!!\n������ ��������\n�����, ���� V",    // 2
    "������!!!\n������ ��������\n�����, ���� W",    // 3
    "������!!!\n������ ������-\n���� �����",        // 4
    "������!!!\n���\n�� �����������,\n���� U",      // 5
    "������!!!\n���\n�� �����������,\n���� V",      // 6
    "������!!!\n���\n�� �����������,\n���� W",      // 7
    "������!!!\n���\n�����������,\n���� U",         // 8
    "������!!!\n���\n�����������,\n���� V",         // 9
    "������!!!\n���\n�����������,\n���� W",         // 10
    "������!!!\n������ Ud",                         // 11
    "������!!!\n������� Ud",                        // 12
    "������!!!\n�������\n������ Ud",                // 13
    "������!!!\n�������� ����-\n��� ������",        // 14
    "������!!!\n��� ���� ������",                   // 15
    "������!!!\n������ ����� ���",                  // 16
    "������!!!\n�� ����������",                     // 17
    "������!!!\n�� ����������",                     // 18
    "������!!!\n�� ����������",                     // 19
    "������!!!\n�� ����������",                     // 20
    "������!!!\n�� ����������",                     // 21
    "������!!!\n�� ����������",                     // 22
    "������!!!\n�� ����������",                     // 23
    "������!!!\n�� ����������",                     // 24
    "������!!!\n�� ����������",                     // 25
    "������!!!\n�� ����������",                     // 26
    "������!!!\n�� ����������",                     // 27
    "������!!!\n�� ����������",                     // 28
    "������!!!\n�� ����������",                     // 29
    "������!!!\n�� ����������",                     // 30
    "������!!!\n�� ����������",                     // 31
    "������!!!\n������ ������\n��������� ����",     // 32
    "������!!!\n����� ������� ���",                 // 33
    "������!!!\n����������",                        // 34
    "������!!!\n����������\n�������������",         // 35
    "������!!!\n ������ \n���������",               // 36
    "������!!!\n������� ������",                    // 37
    "������!!!\n��������\n���������",               // 38
    "������!!!\n����������\n���� 1",                // 39
    "������!!!\n����������\n���� 2",                // 40
    "������!!!\n��������\n������� ������\nF_MAX",   // 41
    "������!!!\n��������\n������� ������\nF_MIN",   // 42
    "������!!!\n����� ���� U",                      // 43
    "������!!!\n����� ���� V",                      // 44
    "������!!!\n����� ���� W",                      // 45
    "������!!!\n��������\n���������",               // 46
    "������!!!\n������ ModBus",                     // 47
    "������!!!\n������ ��������",                   // 48
    "������!!!\n��������\n����������",              // 49
    "������!!!\n��� ����",                          // 50
    "������!!!\n ������ \n�����������",             // 51
    "������!!!\n������ \n����������� \n�������",    // 52
    "������!!!\n�������� \n���������",              // 53
    "������!!!\n������� \n�����",                   // 54
    "������!!!\n����� \n�����",                     // 55
    "������!!!\n���������� \n ������",              // 56
    "������!!!\n����� \n���",                       // 57
    "������!!!\n�������� \n����",                   // 58
    "������!!!\n��������� \n�����",                 // 59
    "������!!!\n����������\n���� 3",                // 60
    "������!!!\n����������\n���� 4",                // 61
    "������!!!\n����������\n���� 5",                // 62
    "������!!!\n������",                            // 63    
    "������!!!\n��������\n��������������",          // 64 - ��� ���. ���� ��
    "������!!!\n����������\n���������� 1",           // 65 - ��� ���. ���� ��    
    "������!!!\n����������\n���������� 2",           // 66 - ��� ���. ���� ��        
    "������!!!\n����������\n���������� 3",           // 67 - ��� ���. ���� ��        
    "������!!!\n����������\n���������� 4",           // 68 - ��� ���. ���� ��        
    "������!!!\n������� �����\n��������������",      // 69 - ��� ���. ���� ��        
};
const char * msgStringsTitle[] = 
{
    "������",
    "������",
    "�������",
    "���.",
    "����.",
    "���� ����������",
    "�������� �:",
    "������. ������",
    "��",
    "���",
    "���� - �����"

};
const char * msgStringsMess[] = 
{
    "Installation error!",
    "Connection error!",
    "Passwords are not available!",
    "���� ����!",
    "���� �����.\n������������?",
    "������ ������!",
    "������ �������� �������������!",
    "������ ������!",
    "������ �������:",
    "������ �������:",
    "Installation:",
    "��������� ���",
    "�������� ������������� � ��������!",
    "�������� �� �������������!",
    "��� �������!",
    "��� �������!",
    "������ �� ����������!",
    "������ �� ������ ��������!",
    "��������� ��������� �������?",
    "�� �������� ���������� ������ ��������",
    "�������� �� ���������� ��� ��� �������!",
    "No connect",
    "�� ������ �������������� ���������� ��!"
};
//===================================================================
//          ����������� ������ ����������
//===================================================================
extern u32 FindSpace(const char *str);
//===================================================================
//          ���������� ������ ����������
//===================================================================
extern u32 FindHyphen(const char *str);

// ���������� ����� ��������������� ������������� �������
const char * GetString(u16 IndexString)
{
    return (IndexString < COUNT_STRINGS) ? msgStrings[IndexString] : 0;
}

// ���������� ����� ������ ��������������� ������������� �������
const char * GetStringAlarm(u16 IndexString)
{
    return (IndexString < COUNT_FLASH) ? &MasFlash[IndexString][0] : 0;
}

// ���������� ����� ��������, ������, ������, ������� � �.�.
char * GetStringTitle(MessageTitul_type IndexString)
{
    return (IndexString < COUNT_TITLE) ? &MasTitle[IndexString][0] : 0;
}

// ���������� ����� ���������
const char * GetStringMess(u16 IndexString)
{
    return (IndexString < COUNT_MESS) ? &MasMess[IndexString][0] : 0;
}
/* Private macro -------------------------------------------------------------*/



/* Private types -------------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private functions ---------------------------------------------------------*/
// ������� ��������� ������������ ��������� 
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
// ������� ��������� ���������� ��������� 
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
// ������� ��������� ��������� 
MessageFlashErr_type MessageSetString(MessageFlashFlag_type Flag, u8 *String)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// ������� ��������� ������������ ��������� � �������
MessageFlashErr_type MessageSetInQueue(MessageFlashFlag_type Flag)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// ������� ��������� ��������� � �������
MessageFlashErr_type MessageSetInQueueString(MessageFlashFlag_type Flag, u8 *String)
{
    MessageFlashErr_type Err;
    Err = MessageFlashErr_OK;
    
    return Err;
}
// ������� ������ ���������
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
// ������� ������������� �������� ���������
MessageFlashErr_type MessageInit(MessageFlashFlag_type Flag, u8 *String, u8 Num)
{
    MessageFlashErr_type Err;
    
    Err = MessageFlashErr_OK;
    return Err;
}
// ������� �������� ���������� ���������
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

// ������� ����������� ���������� ���������
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

// ����� ��������� � �����
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

// ������� �������������
void ClearRectangle(u16 xPos, u16 yPos, u16 xPos2, u16 yPos2)
{
    u8      *cPnt;
    u8      xOfs, i, j;
    u8      xOfs2;
    u8      xStart, xStop;

    cPnt = &GrafBff[(xPos / 8) + yPos * (YPIXELS / 8)];    //���������� ��������� � ������ �� �������� �����������
    xOfs = xPos & 7;                                          //�������� � �����
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