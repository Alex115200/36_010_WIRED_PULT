/************************** (C) Copyright 2013 Triol ***************************
* File Name          : Message.h
* Author             : Kats Eyvhen
* TS Idea            : 2013.04.26
* TS Version         : 2013.04.26
* Description        : Function for work with message
*******************************************************************************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef MESSAGE_H
#define MESSAGE_H



/* Includes ------------------------------------------------------------------*/



/* Exported define -----------------------------------------------------------*/
// ���������� ���������
#define     COUNT_STRINGS           5
#define     COUNT_STRINGS_ALARM     70

#define     COUNT_FLASH             70
#define     LEN_FLASH               64
#define     COUNT_TITLE             11
#define     LEN_TITLE               16
#define     COUNT_MESS              23
#define     LEN_MESS                64

#define MAX_STR_MESSAGE_FLASH       16

#define MAX_STR_COUNT_MESSAGE_FLASH 4

#define ID_MAS_MESS                1
#define ID_MAS_TITLE               2
#define ID_MAS_FLASH               3

// ����� ���������
typedef enum{
    MessageFlashFlag_NONE,
    MessageFlashFlag_ALARM,
}MessageFlashFlag_type;

// ����� ��������� ���������
typedef enum{
    MessageTitul_STATUS,
    MessageTitul_MANUAL,
    MessageTitul_AUTO,
    MessageTitul_MIN,
    MessageTitul_MAX,
    MessageTitul_ENTER_PARAM,
    MessageTitul_PARAM,
    MessageTitul_EDIT,
    MessageTitul_YES,
    MessageTitul_NO,
    MessageTitul_ESC,
}MessageTitul_type;

// ����� ������� ���������
typedef enum{
    MessageMess_ERROR_SET,
    MessageMess_ERROR_CONECT,
    MessageMess_PASSWORD,
    MessageMess_FILE_EMPTY,
    MessageMess_FILE_FULL,
    MessageMess_ERROR_WRITE,
    MessageMess_ERROR_COMPATIBILITY,
    MessageMess_ERROR_READ,
    MessageMess_READ_SET,
    MessageMess_WRITE_SET,
    MessageMess_SETUP,
    MessageMess_NO_HELP,
    MessageMess_PARAM_EDIT_STOP,
    MessageMess_PARAM_NOT_EDIT,
    MessageMess_NO_ACCESS,
    MessageMess_NO_JORNAL,
    MessageMess_DRIVE_NO_SETUP,
    MessageMess_DRIVE_NO_TEST,
    MessageMess_REPID_SETUP_DRIVE,
    MessageMess_NO_MORE_DRIVE,
    MessageMess_PARAM_NO_EDIT_OR_NO_EXIST,
    MessageMess_NO_CONECT,
    MessageMess_ERROR_FUNCT,
}MessageMess_type;

// ����� ���������
typedef enum{
    MessageFlashErr_OK,
    MessageFlashErr_ERR,
}MessageFlashErr_type;

// ��������� ��� ������ � ����������
typedef struct{
    u16                     TimeStart;
    u16                     TimeStop;
    char const              * MessageString;
    MessageFlashFlag_type   MessageFlag;
}MessageFlash_type;

/* Exported macro ------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/



/* Exported variables --------------------------------------------------------*/



/* Exported function prototypes ----------------------------------------------*/
// ������� ��������� ������������ ��������� 
MessageFlashErr_type MessageSet(MessageFlashFlag_type Flag, u16 Time);
// ������� ��������� ���������� ��������� 
MessageFlashErr_type MessageSetAlarm(MessageFlashFlag_type Flag, u16 Time);
// ������� ��������� ��������� 
MessageFlashErr_type MessageSetString(MessageFlashFlag_type Flag, u8 *String);
// ������� ��������� ������������ ��������� � �������
MessageFlashErr_type MessageSetInQueue(MessageFlashFlag_type Flag);
// ������� ��������� ��������� � �������
MessageFlashErr_type MessageSetInQueueString(MessageFlashFlag_type Flag, u8 *String);
// ������� ������ ���������
MessageFlashErr_type MessageShow(keyName_type Key);
// ������� ������������� �������� ���������
MessageFlashErr_type MessageInit(MessageFlashFlag_type Flag, u8 *String, u8 Num);
// ������� �������� ���������� ���������
MessageFlashErr_type MessageClear(u8 Num);
// ������� ����������� ���������� ���������
MessageFlashErr_type MessageActiv(void);
// ����� ��������� � �����
u16 Format_MessageFlash(const char *const Msg);
// ������� �������������
void ClearRectangle(u16 xPos, u16 yPos, u16 xPos2, u16 yPos2);
// ���������� ����� ������ ��������������� ������������� �������
const char * GetStringAlarm(u16 IndexString);
// ���������� ����� ��������, ������, ������, ������� � �.�.
char * GetStringTitle(MessageTitul_type IndexString);
// ���������� ����� ���������
const char * GetStringMess(u16 IndexString);


/* Exported functions --------------------------------------------------------*/



#endif // MESSAGE_H

/************************** (C) Copyright 2013 Triol **************************/