/*******************************************************************************
 * File      : type.h
 * Author    : yotugi
 * Date      : 2019/06/28
 * CopyRight : All rights reserved
*******************************************************************************/
#ifndef TYPE_H
#define TYPE_H
 /********************************************************************************
 * define
 ********************************************************************************/
#define TRUE        1
#define FALSE       !TRUE

#define KEY_RELEASE     0
#define KEY_PUT         1
#define KEY_HOLD        2

#define CODE_X      0
#define CODE_Y      1

#define MAX_PATH 1024
#define CONFIG_FILE "KeybordMouse.config"
/********************************************************************************
 * structure
 ********************************************************************************/
typedef struct ST_KEY_STATUS 
{
    unsigned char left;
    unsigned char right;
    unsigned char up;
    unsigned char down;
    unsigned char ctl;
    unsigned char enter;
    unsigned char shift;
    unsigned char f12;
} st_key_status;

typedef struct ST_FD_DATA 
{
    char eventFile[1024];
    int fd;
} st_fd_data;

typedef struct ST_CONFIG_FILE
{
    char deviceFile[MAX_PATH];
} st_config_file;

/**************************************************************************************
 * other
 ***************************************************************************************/
typedef enum EVENT_NUM {
    event0 = 0,
    event1,
    event2,
    event3,
    event4,
    event5,
    event6,
    event7,
    event8,
    event9,
    event10,
    event11,
    event12,
    event13,
    event14,
    event15,
    event16,
    event17,
    event18,
    event19, 
} event_num;
                                
typedef unsigned char bool;

int ioctlSet( int );
#endif /* TYPE_H */
