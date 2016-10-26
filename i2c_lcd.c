
#include <xc.h>
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h> 
#include "mcc.h"

static unsigned char _functionset = 0;
static unsigned char _entrymodeset = 0;
static unsigned char _displayfunction = 0;
static unsigned char _displaycontrol = 0;
static unsigned char _numlines = 0;
static unsigned char _backlightval = 0;


// Local function declarations

static void LCDsend(unsigned char value, unsigned char mode);
static unsigned char LCDreceive(unsigned char RsMode);
static void LCDwrite4bits(unsigned char value);
static unsigned char LCDread4bits(unsigned char RsEnMode);
static void LCDpulseEnableNeg(unsigned char value);
static void LCDpulseEnablePos(unsigned char value);
static int LCDwritePCF8574(unsigned char value);
static unsigned char LCDreadPCF8574(void);


void LCD_Init(void){
    _backlightval |= Bl; // On at start up
    _numlines = LCD_MAX_ROWS;
    I2C_MESSAGE_STATUS status;
    // Ensure supply rails are up before config sequence
    __delay_ms(50);
    uint8_t   writeBuffer[1];
    
    writeBuffer[0] = LCD_INIT;
    // Set all control and data lines low. D4 - D7, En (High=1), Rw (Low = 0 or Write), Rs (Control/Instruction) (Low = 0 or Control)
    I2C_MasterWrite(writeBuffer,1,LCD_I2C_ADDR,&status); // Backlight off (Bit 3 = 0)
    //I2C_MasterWrite(writeBuffer,1,LCD_ADDRESS,&status);
    __delay_ms(20);

    // Sequence to put the LCD into 4 bit mode this is according to the hitachi HD44780 datasheet page 109

    // we start in 8bit mode
    LCDwrite4bits(LCD_8BIT_INIT);
    __delay_ms(10);  // wait more than 4.1ms

    // second write
    LCDwrite4bits(LCD_8BIT_INIT);
    __delay_us(500); // wait > 100us

    // third write
    LCDwrite4bits(LCD_8BIT_INIT);
    __delay_us(500);

    // now set to 4-bit interface
    LCDwrite4bits(LCD_4BIT_INIT);
    __delay_us(500);

    // set # lines, font size, etc.
    _functionset = LCD_INTF4BITS | LCD_TWO_LINES | LCD_FONT_5_7;
    LCDcommandWrite(LCD_FUNCTION_SET | _functionset);
    //DelayMicroseconds(150);

    _displayfunction = LCD_DISPLAY_OFF | LCD_CURSOR_OFF | LCD_BLINKING_OFF;
    _displayfunction &= ~LCD_DISPLAY_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
    LCDdisplayOff();

    // turn the display on with no cursor or blinking default
    LCDdisplayOn();

    // set the entry mode
    _entrymodeset = LCD_INCREMENT | LCD_SHIFT_OFF; // Initialize to default text direction (for roman languages)
    LCDcommandWrite(LCD_ENTRY_MODE_SET | _entrymodeset);

    // Display Function set
    // _displayfunction = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINKING_OFF;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);

    // Display Control set
    _displaycontrol = LCD_DISPLAY_SHIFT | LCD_SHIFT_LEFT;
    LCDcommandWrite(LCD_MV_CUR_SHIFT_DISPLAY | _displaycontrol);

    // clear display and return cursor to home position. (Address 0)
    LCDclear();
}


/********** high level commands, for the user! */
void LCD_Write_Char(char message)
{
    LCDdataWrite((unsigned char) message);
}


void LCD_Write_Str(const char *message)
{
    unsigned char *message_ptr = (unsigned char *) message;

    while (*message_ptr)
        LCDdataWrite((unsigned char) (*message_ptr++));
}


void LCDclear(void){
    LCDcommandWrite(LCD_CLEAR_DISPLAY);// clear display, set cursor position to zero
    while (LCDbusy()){};
}

void LCDhome(void){
    LCDcommandWrite(LCD_RETURN_HOME);  // set cursor position to zero
//#ifdef USE_BUSY_FLAG
    while (LCDbusy()){};
//#else
  //  DelayMicroseconds(30000);  // this command takes a long time!
//#endif
}

void LCDsetCursor(unsigned char col, unsigned char row)
{
  int row_offsets[] = { LCD_LINE1, LCD_LINE2};
  if ( row >= _numlines ) {
    row = _numlines-1;    // we count rows starting w/0
  }

  LCDcommandWrite(LCD_DD_RAM_ADDRESS | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LCDdisplayOff(void) {
    _displayfunction &= ~LCD_DISPLAY_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}

void LCDdisplayOn(void) {
    _displayfunction |= LCD_DISPLAY_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}

// Turns the underline cursor on/off
void LCDcursorOff(void) {
    _displayfunction &= ~LCD_CURSOR_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}

void LCDcursorOn(void) {
    _displayfunction |= LCD_CURSOR_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}

// Turn on and off the blinking cursor
void LCDblinkOff(void) {
    _displayfunction &= ~LCD_BLINKING_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}

void LCDblinkOn(void) {
    _displayfunction |= LCD_BLINKING_ON;
    LCDcommandWrite(LCD_DISPLAY_ON_OFF | _displayfunction);
}
// These commands scroll the display without changing the RAM
void LCDscrollDisplayLeft(void) {
    _displaycontrol &=  ~LCD_SHIFT_RIGHT;
    _displaycontrol |=   LCD_DISPLAY_SHIFT;
    LCDcommandWrite(LCD_MV_CUR_SHIFT_DISPLAY | _displaycontrol);
}

void LCDscrollDisplayRight(void) {
    _displaycontrol |=  LCD_SHIFT_RIGHT;
    _displaycontrol |=  LCD_DISPLAY_SHIFT;
    LCDcommandWrite(LCD_MV_CUR_SHIFT_DISPLAY | _displaycontrol);
}


// This is for text that flows Left to Right
void LCDleftToRight(void) {
    _entrymodeset |= LCD_INCREMENT;
    //_entrymodeset |= LCD_SHIFT_ON;
    LCDcommandWrite(LCD_ENTRY_MODE_SET | _entrymodeset);
}

// This is for text that flows Right to Left
void LCDrightToLeft(void) {
    _entrymodeset &= ~LCD_INCREMENT;
    //_entrymodeset &= ~LCD_SHIFT_ON;
    LCDcommandWrite(LCD_ENTRY_MODE_SET | _entrymodeset);
}

// This will 'right justify' text from the cursor. Display shift
void LCDautoscroll(void) {
    _entrymodeset |= LCD_SHIFT_ON;
    //_entrymodeset |= LCD_INCREMENT;
    LCDcommandWrite(LCD_ENTRY_MODE_SET | _entrymodeset);
}

// This will 'left justify' text from the cursor. Cursor Move
void LCDnoAutoscroll(void) {
    _entrymodeset &= ~LCD_SHIFT_ON;
    //_entrymodeset &= ~LCD_INCREMENT;
    LCDcommandWrite(LCD_ENTRY_MODE_SET | _entrymodeset);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDcreateChar(unsigned char location, unsigned char charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    LCDcommandWrite(LCD_CG_RAM_ADDRESS | (location << 3));
    for (int i=0; i<8; i++)
        LCDdataWrite(charmap[i]);
}

// Turn the (optional) backlight off/on
void LCDnoBacklight(void) { 
    _backlightval &= ~Bl;
    LCDwritePCF8574(LCDreadPCF8574());  // Dummy write to LCD, only led control bit is of interest
}

void LCDbacklight(void) { 
    _backlightval |= Bl;
    LCDwritePCF8574(LCDreadPCF8574());  // Dummy write to LCD, only led control bit is of interest
}

/*********** mid level commands, for sending data/cmds */

inline void LCDcommandWrite(unsigned char value) {
    LCDsend(value, Rs & ~Rs);
}

inline unsigned char LCDcommandRead(void) {
    return LCDreceive(Rs & ~Rs);
}

inline void LCDdataWrite(unsigned char value) {
    LCDsend(value, Rs);
}

inline unsigned char LCDdataRead(void) {
    return LCDreceive(Rs);
}

unsigned char LCDbusy(void) {
    return LCDcommandRead() & LCD_BUSY_FLAG_MASK;
}

unsigned char LCDaddressCounter(void) {
    return LCDcommandRead() & LCD_ADDRESS_COUNTER_MASK;
}

unsigned char LCDreadDDRam(unsigned char address)
{
    LCDcommandWrite(LCD_DD_RAM_ADDRESS | (address & LCD_DD_RAM_ADDRESS_MASK));
    return LCDdataRead();
}

unsigned char LCDreadCGRam(unsigned char address)
{
    LCDcommandWrite(LCD_CG_RAM_ADDRESS | (address & LCD_CG_RAM_ADDRESS_MASK));
    return LCDdataRead();
}

/************ low level data write commands **********/

// write either command or data
static void LCDsend(unsigned char value, unsigned char RsMode) {
    unsigned char highnib = value & 0xF0;

    unsigned char lownib  = value << 4;
    lownib &= 0xF0;

    LCDwrite4bits((highnib) | En | RsMode);
    LCDwrite4bits((lownib ) | En | RsMode);
}

// Change this routine for your I2C to 16 pin parallel interface, if your pin interconnects are different to that outlined above // TODO Adapt

// read either command or data
static unsigned char LCDreceive(unsigned char RsMode) {
    unsigned char highnib;
    unsigned char lownib;

    LCDwritePCF8574(LCD_PCF8574_WEAK_PU | (En & ~En) | RsMode); // Set P7..P4 = 1, En = 0, RnW = 0, Rs = XX
    highnib = LCDread4bits(LCD_PCF8574_WEAK_PU | En | RsMode);
    lownib = LCDread4bits(LCD_PCF8574_WEAK_PU | En | RsMode);
    LCDwritePCF8574((LCD_PCF8574_WEAK_PU & ~LCD_PCF8574_WEAK_PU) | En | RsMode); // Set P7..P4 = 1, En = 1, RnW = 0, Rs = XX
    return (unsigned char) ((highnib & 0xF0) | ((lownib & 0xF0) >> 4));
}



static void LCDwrite4bits(unsigned char nibEnRsMode) {
    LCDwritePCF8574(nibEnRsMode & ~Rw);
    LCDpulseEnableNeg(nibEnRsMode & ~Rw);
}


static unsigned char LCDread4bits(unsigned char RsEnMode) {
    unsigned char b;
    LCDpulseEnablePos(RsEnMode | Rw);
    b = LCDreadPCF8574(); // Read the data from the LCD just after the rising edge. NOT WELL DOCUMENTED!
    LCDpulseEnableNeg(RsEnMode | Rw);
    return b;
}

static void LCDpulseEnableNeg(unsigned char _data){
    LCDwritePCF8574(_data | En);	// En high
    __delay_us(1);		// enable pulse must be >450ns

    LCDwritePCF8574(_data & ~En);	// En low
    __delay_us(50);		// commands need > 37us to settle
}

static void LCDpulseEnablePos(unsigned char _data){
    LCDwritePCF8574(_data & ~En);	// En low
    __delay_us(1);		// enable pulse must be >450ns

    LCDwritePCF8574(_data | En);	// En high
    __delay_us(50);		// commands need > 37us to settle
}


static int LCDwritePCF8574(unsigned char value) {
    I2C_MESSAGE_STATUS status = I2C_MESSAGE_PENDING;
    uint16_t timeOut = 0;
    uint8_t   writeBuffer[1];
    
    writeBuffer[0] = value | _backlightval;
    while(status != I2C_MESSAGE_FAIL)
    {
        I2C_MasterWrite(writeBuffer,1,LCD_I2C_ADDR,&status);
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE){
            return 1;
        }
        
        if (timeOut == LCD_RETRY_MAX)
            break;
        else
            timeOut++;
        
    }
    if (status == I2C_MESSAGE_FAIL)
    {
        return 0;
    }
    return 1;
}


static unsigned char LCDreadPCF8574(void){
    uint8_t     *pD;
    I2C_MESSAGE_STATUS status;
    uint16_t timeOut = 0;
       
    while(status != I2C_MESSAGE_FAIL)
    {
        // write one byte to EEPROM (2 is the count of bytes to write)
        I2C_MasterRead(pD,1,LCD_I2C_ADDR,&status);

        // wait for the message to be sent or status has changed.
        while(status == I2C_MESSAGE_PENDING);

        if (status == I2C_MESSAGE_COMPLETE)
            break;

        // if status is  I2C_MESSAGE_ADDRESS_NO_ACK,
        //               or I2C_DATA_NO_ACK,
        // The device may be busy and needs more time for the last
        // write so we can retry writing the data, this is why we
        // use a while loop here

        // check for max retry and skip this byte
        if (timeOut == LCD_RETRY_MAX)
            break;
        else
            timeOut++;
    }
    if (status == I2C_MESSAGE_COMPLETE)
        return pD[0];
    else
        return 0xFF;
}