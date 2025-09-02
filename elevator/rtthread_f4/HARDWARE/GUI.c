#include "oled.h"
#include "GUI.h"
#include "font.h"


//m^n����
uint32_t  oled_pow(uint8_t m,uint8_t n)
{
  uint32_t result=1;	 
  while(n--)result*=m;    
  return result;
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void  OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode)
{      	
  unsigned char c=0,i=0;
  c=chr-' ';//�õ�ƫ�ƺ��ֵ			
  if(x>Max_Column-1){x=0;y=y+2;}
  if(SIZE ==16)
  {
    if(mode)//����
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

//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
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
//��ʾһ���ַ��Ŵ�

//******************************************************************
//��������  GUI_DrawFont16
//���ܣ�    ��ʾ����16X16��������
//���������x,y :�������
//			fc:ǰ�û�����ɫ
//			bc:������ɫ	 
//			s:�ַ�����ַ
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************
void GUI_DrawFont16(uint16_t x, uint16_t y, uint8_t *s,uint8_t mode)
{
  // uint8_t i,j;
  uint16_t k;
  uint16_t HZnum;
  // u16 x0=x;
  HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//�Զ�ͳ�ƺ�����Ŀ
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
      else//����
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
    //continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
  }
} 

/***********������������ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7*****************/
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
//��������  Show_Str
//���ܣ�    ��ʾһ���ַ���,������Ӣ����ʾ
//���������x,y :�������
// 			fc:ǰ�û�����ɫ
//			bc:������ɫ
//			str :�ַ���	 
//			size:�����С
//			mode:ģʽ	0,���ģʽ;1,����ģʽ
//����ֵ��  ��
//�޸ļ�¼����
//******************************************************************    	   		   

void  Show_Str(uint16_t x, uint16_t y,  uint8_t *str,uint8_t size,uint8_t mode)
{					
  uint16_t x0=x;							  	  
  uint8_t bHz=0;     //�ַ��������� 
  while(*str!=0)//����δ����
  { 
    if(!bHz)
    {
      if(x>128) 
        return; 
      if(*str>0x80)bHz=1;//���� 
      else              //�ַ�
      {          
        if(*str==0x0D)//���з���
        {         
          y+=size;
          x=x0;
          str++; 
        }  
        else
        {
          OLED_ShowChar(x,y,*str,mode);
          x+=size/2;; //�ַ�,Ϊȫ�ֵ�һ��  
        } 
        str++; 
      }
    }else//���� 
    {   
      if(x>128) 
        return;  
      bHz=0;//�к��ֿ�    
      GUI_DrawFont16(x,y,str,mode);
      
      str+=2; 
      x+=size;//��һ������ƫ��	    
    }						 
  }   
}





