#include "oled_bsp.h"
#include "stdlib.h"
#include "oledfont_bsp.h" 
#include "stdint.h"
#include "string.h"
#include "stdio.h"

uint8_t HSI2C_switch=0;
//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   
u8 OLED_GRAM[8][128];

void IIC_Start()
{
	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
//	delay_ms(1);
	OLED_SDIN_Clr();
//	delay_ms(1);
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{

	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
		m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
	}


}


//Ӳ��iic
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//��ʼ�� IIC_GPIO ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	//��ʼ��IIC_SCL
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//��ʼ��IIC_SDA	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
void I2C_Config(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_GPIO_Config();//����SDA��SCL GPIO
	//��ʼ��IIC����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	
	//��ʼ��IIC �ṹ��
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;			    //ʹ��Ӧ��
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//IIC 7 λѰַ
	I2C_InitStruct.I2C_ClockSpeed = 400000;//ͨ������ 400k
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;		//ռ�ձ� 1/2
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			  //ѡ��IICģʽ
	I2C_InitStruct.I2C_OwnAddress1 = 0x01;//����������ַ����ӻ��������𼴿�����ͨ��
	I2C_AcknowledgeConfig(I2C2, DISABLE );
	I2C_Init(I2C2,&I2C_InitStruct);
	//ʹ��IIC
	I2C_Cmd(I2C2, ENABLE);
}

void HI2C_WriteByte(uint8_t data)
{
	I2C_SendData(I2C2, data); //����ҳ��ַ
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) )
	{

	} //�ȴ��¼�EV8
}

void HI2C_Start()
{
	// uint8_t i=5;
	while( I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) )
	{

	} //�ȴ����߿���

	I2C_GenerateSTART(I2C2, ENABLE); //I2C������ʼ�ź�
	
}

void HI2C_SendAddress(uint8_t addr)
{
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) )
	{

	} //�ȴ�EV5=1,������Բο��ֲ�����EV5�¼�
	I2C_Send7bitAddress(I2C2, addr, I2C_Direction_Transmitter); //���ʹӻ�7λ��ַ
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) )
	{

	} //�ȴ�EV6=1
}

void HI2C_Stop()
{
	uint16_t i=1000;
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) )
	{
		i--;
		if(i==0)break;
	} 
	I2C_GenerateSTOP(I2C2, ENABLE); //I2C����ֹͣ�ź�
	
}
//��OLED�Ĵ�����ַдһ��byte������
void HI2C_WriteData(uint8_t dat,uint8_t cmd)
{
	while( I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) ); //�ȴ����߿���
	I2C_GenerateSTART(I2C2, ENABLE); //I2C������ʼ�ź�
	
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) ); //�ȴ�EV5=1,������Բο��ֲ�����EV5�¼�
	I2C_Send7bitAddress(I2C2, 0x78, I2C_Direction_Transmitter); //���ʹӻ�7λ��ַ
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ); //�ȴ�EV6=1
	
	//while( I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //�ȴ��¼�EV8_1
	I2C_SendData(I2C2, cmd); //���͵�ַ�ֽ�
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //�ȴ��¼�EV8
	
	I2C_SendData(I2C2, dat); //���������ֽ�
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //�ȴ��¼�EV8

	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) ); //�ȴ��¼�EV8_2	
	I2C_GenerateSTOP(I2C2, ENABLE); //I2C����ֹͣ�ź�

}

//�����Դ浽LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[i][n],OLED_DATA); 
	}   
}
#if OLED_MODE==1
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();		   
	OLED_CS_Clr();
	OLED_WR_Clr();	 
	OLED_WR_Set();
	OLED_CS_Set();	  
	OLED_RS_Set();	 
} 	    	    
#else
//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
    if(HSI2C_switch)
    {
        //Ӳ��iic
        if(cmd)
        {
            HI2C_WriteData(dat,0x40);
        }
        else
        {
            HI2C_WriteData(dat,0x00);
        }
    }
    else
    {
        //���iic
        IIC_Start();
        Write_IIC_Byte(0x78);
        IIC_Wait_Ack();		  
        if(cmd)//д����
        {
            //OLED_RS_Set(); 	
            Write_IIC_Byte(0x40);			//write data
		    IIC_Wait_Ack();	
        }
        else//д����
        {
            Write_IIC_Byte(0x00);			//write cmd
		    IIC_Wait_Ack();	
        }
        // for(i=0;i<8;i++)
        // {			  
        //     OLED_SCLK_Clr();
        //     if(dat&0x80)
        //     OLED_SDIN_Set();
        //     else 
        //     OLED_SDIN_Clr();
        //     OLED_SCLK_Set();
        //     dat<<=1;   
        // }				 		  
        // OLED_CS_Set();
        // OLED_RS_Set();
        Write_IIC_Byte(dat);
		IIC_Wait_Ack();	
		IIC_Stop();   	
    }
	  
} 
#endif
	  	  
//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[i][n]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)OLED_GRAM[pos][x]|=temp;
	else OLED_GRAM[pos][x]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //����1206����
		else temp=oled_asc2_1608[chr][t];		 //����1608���� 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//*p:�ַ�����ʼ��ַ
//��16����
void OLED_ShowString(u8 x,u8 y,const u8 *p,uint8_t size, uint8_t mode)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=12;}
        //if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,mode);	
		if(size==12)x+=6;
		else x+=8;
        p++;
    }  
}
void OLED_ShowFloat(uint8_t x,uint8_t y,float data,uint8_t precision,u8 mode)
{
	if(precision>4)precision=4;
	uint32_t temp=(uint32_t)abs(data)*10000;
	char ch[20];
	uint8_t p=0;
	uint8_t d_flag=0;
	snprintf(ch,sizeof(ch),"%.4f",data);

	for(uint8_t i=0;ch[i]!='\0';i++)
	{
		if(d_flag)
		{
			p++;
			if(p>precision)
			{
				break;
			}
		}
		if(ch[i]=='.')
		{
			d_flag=1;
		}
		OLED_ShowChar(x,y,ch[i],12,mode);
		x+=6;
	}

}

//��ʾͼƬ
//x,y:�������  
//w:ͼƬ���ؿ��
//h:ͼƬ���ظ߶�
//*img:ͼƬ
//ͼƬ��ʽ����λ���ϣ����ֽ�������

void OLED_FillImage(uint8_t x,uint8_t y,uint8_t width,uint8_t high,const uint8_t*img)
{
    if(x>127||y>63)return;
    uint8_t temp=0,offset=0;
	u8 by=0;
	u8 byte_h=0;
    by=y/8;//��ʼҳ��ַ
    byte_h=high/8;//ͼƬռ�Դ��ֽڸ߶�
	
    if(high%8>0)byte_h++;
    
    offset=(y%8)%8;
    if(offset!=8)byte_h++;
    for(uint16_t h=0;h<byte_h;h++)
    {
        for(uint16_t w=0;w<width;w++)
        {
            temp=0;
            if(h==0)//��һ��
            {
                temp=img[(h)*width+w]<<offset;//����offset
            }
            else if(h==(byte_h-1))//���һ��
            {
                if(offset!=8)
                {
                    temp=img[(h-1)*width+w]>>(8-offset);//����(8-offset)
                }
                else
                {
                    temp=img[(h-1)*width+w]>>(8-offset);//��һ�����ص�����8-offset
                    temp|=img[(h)*width+w]<<offset;
                }
                
            }
            else//
            {
                temp=img[(h-1)*width+w]>>(8-offset);//��һ�����ص�����8-offset
                temp|=(img[(h)*width+w]<<offset);
            }
            if(by+h>8||x+w>127)continue;
            OLED_GRAM[by+h][x+w]|=temp;
			//OLED_Refresh_Gram();
        }
    }
}

void OLED_InvertBlock(uint8_t x,uint8_t y,uint8_t width,uint8_t high)
{
	if((x>127||y>63))return;
	
	uint8_t start_line=y/8;
	uint8_t end_line=(y+high)/8;
	uint8_t start_offset=y%8;
	uint8_t end_offset=8-((y+high)%8);
	uint8_t temp=0;
	for(uint8_t h=0;h<=end_line-start_line;h++)
	{
		for(uint8_t w=0;w<width;w++)
		{
			temp=0;
			if(y/8+h>8||x+w>127)continue;
			if(h==0)
			{
				temp|=OLED_GRAM[y/8+h][x+w]^(0xFF<<start_offset);
			}
			if(h==end_line-start_line)
			{
				temp&=(0xFF>>end_offset);
				if(end_offset==8)
				{
					//temp|=OLED_GRAM[y/8+h][x+w]^(0xFF);
				}
				else
				{
					temp|=OLED_GRAM[y/8+h][x+w]^(0xFF>>end_offset);
				}
				
			}
			if((h!=0)&&(h!=end_line-start_line))
			{
				temp=OLED_GRAM[y/8+h][x+w]^(0xFF);
			}
			OLED_GRAM[y/8+h][x+w]=temp;
		}
	}
}
void OLED_ClearGram(void)
{
    memset(OLED_GRAM,0,1024);
}
//��ʼ��SSD1306					    
void OLED_Init(void)
{ 	
 
 	// GPIO_InitTypeDef  GPIO_InitStructure;
 	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //ʹ��PC,D,G�˿�ʱ��

	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6;	 //PD3,PD6�������  
 	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	// GPIO_Init(GPIOD, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
 	// GPIO_SetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_6);	//PD3,PD6 �����

 #if OLED_MODE==1
 
 	GPIO_InitStructure.GPIO_Pin =0xFF; //PC0~7 OUT�������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,0xFF); //PC0~7�����

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //PG13,14,15 OUT�������
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PG13,14,15 OUT  �����

 #else
 	if(HSI2C_switch)
	{
		I2C_GPIO_Config();
		I2C_Config();
	}
	else
	{
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);	  
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);	
	}


 #endif
  							  
	//OLED_RST_Clr();
	// delay_ms(1000);
	//OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	

    //��ת
    OLED_WR_Byte(0xC0,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xA0,OLED_CMD); //������ʾ	 
    OLED_ClearGram();
	//OLED_Clear();
    // delay_ms(800);

	// OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	// OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	// OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	// OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	// OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	// OLED_WR_Byte(0x81,OLED_CMD); // contract control
	// OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	// OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	// OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse A6

	// OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	// OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	// OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	// OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	// OLED_WR_Byte(0x00,OLED_CMD);//
	
	// OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	// OLED_WR_Byte(0x80,OLED_CMD);//
	
	// OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	// OLED_WR_Byte(0x05,OLED_CMD);//
	
	// OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	// OLED_WR_Byte(0xF1,OLED_CMD);//
	
	// OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	// OLED_WR_Byte(0x12,OLED_CMD);//
	
	// OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	// OLED_WR_Byte(0x30,OLED_CMD);//
	
	// OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	// OLED_WR_Byte(0x14,OLED_CMD);//
	
	// OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel

	// OLED_WR_Byte(0x20,OLED_CMD);
	// OLED_WR_Byte(0x02,OLED_CMD);
}  
