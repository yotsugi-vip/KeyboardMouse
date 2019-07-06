/*******************************************************************************
 * File      : Main.c  
 * Author    : yotugi
 * Date      : 2019/06/28
 * CopyRight : All rights reserved by 
 *******************************************************************************/

/*******************************************************************************
 * Include Header
 *******************************************************************************/
#include"stdfx.h"

/*******************************************************************************
 * Global
 *******************************************************************************/
struct  input_event stInput;                /* Read Input Event Struct          [インプットイベント型]                 */
struct  input_event stExec;                 /* Write Exec Event Struct          [実行イベント用]                       */
int fd_key = 0;                             /* Keyboard device file descriptor  [キー入力ファイルディスクリプタ]       */
int fd_mouse = 0;                           /* Mouse device file descriptor     [マウス入力ファイルディスクリプタ]     */
int fd_virtual_device = 0;                  /* virtual devicr file descriptor   [仮想デバイスのファイルディスクリプタ] */
st_key_status keyStatus = { KEY_RELEASE };  /* Key Status                       [キーステータス]                       */
bool blLoop = FALSE;                        /* Main Loop Exec                   [メインループのループ許可]             */
st_fd_data fdInput;                         /* Input struct                     [入力ファイルディスクリプタの構造体]   */
/*******************************************************************************
 * ProtoType
 *******************************************************************************/
void getKeyEvent( void );
void testMouse( void );
void testKey( void );
void MouseEvent( void );
int openDevice( char*, int );
void virtualDeviceSetup( void );
void keyCtrl( int, int, int, int );
void closeDevices( void );

/*******************************************************************************
 * FuncName : main
 * Summary  : main function [メイン関数]
 * Argument : void
 * Return   : 0
 *******************************************************************************/
int main()
{
    //testMouse();
    //testKey();
    virtualDeviceSetup();
    getKeyEvent();
    closeDevices();
    return 0;
}

/*******************************************************************************
 * FuncName : getKeyEvent
 * Summary  : get key Event [キーイベント取得]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void getKeyEvent() 
{
    if( ( fd_key   = openDevice( "/dev/input/event3", O_RDWR ) ) == -1 ) return;
    if( ( fd_mouse = openDevice( "/dev/input/event9", O_RDWR ) ) == -1 ) return;
    if( ioctlSet(1) == -1 ) return;

    /* Main loop [メインループ] */
    while( blLoop == FALSE ) 
    {    
        /* Read Input key [入力キー読み込み] */
        if( read( fd_key, &stInput, sizeof(stInput) ) != sizeof(stInput) ) 
        {
            printf( "error key event can't read [errno:%d %s]\n", errno, strerror(errno) );
            continue;
        }

        switch( stInput.code ) 
        {
            case KEY_DOWN:
                keyStatus.down = stInput.value;
                break;
            case KEY_UP:
                keyStatus.up = stInput.value;
                break;
            case KEY_RIGHT:
                keyStatus.right = stInput.value;
                break;
            case KEY_LEFT:
                keyStatus.left = stInput.value;
                break;
            case KEY_ENTER:
                keyStatus.enter = stInput.value;
                break;
            case KEY_F12:
                keyStatus.f12 = stInput.value;
                break;
            case KEY_LEFTCTRL:
            case KEY_RIGHTCTRL:
                keyStatus.ctl = stInput.value;
                break;
            case KEY_LEFTSHIFT:
            case KEY_RIGHTSHIFT:
                keyStatus.shift = stInput.value;
                break;
            default:
                break;
        }
        MouseEvent();        
    }
    ioctlSet(0);
}

/*******************************************************************************
 * FuncName : MouseEvent
 * Summary  : Make Mouse Event [マウスイベント生成]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void MouseEvent()
{
    if( keyStatus.ctl != 0 && keyStatus.shift != 0 && keyStatus.f12 != 0 ) 
    {
        blLoop = TRUE;
    }
    
    if( keyStatus.ctl != 0 && keyStatus.shift != 0 && keyStatus.enter != 0 ) 
    {
        /*  Excute nter key press [エンター入力を実行] */
        keyCtrl( fd_virtual_device, EV_KEY, BTN_LEFT, KEY_PUT );

        /*  I don't know what's doing [よくわからん] */
        keyCtrl( fd_virtual_device, EV_SYN, 0, 0 );

        /* Excute Enter key release [エンターリリースを実行] */
        keyCtrl( fd_virtual_device, EV_KEY, BTN_LEFT, KEY_RELEASE );
    }

    for( int i = 2; i > 0; i-- ) 
    {
        keyCtrl( fd_virtual_device, EV_REL, CODE_Y, -keyStatus.up);
        keyCtrl( fd_virtual_device, EV_SYN, 0, 0 );
        keyCtrl( fd_virtual_device, EV_REL, CODE_Y, keyStatus.down);
        keyCtrl( fd_virtual_device, EV_SYN, 0, 0 );
        keyCtrl( fd_virtual_device, EV_REL, CODE_X, -keyStatus.left);
        keyCtrl( fd_virtual_device, EV_SYN, 0, 0 );
        keyCtrl( fd_virtual_device, EV_REL, CODE_X, keyStatus.right);
    }
}

/*******************************************************************************
 * FuncName : testMouse
 * Summary  : Mouse Debug [マウスデバック]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void testMouse() 
{
    
    if( ( fd_mouse = open( "/dev/input/event9", O_RDWR ) ) == -1 ) 
    {
        printf("error\n");
        return;
    }

    ioctl(fd_mouse, EVIOCGRAB, 1);

    for(;;) 
    {
        if( read( fd_mouse, &stInput, sizeof(stInput) ) == sizeof(stInput) )
        {
            printf("code:%02d type:%04d value:%04d\n", stInput.code, stInput.type, stInput.value );
        }
    }
}

/*******************************************************************************
 * FuncName : testKey
 * Summary  : Keyboard Debug [キーボードデバック]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void testKey() 
{
   
    if( ( fd_key = open("/dev/input/event3", O_RDWR ) ) == -1 ) return;

    for(;;) 
    {
        if( read( fd_key, &stInput, sizeof(stInput) ) == sizeof(stInput) ) 
        {
            printf("time:%08ld code:%04d type:%04d value:%04d\n",stInput.time.tv_sec, stInput.code, stInput.type, stInput.value );
        }
    }
}

/*******************************************************************************
 * FuncName                  : openDevice
 * Summary                   : Device file open                    [デバイスファイルオープン]
 * Argument [in] device_file : device file path                    [デバイスファイルパス]
 *          [in] option      : Second argument of open function    [openの第2引数]
 * Return   -1               : Fail                                [失敗]
 *          other than -1    : File descriptor　                   [ファイルディスクリプタの値]
 *******************************************************************************/
int openDevice( char* devive_file, int option ) 
{
   int fd = 0;

   if( ( fd = open( devive_file, option ) ) == -1 ) 
   {
       printf( "error %s can't open [errno:%d %s]\n", devive_file, errno, strerror(errno) );
   } else {
       printf( "success %s could open\n", devive_file );
   }

   return fd;
}

/*******************************************************************************
 * FuncName           : ioctlSet
 * Summary            : wrapped ioctl                    [ioctlのラッパー関数]  
 * Argument [in]arg   : Third argument of ioctl function [ioctlの第3引数]
 * Return   -1        : Fail                             [失敗]
 *           0        : Success                          [成功]
 *******************************************************************************/
int ioctlSet( int arg ) 
{
    if( fd_key == -1 )
    {
        printf("error key eventfile can't open\n");
        return -1;
    }

    /* Excute Key release [キーのリリースイベントを実行] */
    keyCtrl( fd_key, EV_KEY, KEY_ENTER, KEY_RELEASE);

    /* Write invalid data  [無効データ書き込み] */
    keyCtrl( fd_key, 0, 0, 0);

    if( ioctl( fd_key, EVIOCGRAB, arg ) == -1 ) 
    {
        printf( "erro ioctl [errno:%d %s]", errno, strerror(errno) );
        return -1;
    }

    return 0;
}

/*******************************************************************************
 * FuncName : virtualKeySetup
 * Summary  : Set up virtual device [仮想デバイスセットアップ]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void virtualDeviceSetup() 
{
    struct uinput_setup device;

    /* Open uinput device file  [デバイスファイルオープン] */
    if( ( fd_virtual_device = openDevice("/dev/uinput", O_RDWR) ) == -1 ) return;

    /* virtual device set up [デバイスセットアップ] */
    ioctl( fd_virtual_device, UI_SET_EVBIT, EV_KEY );
    ioctl( fd_virtual_device, UI_SET_KEYBIT, BTN_LEFT );

    ioctl( fd_virtual_device, UI_SET_EVBIT, EV_REL );
    ioctl( fd_virtual_device, UI_SET_RELBIT, REL_X );
    ioctl( fd_virtual_device, UI_SET_RELBIT, REL_Y );

    memset( &device, 0, sizeof(device) );
    strcpy( device.name, "virtual_mouse" );
    device.id.bustype = BUS_USB;
    device.id.vendor  = 0x000;
    device.id.product = 0x000;
    
    ioctl( fd_virtual_device, UI_DEV_SETUP, &device );
    ioctl( fd_virtual_device, UI_DEV_CREATE );
}

/*******************************************************************************
 * FuncName               : KeyCtrl
 * Summary                : Excute key event             [キーイベント書き込み]
 * Argument [in] fd       : file descriptor              [イベントファイルディスクリプタ]
 *          [in] type     : value of exec_event.type     [exec_event.type の値]
 *          [in] code     : value of exec_event.code     [exec_event.code の値]
 *          [in] value    : value of exec_event.value    [exec_event.value の値]
 * Return                 : void
 *******************************************************************************/
void keyCtrl( int fd, int type, int code, int value)
{
    gettimeofday( &stExec.time, NULL );
    stExec.type = type;
    stExec.code = code;
    stExec.value = value;

    if( write( fd, &stExec, sizeof(stExec) ) != sizeof(stExec) )
    {
        printf("error can't write [errno:%d %s]", errno, strerror(errno) );
    }
}

/*******************************************************************************
 * FuncName : closeDevices
 * Summary  : virtual device close [デバイスクローズ]
 * Argument : void
 * Return   : void
 *******************************************************************************/
void closeDevices()
{
    // destroy virtual device [仮想デバイス破棄]
    ioctl( fd_virtual_device, UI_DEV_DESTROY );

    /* close all device file [各デバイスファイルクローズ] */
    if( close( fd_key ) == -1 ) 
    {
        printf("error %s couldn't close [errno%d %s]\n", "key", errno, strerror(errno) );
       
    } else {
        printf("success %s could close\n", "fd_key" );
    }

    if( close( fd_mouse) == -1 ) 
    {
        printf("error %s couldn't close [errno%d %s]\n", "fd_mouse", errno, strerror(errno) );
    } else {
        printf("success %s could close\n", "fd_mouse" );
    }

    if( close( fd_virtual_device ) == -1 ) 
    {
        printf("error %s couldn't close [errno%d %s]\n", "fd_virtual_device", errno, strerror(errno) );
    } else {
        printf("success %s could close\n", "fd_virtual_device");        
    }
}
