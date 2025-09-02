#include "oled.h"
#include "GUI.h"
#include "font.h"


//m^n函数
uint32_t  oled_pow(uint8_t m,uint8_t n)
{
  uint32_t result=1;	 
  while(n--)result*=m;    
  return result;
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void  OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode)
{      	
  unsigned char c=0,i=0;
  c=chr-' ';//得到偏移后的值			
  if(x>Max_Column-1){x=0;y=y+2;}
  if(SIZE ==16)
  {
    if(mode)//反白
    {
    OLED_Set_Pos(x,y);	
    for(i=0;i<8;i++)
      OLED_WR_Byte(~F8X16[c*16+i],OLED_DATA);
    OLED_Set_Pos(x,y+1);
    for(i=0;i<8;i++)
      OLED_WR_Byte(~F8X16[c*16+i+8],OLED_DATA);
    }
    else
    {
      OLED_Set_Pos(x,y);	
      for(i=0;i<8;i++)
        OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
      OLED_Set_Pos(x,y+1);
      for(i=0;i<8;i++)
        OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
    } 
  }
  else {	
    OLED_Set_Pos(x,y+1);
    for(i=0;i<6;i++)
      OLED_WR_Byte(~F6x8[c][i],OLED_DATA);
    
  }
}

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void  OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size2)
{         	
  uint8_t t,temp;
  uint8_t enshow=0;						   
  for(t=0;t<len;t++)
  {
    temp=(num/oled_pow(10,len-t-1))%10;
    if(enshow==0&&t<(len-1))
    {
      if(temp==0)
      {
        OLED_ShowChar(x+(size2/2)*t,y,' ',0);
        continue;
      }else enshow=1; 
      
    }
    OLED_ShowChar(x+(size2/2)*t,y,temp+'0',0); 
  }
} 
//显示一个字符号串

//******************************************************************
//函数名：  GUI_DrawFont16
//功能：    显示单个16X16中文字体
//输入参数：x,y :起点坐标
//			fc:前置画笔颜色
//			bc:背景颜色	 
//			s:字符串地址
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************
void GUI_DrawFont16(uint16_t x, uint16_t y, uint8_t *s,uint8_t mode)
{
  // uint8_t i,j;
  uint16_t k;
  uint16_t HZnum;
  // u16 x0=x;
  HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目
  for (k=0;k<HZnum;k++) 
  {
    if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
    { 	    
      uint8_t t;
      
      if(!mode) 
      {
        OLED_Set_Pos(x,y);	
        for(t=0;t<16;t++)
        {
          OLED_WR_Byte(tfont16[k].Msk[t],OLED_DATA);
          
        }	
        OLED_Set_Pos(x,y+1);	
        for(t=0;t<16;t++)
        {	
          OLED_WR_Byte(tfont16[k].Msk[t+16],OLED_DATA);
        }
      }
      else//反白
      {
        OLED_Set_Pos(x,y);	
        for(t=0;t<16;t++)
        {
          OLED_WR_Byte(~tfont16[k].Msk[t],OLED_DATA);
          
        }	
        OLED_Set_Pos(x,y+1);	
        for(t=0;t<16;t++)
        {	
          OLED_WR_Byte(~tfont16[k].Msk[t+16],OLED_DATA);
        }
        
      }
      
      continue;
    }
    //continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
  }
} 

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
  unsigned int j=0;
  unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
  for(y=y0;y<y1;y++)
  {
    OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
    {      
      OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
    }
  }
} 

//******************************************************************
//函数名：  Show_Str
//功能：    显示一个字符串,包含中英文显示
//输入参数：x,y :起点坐标
// 			fc:前置画笔颜色
//			bc:背景颜色
//			str :字符串	 
//			size:字体大小
//			mode:模式	0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************    	   		   

void  Show_Str(uint16_t x, uint16_t y,  uint8_t *str,uint8_t size,uint8_t mode)
{					
  uint16_t x0=x;							  	  
  uint8_t bHz=0;     //字符或者中文 
  while(*str!=0)//数据未结束
  { 
    if(!bHz)
    {
      if(x>128) 
        return; 
      if(*str>0x80)bHz=1;//中文 
      else              //字符
      {          
        if(*str==0x0D)//换行符号
        {         
          y+=size;
          x=x0;
          str++; 
        }  
        else
        {
          OLED_ShowChar(x,y,*str,mode);
          x+=size/2;; //字符,为全字的一半  
        } 
        str++; 
      }
    }else//中文 
    {   
      if(x>128) 
        return;  
      bHz=0;//有汉字库    
      GUI_DrawFont16(x,y,str,mode);
      
      str+=2; 
      x+=size;//下一个汉字偏移	    
    }						 
  }   
}





