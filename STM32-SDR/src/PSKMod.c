/*
 * PSKMod.c
 *
 *  Created on: Apr 19, 2013
 *      Author: CharleyK
 */
#include 	"PSKDet.h"
#include 	"PSKMod.h"
#include 	"PSK_TX_ShapeTable.h"


//#define TXOFF_CODE 2	/* control code that can be placed in the input by Ctrl-Q */
//#define TXON_CODE 3		/* ditto for Ctrl-S */
//#define TXTOG_CODE 1

//#define PHZ_0 0			/*specify various signal phase states */
//#define PHZ_90 1
//#define PHZ_180 2
//#define PHZ_270 3
//#define PHZ_OFF 4

#define SYM_NOCHANGE 0	/*Stay the same phase */
//#define SYM_P90 1		/*Plus 90  deg */
#define SYM_P180 2		/*Plus 180 deg */
//#define SYM_M90 3		/*Minus 90 deg */
#define SYM_OFF 4		/*No output */
#define SYM_ON 5		/*constant output */


float S1, S2;
extern const unsigned int VARICODE_TABLE[256];

void Update_PSK(void) {
	static uint8_t symbol;
	extern const int PSKPhaseLookupTable[6][5];
	static int Itbl_num, Qtbl_num;  //chhstatic
	static int m_PresentPhase; //chhstatic
	extern const float PSKShapeTbl[2049];

	int tempdata;

	S1 = PSKShapeTbl[Itbl_num*256+m_Ramp+1];
	S2 = PSKShapeTbl[Qtbl_num*256+m_Ramp+1];

	m_Ramp += 1;
	if (m_Ramp == 256)  /* time to update symbol */
	{
	m_Ramp = 0;
	symbol = GetNextBPSKSymbol();
	tempdata = PSKPhaseLookupTable[symbol][m_PresentPhase];
	Itbl_num = (tempdata & 0x700)>>8;
	Qtbl_num = (tempdata & 0x070)>>4;
	m_PresentPhase = (tempdata & 0x7);
	}
}


void InitPSKModulator(void)
{
	int i;

	m_Ramp = 0;
	m_pTail = 0;
	m_pHead = 0;

	i = 0;
	while(i<26)
	{XmitBuffer[i] = i+97; //Fill Buffer with lower case alphabet
	i++;}


	m_AddEndingZero = TRUE;
	m_SymbolRate = SYMBOL_RATE31;


//	i = 0;
//	while(i<32)		/*create post/preamble tables */
//	{
//		m_Preamble[i] = TXTOG_CODE;
//		m_Postamble[i++] = TXON_CODE;
//	}
//	m_Preamble[i] = 0;		/* null terminate these tables */
//	m_Postamble[i] = 0;

	m_TxShiftReg = 0;
}

char GetNextBPSKSymbol(void)
{
	char symb;
	char ch;

	if( m_TxShiftReg == 0 )
	{
		if( m_AddEndingZero )		/* if is end of code */
		{
			symb = SYM_P180;		/* end with a zero */
			m_AddEndingZero = FALSE;
		}
		else
		{
			ch = GetTxChar();			/*get next character to xmit */
			m_TxShiftReg = VARICODE_TABLE[ ch&0xFF ];
			symb = SYM_P180;	/*Start with a zero */
			}
		}
	else			/* is not end of code word so send next bit */
	{
		if( m_TxShiftReg&0x8000 )
			{symb = SYM_NOCHANGE;}
		else
			{symb = SYM_P180;}

		m_TxShiftReg = m_TxShiftReg<<1;	/*point to next bit */
		if( m_TxShiftReg == 0 )			/* if at end of codeword */
			m_AddEndingZero = TRUE;		/* need to send a zero nextime */
	}
	return symb;
}

/*==================================================================*/
/*get next character/symbol depending on TX state.		    		*/
/*==================================================================*/

/*==================================================================*/
/*		TX Queing routines				    					*/
/*==================================================================*/

/*==================================================================*/
	char GetTxChar(void) {
	char ch;

	//if( m_pHead != m_pTail )	/*if something in Queue */
	//{
	//	ch = XmitBuffer[m_pTail++] & 0x00FF;
	//	if( m_pTail >= TX_BUF_SIZE )
	//		m_pTail = 0;
	//}
	//else
	//	ch = TXTOG_CODE;		/* if que is empty return TXTOG_CODE */
	//if(m_TempNeedShutoff)
	//{
	//	m_TempNeedShutoff = FALSE;
	//	m_NeedShutoff = TRUE;
	//}

	//if(m_TempNoSquelchTail)
	//{
	//	m_TempNoSquelchTail = FALSE;
	//	m_NoSquelchTail = TRUE;
	//}

	ch = XmitBuffer[m_pTail];
	m_pTail++;
	if (m_pTail>25) m_pTail = 0;

	return ch;
	}
