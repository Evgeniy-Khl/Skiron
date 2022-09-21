#define PIC32         0
//-------- pic32.raw -----------
#define DOWNraw       0
#define MINUSMraw     1
#define PLUSraw       2
#define UPraw         3

#define PIC48         (UPraw+1)*4
//-------- pic48.raw -----------
#define BACKraw       0
#define CALENDERraw   1
#define CLOCKraw      2
#define CLOSEraw      3
#define FORWARDraw    4
#define HOMEraw       5
#define LISTraw       6
#define LOCKraw       7
#define PRINTERraw    8
#define QUESTIONraw   9
#define REPORTraw     10
#define TOOLSraw      11

#define PICTURE128    (TOOLSraw+1)*9 + PIC48
#define SECTOR        PICTURE128+64