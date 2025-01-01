#include "oled_bsp.h"
#include "stdlib.h"
#include "oledfont_bsp.h" 
#include "stdint.h"
#include "string.h"
#include "stdio.h"

uint8_t HSI2C_switch=0;
//OLED的显存
//存放格式如下.
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


//硬件iic
void I2C_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//初始化 IIC_GPIO 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	//初始化IIC_SCL
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//初始化IIC_SDA	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
void I2C_Config(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_GPIO_Config();//配置SDA和SCL GPIO
	//初始化IIC外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	
	//初始化IIC 结构体
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;			    //使能应答
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//IIC 7 位寻址
	I2C_InitStruct.I2C_ClockSpeed = 400000;//通信速率 400k
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;		//占空比 1/2
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;			  //选择IIC模式
	I2C_InitStruct.I2C_OwnAddress1 = 0x01;//输入主机地址，与从机有所区别即可正常通信
	I2C_AcknowledgeConfig(I2C2, DISABLE );
	I2C_Init(I2C2,&I2C_InitStruct);
	//使能IIC
	I2C_Cmd(I2C2, ENABLE);
}

void HI2C_WriteByte(uint8_t data)
{
	I2C_SendData(I2C2, data); //设置页地址
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) )
	{

	} //等待事件EV8
}

void HI2C_Start()
{
	// uint8_t i=5;
	while( I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) )
	{

	} //等待总线空闲

	I2C_GenerateSTART(I2C2, ENABLE); //I2C发送起始信号
	
}

void HI2C_SendAddress(uint8_t addr)
{
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) )
	{

	} //等待EV5=1,具体可以参考手册上面EV5事件
	I2C_Send7bitAddress(I2C2, addr, I2C_Direction_Transmitter); //发送从机7位地址
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) )
	{

	} //等待EV6=1
}

void HI2C_Stop()
{
	uint16_t i=1000;
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) )
	{
		i--;
		if(i==0)break;
	} 
	I2C_GenerateSTOP(I2C2, ENABLE); //I2C发送停止信号
	
}
//向OLED寄存器地址写一个byte的数据
void HI2C_WriteData(uint8_t dat,uint8_t cmd)
{
	while( I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY) ); //等待总线空闲
	I2C_GenerateSTART(I2C2, ENABLE); //I2C发送起始信号
	
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT) ); //等待EV5=1,具体可以参考手册上面EV5事件
	I2C_Send7bitAddress(I2C2, 0x78, I2C_Direction_Transmitter); //发送从机7位地址
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) ); //等待EV6=1
	
	//while( I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //等待事件EV8_1
	I2C_SendData(I2C2, cmd); //发送地址字节
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //等待事件EV8
	
	I2C_SendData(I2C2, dat); //发送数据字节
	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING) ); //等待事件EV8

	while( !I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED) ); //等待事件EV8_2	
	I2C_GenerateSTOP(I2C2, ENABLE); //I2C发送停止信号

}

//更新显存到LCD		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[i][n],OLED_DATA); 
	}   
}
#if OLED_MODE==1
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
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
//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
    if(HSI2C_switch)
    {
        //硬件iic
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
        //软件iic
        IIC_Start();
        Write_IIC_Byte(0x78);
        IIC_Wait_Ack();		  
        if(cmd)//写数据
        {
            //OLED_RS_Set(); 	
            Write_IIC_Byte(0x40);			//write data
		    IIC_Wait_Ack();	
        }
        else//写命令
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
	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[i][n]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=y/8;
	bx=y%8;
	temp=1<<(bx);
	if(t)OLED_GRAM[pos][x]|=temp;
	else OLED_GRAM[pos][x]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体
		else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
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
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
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

//显示图片
//x,y:起点坐标  
//w:图片像素宽度
//h:图片像素高度
//*img:图片
//图片格式：低位在上，低字节在左上

void OLED_FillImage(uint8_t x,uint8_t y,uint8_t width,uint8_t high,const uint8_t*img)
{
    if(x>127||y>63)return;
    uint8_t temp=0,offset=0;
	u8 by=0;
	u8 byte_h=0;
    by=y/8;//起始页地址
    byte_h=high/8;//图片占显存字节高度
	
    if(high%8>0)byte_h++;
    
    offset=(y%8)%8;
    if(offset!=8)byte_h++;
    for(uint16_t h=0;h<byte_h;h++)
    {
        for(uint16_t w=0;w<width;w++)
        {
            temp=0;
            if(h==0)//第一行
            {
                temp=img[(h)*width+w]<<offset;//下移offset
            }
            else if(h==(byte_h-1))//最后一行
            {
                if(offset!=8)
                {
                    temp=img[(h-1)*width+w]>>(8-offset);//上移(8-offset)
                }
                else
                {
                    temp=img[(h-1)*width+w]>>(8-offset);//上一行像素点上移8-offset
                    temp|=img[(h)*width+w]<<offset;
                }
                
            }
            else//
            {
                temp=img[(h-1)*width+w]>>(8-offset);//上一行像素点上移8-offset
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
//初始化SSD1306					    
void OLED_Init(void)
{ 	
 
 	// GPIO_InitTypeDef  GPIO_InitStructure;
 	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 	// RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOG, ENABLE);	 //使能PC,D,G端口时钟

	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_6;	 //PD3,PD6推挽输出  
 	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	// GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	// GPIO_SetBits(GPIOD,GPIO_Pin_3|GPIO_Pin_6);	//PD3,PD6 输出高

 #if OLED_MODE==1
 
 	GPIO_InitStructure.GPIO_Pin =0xFF; //PC0~7 OUT推挽输出
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOC,0xFF); //PC0~7输出高

 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;				 //PG13,14,15 OUT推挽输出
 	GPIO_Init(GPIOG, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOG,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);						 //PG13,14,15 OUT  输出高

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
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);	  
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11);	
	}


 #endif
  							  
	//OLED_RST_Clr();
	// delay_ms(1000);
	//OLED_RST_Set(); 
					  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	

    //反转
    OLED_WR_Byte(0xC0,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xA0,OLED_CMD); //开启显示	 
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
