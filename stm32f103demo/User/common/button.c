/************************************************************
  * @brief   ��������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/
#include "./common/button.h"

/*******************************************************************
 *                          ��������                               
 *******************************************************************/

static struct button* Head_Button = NULL;


/*******************************************************************
 *                         ��������     
 *******************************************************************/
 
static void Print_Btn_Info(Button_t* btn);
static void Add_Button(Button_t* btn);


/************************************************************
  * @brief   ��������
	* @param   name : ��������
	* @param   btn : �����ṹ��
  * @param   read_btn_level : ������ƽ��ȡ��������Ҫ�û��Լ�ʵ�ַ���uint8_t���͵ĵ�ƽ
  * @param   btn_trigger_level : ����������ƽ
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Create(const char *name,
                  Button_t *btn, 
                  uint8_t(*read_btn_level)(void),
                  uint8_t btn_trigger_level)
{
  if( btn == NULL)
  {
    PRINT_ERR("struct button is null!");
    ASSERT(ASSERT_ERR);
  }
  
  memset(btn, 0, sizeof(struct button));  //����ṹ����Ϣ�������û���֮ǰ���
 
  StrnCopy(btn->Name, name, BTN_NAME_MAX); /* ������������ */
  
  
  btn->Button_State = NONE_TRIGGER;           //����״̬
  btn->Button_Last_State = NONE_TRIGGER;      //������һ��״̬
  btn->Button_Trigger_Event = NONE_TRIGGER;   //���������¼�
  btn->Read_Button_Level = read_btn_level;    //��������ƽ����
  btn->Button_Trigger_Level = btn_trigger_level;  //����������ƽ
  btn->Button_Last_Level = btn->Read_Button_Level(); //������ǰ��ƽ
  btn->Debounce_Time = 0;
  
  PRINT_DEBUG("button create success!");
  
  Add_Button(btn);          //������ʱ�����ӵ���������
  
  Print_Btn_Info(btn);     //��ӡ��Ϣ
 
}

/************************************************************
  * @brief   ���������¼���ص�����ӳ����������
	* @param   btn : �����ṹ��
	* @param   btn_event : ���������¼�
  * @param   btn_callback : ��������֮��Ļص�������������Ҫ�û�ʵ��
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if( btn == NULL)
  {
    PRINT_ERR("struct button is null!");
    ASSERT(ASSERT_ERR);       //����
  }
  
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback; //�����¼������Ļص����������ڴ��������¼�
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; //�����¼������Ļص����������ڴ��������¼�
  }
}

/************************************************************
  * @brief   ɾ��һ���Ѿ������İ���
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button** curr;
  for(curr = &Head_Button; *curr;) 
  {
    struct button* entry = *curr;
    if (entry == btn) 
    {
      *curr = entry->Next;
    } 
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   ��ȡ�����������¼�
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  //�����¼������Ļص����������ڴ��������¼�
  for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
  {
    if(btn->CallBack_Function[i] != 0)
    {
      PRINT_INFO("Button_Event:%d",i);
    }      
  } 
}

uint8_t Get_Button_Event(Button_t *btn)
{
  return (uint8_t)(btn->Button_Trigger_Event);
}
/************************************************************
  * @brief   ��ȡ�����������¼�
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
uint8_t Get_Button_State(Button_t *btn)
{
  return (uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   �������ڴ�������
  * @param   btn:�����İ���
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    ������һ�����ڵ��ô˺�������������Ϊ20~50ms
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  uint8_t current_level = (uint8_t)btn->Read_Button_Level();//��ȡ��ǰ������ƽ
  
  if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME)) //������ƽ�����仯������
  {
      btn->Button_Last_Level = current_level; //���µ�ǰ������ƽ
      btn->Debounce_Time = 0;                 //ȷ�����ǰ���
      
      //���������û�����µģ��ı䰴��״̬Ϊ����(�״ΰ���/˫������)
      if((btn->Button_State == NONE_TRIGGER)||(btn->Button_State == BUTTON_DOUBLE))
      {
        btn->Button_State = BUTTON_DOWM;
      }
      //�ͷŰ���
      else if(btn->Button_State == BUTTON_DOWM)
      {
        btn->Button_State = BUTTON_UP;
        PRINT_DEBUG("�ͷ��˰���");
      }
  }
  
  switch(btn->Button_State)
  {
    case BUTTON_DOWM :            // ����״̬
    {
      if(btn->Button_Last_Level == btn->Button_Trigger_Level) //��������
      {
        #ifdef CONTINUOS_TRIGGER     //֧����������

        if(++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
        {
          btn->Button_Cycle = 0;
          btn->Button_Trigger_Event = BUTTON_CONTINUOS; 
          TRIGGER_CB(BUTTON_CONTINUOS);    //����
          PRINT_DEBUG("����");
        }
        
        #else
        
        btn->Button_Trigger_Event = BUTTON_DOWM;
      
        if(++(btn->Long_Time) >= BUTTON_LONG_TIME)  //�ͷŰ���ǰ���´����¼�Ϊ����
        {
          #ifdef LONG_FREE_TRIGGER
          
          btn->Button_Trigger_Event = BUTTON_LONG; 
          
          #else
          
          if(++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)    //������������������
          {
            btn->Button_Cycle = 0;
            btn->Button_Trigger_Event = BUTTON_LONG; 
            TRIGGER_CB(BUTTON_LONG);    //����
          }
          #endif
          
          if(btn->Long_Time == 0xFF)  //����ʱ�����
          {
            btn->Long_Time = BUTTON_LONG_TIME;
          }
          PRINT_DEBUG("����");
        }
          
        #endif
      }

      break;
    } 
    
    case BUTTON_UP :        // ����״̬
    {
      if(btn->Button_Trigger_Event == BUTTON_DOWM)  //��������
      {
        if((btn->Timer_Count <= BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State == BUTTON_DOUBLE)) // ˫��
        {
          btn->Button_Trigger_Event = BUTTON_DOUBLE;
          TRIGGER_CB(BUTTON_DOUBLE);    
          PRINT_DEBUG("˫��");
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = NONE_TRIGGER;
        }
        else
        {
            btn->Timer_Count=0;
            btn->Long_Time = 0;   //��ⳤ��ʧ�ܣ���0
          
          #ifndef SINGLE_AND_DOUBLE_TRIGGER 
            TRIGGER_CB(BUTTON_DOWM);    //����
          #endif
            btn->Button_State = BUTTON_DOUBLE;
            btn->Button_Last_State = BUTTON_DOUBLE;
          
        }
      }
      
      else if(btn->Button_Trigger_Event == BUTTON_LONG)
      {
        #ifdef LONG_FREE_TRIGGER
          TRIGGER_CB(BUTTON_LONG);    //����
        #else
          TRIGGER_CB(BUTTON_LONG_FREE);    //�����ͷ�
        #endif
        btn->Long_Time = 0;
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = BUTTON_LONG;
      } 
      
      #ifdef CONTINUOS_TRIGGER
        else if(btn->Button_Trigger_Event == BUTTON_CONTINUOS)  //����
        {
          btn->Long_Time = 0;
          TRIGGER_CB(BUTTON_CONTINUOS_FREE);    //�����ͷ�
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = BUTTON_CONTINUOS;
        } 
      #endif
      
      break;
    }
    
    case BUTTON_DOUBLE :
    {
      btn->Timer_Count++;     //ʱ���¼ 
      if(btn->Timer_Count>=BUTTON_DOUBLE_TIME)
      {
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = NONE_TRIGGER;
      }
      #ifdef SINGLE_AND_DOUBLE_TRIGGER
      
        if((btn->Timer_Count>=BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State != BUTTON_DOWM))
        {
          btn->Timer_Count=0;
          TRIGGER_CB(BUTTON_DOWM);    //����
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = BUTTON_DOWM;
        }
        
      #endif

      break;
    }

    default :
      break;
  }
  
}
/************************************************************
  * @brief   �����ķ�ʽɨ�谴�������ᶪʧÿ������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    �˺���Ҫ���ڵ��ã�����20-50ms����һ��
  ***********************************************************/
void Button_Process(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
      Button_Cycle_Process(pass_btn);
  }
}

/************************************************************
  * @brief   ��������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
void Search_Button(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != NULL; pass_btn = pass_btn->Next)
  {
    PRINT_INFO("button node have %s",pass_btn->Name);
  }
}



/************************************************************
  * @brief   �������а����ص�����
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    �ݲ�ʵ��
  ***********************************************************/
void Button_Process_CallBack(void *btn)
{
  uint8_t btn_event = Get_Button_Event(btn);

  switch(btn_event)
  {
    case BUTTON_DOWM:
    {
      PRINT_INFO("������İ��´����Ĵ����߼�");
      break;
    }
    
    case BUTTON_UP:
    {
      PRINT_INFO("��������ͷŴ����Ĵ����߼�");
      break;
    }
    
    case BUTTON_DOUBLE:
    {
      PRINT_INFO("�������˫�������Ĵ����߼�");
      break;
    }
    
    case BUTTON_LONG:
    {
      PRINT_INFO("������ĳ��������Ĵ����߼�");
      break;
    }
    
    case BUTTON_LONG_FREE:
    {
      PRINT_INFO("������ĳ����ͷŴ����Ĵ����߼�");
      break;
    }
    
    case BUTTON_CONTINUOS:
    {
      PRINT_INFO("����������������Ĵ����߼�");
      break;
    }
    
    case BUTTON_CONTINUOS_FREE:
    {
      PRINT_INFO("����������������ͷŵĴ����߼�");
      break;
    }
      
  } 
}


/************************************************************
  *******             �������ڲ����ú���           **********
  ***********************************************************/
static void Print_Btn_Info(Button_t* btn)
{
  
  PRINT_INFO("button struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\
              ",
              btn->Name,
              btn->Button_State,
              btn->Button_Trigger_Event,
              btn->Button_Trigger_Level,
              btn->Button_Last_Level);
  Search_Button();
}
/************************************************************
  * @brief   ʹ�õ�������������������
	* @param   NULL
  * @return  NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    NULL
  ***********************************************************/
static void Add_Button(Button_t* btn)
{
  struct button *pass_btn = Head_Button;
  
  while(pass_btn)
  {
    pass_btn = pass_btn->Next;
  }
  
  btn->Next = Head_Button;
  Head_Button = btn;
}


/*************************** ������example ******************************************/

///**
//  ******************************************************************************
//  * @file    main.c
//  * @author  jiejie
//  * @version V1.0
//  * @date    2018-xx-xx
//  * @brief   main
//  ******************************************************************************
//  * @attention ��demo����RTOS�汾����ҪRTOS��demo���ע�ܽܵ�GitHub��
//  *	GitHub��https://github.com/jiejieTop
//  *
//  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
//  * ��̳    :http://www.firebbs.cn
//  * �Ա�    :https://fire-stm32.taobao.com
//  *
//  ******************************************************************************
//  */ 
//#include "include.h"

///**
//  ******************************************************************
//													   ��������
//  ******************************************************************
//  */ 

//Button_t Button1;
//Button_t Button2; 


///**
//  ******************************************************************
//														��������
//  ******************************************************************
//  */ 
//static void BSP_Init(void);

//void Btn1_Dowm_CallBack(void *btn)
//{
//  PRINT_INFO("Button1 ����!");
//}

//void Btn1_Double_CallBack(void *btn)
//{
//  PRINT_INFO("Button1 ˫��!");
//}

//void Btn1_Long_CallBack(void *btn)
//{
//  PRINT_INFO("Button1 ����!");
//  
//  Button_Delete(&Button2);
//  PRINT_INFO("ɾ��Button1");
//  Search_Button();
//}

//void Btn2_Dowm_CallBack(void *btn)
//{
//  PRINT_INFO("Button2 ����!");
//}

//void Btn2_Double_CallBack(void *btn)
//{
//  PRINT_INFO("Button2 ˫��!");
//}

//void Btn2_Long_CallBack(void *btn)
//{
//  PRINT_INFO("Button2 ����!");
//}


///**
//  ******************************************************************
//  * @brief   ������
//  * @author  jiejie
//  * @version V1.0
//  * @date    2018-xx-xx
//  ******************************************************************
//  */ 
//int main(void)
//{

//	BSP_Init();
//  
////  PRINT_DEBUG("��ǰ��ƽ��%d",Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN));
//  
//  Button_Create("Button1",
//                &Button1, 
//                Read_KEY1_Level, 
//                KEY_ON);
//  Button_Attach(&Button1,BUTTON_DOWM,Btn1_Dowm_CallBack);
//  Button_Attach(&Button1,BUTTON_DOUBLE,Btn1_Double_CallBack);
//  Button_Attach(&Button1,BUTTON_LONG,Btn1_Long_CallBack);
//  
//  Button_Create("Button2",
//                &Button2, 
//                Read_KEY2_Level, 
//                KEY_ON);
//  Button_Attach(&Button2,BUTTON_DOWM,Btn2_Dowm_CallBack);
//  Button_Attach(&Button2,BUTTON_DOUBLE,Btn2_Double_CallBack);
//  Button_Attach(&Button2,BUTTON_LONG,Btn2_Long_CallBack);
// 
//	while(1)                            
//	{

//    Button_Process();     //��Ҫ���ڵ��ð�����������

//		Delay_ms(20);
//    
//    
//    
//	}
//}

///**
//  ******************************************************************
//  * @brief   BSP_Init���������а弶��ʼ��
//  * @author  jiejie
//  * @version V1.0
//  * @date    2018-xx-xx
//  ******************************************************************
//  */ 
//static void BSP_Init(void)
//{
//	/* LED ��ʼ�� */
//	LED_GPIO_Config();
//	
//#if USE_DWT_DELAY
//	/* �ں˾�ȷ��ʱ����ʼ�� */
//	CPU_TS_TmrInit();
//#else
//	/* �δ�ʱ����ʼ�� */
//	SysTick_Init();
//#endif
//	
//	/* ���ڳ�ʼ�� */
//	USART_Config();
//  
//	/* ������ʼ�� */
//  Key_GPIO_Config();
//  
////	/* �ⲿ�жϳ�ʼ�� */
////	EXTI_Key_Config(); 
//	
//	CRC_Config();
//	
//	/* ��ӡ��Ϣ */
//	PRINT_INFO("welcome to learn jiejie stm32 library!\n");
//	
//}


///********************************END OF FILE***************************************/


