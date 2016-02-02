#include  <msp430x14x.h>

#define  LCD_RS_High   P6OUT|=0x04   // LCD_RS  高电平   显示数据         P62
#define  LCD_RS_Low    P6OUT&=0xfb   // LCD_RS  低电平   指令数据   


 
#define  LCD_RW_High   P6OUT|=0x10   // LCD_RW  高电平   数据读到DB0_DB7   P64
#define  LCD_RW_Low    P6OUT&=0xef   // LCD_RW  低电平   DB0_DB7 数据写入IR DR


#define  LCD_EN_High   P6OUT|=0x08   // LCD_EN  高电平   高有效            P63
#define  LCD_EN_Low    P6OUT&=0xf7   // LCD_EN  低电平   使能信号

#define LCD_PSB_High   P3OUT|=0x01;  //  并口   高电平
#define LCD_PSB_Low    P3OUT&=0xfe;  //  串口   低电平    P30

//P67 悬空

void delay_ms(unsigned int timer)
{
  unsigned int i;
    for(i=0;i<1000;i--)
       for(;timer>0;timer--)
         ;
}


unsigned char lcd_busy()
 {                          
    unsigned char result;
    
     P4DIR=0x00;    
     LCD_RS_Low;          //  指令数据
     LCD_RW_High;
     LCD_EN_High;
        

     result = P4IN&0x80;   // 读取数据口数据
     LCD_EN_Low; 
     
     return(result); 
 }
/*写命令到液晶*/
void LCD_WCMD(unsigned char cmd)
{                          
  while(lcd_busy());

     P4DIR=0xff;          // OUT;
     LCD_RS_Low;          //  指令数据
     LCD_RW_Low;
  //   LCD_EN_Low; 
     LCD_EN_High;
     P4OUT = cmd;
    

     LCD_EN_Low; 
}
/*写数据到液晶*/
void LCD_WDAT(unsigned char dat)
{                          
   while(lcd_busy());
   
     P4DIR=0xff;           // OUT;
     LCD_RS_High;          //  数据
     LCD_RW_Low;
  //   LCD_EN_Low; 
     
     LCD_EN_High;
     P4OUT = dat;
     
    
     LCD_EN_Low; 
}

/*显示的字符串*/
void Display(unsigned char X,unsigned char Y,unsigned char *string)
{

   if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}

     LCD_WCMD(X+Y);
   
   while(*string )
    {                         //显示字符
      LCD_WDAT(*string++);
    }

}
void Display_number(unsigned char X,unsigned char Y,unsigned char number)
{
    if (X==0)
     {X=0x80;}
   else if (X==1)
     {X=0x90;}
   else if (X==2)
     {X=0x88;}
   else if (X==3)
     {X=0x98;}

     LCD_WCMD(X+Y);
     LCD_WDAT(48+number);
}
/*时钟初始化*/
void Clock_init()
{
   unsigned char t;
   BCSCTL1 = RSEL0 + RSEL1 + RSEL2;         // XT2on, max RSEL
   BCSCTL1&=~XT2OFF;                        //打开XT2振荡器
   do
   {
      IFG1 &= ~OFIFG;                       // 清除振荡器失效标志
      for (t = 255; t > 0; t--);            // 延时，等待XT2起振
   }
   while ((IFG1 & OFIFG) != 0);             // 判断XT2是否起振
   
   BCSCTL2 |= SELS+SELM_2;                  // SMCLK = XT2
}
/*液晶初始化*/
void LCD_init()
{
   LCD_WCMD(0x34);      //扩充指令操作
   LCD_WCMD(0x30);      //基本指令操作
   LCD_WCMD(0x0C);      //显示开，关光标
   LCD_WCMD(0x01);      //清除LCD的显示内容
      
}
void  main( void )
{
 
  WDTCTL = WDTPW + WDTHOLD;     // Stop watchdog timer to prevent time out reset
  Clock_init();                 // 时钟初始化

  P6DIR=0x1c;
  P3DIR=0x01;
  LCD_PSB_High;                // 液晶显示  并口方式  
  
  LCD_init();

  Display(0,0,"电子园");
  Display(1,0,"www.cepark.com");
  Display(2,1,"2011年11月12日");
  Display(3,5,"numen");        // 第四行要显示的内容
  _BIS_SR(LPM0_bits); 
 
}
