/*
 * Register definitions for Programmable 1-PLL VCXO Clock Synthesizer CDCE913
 *
 * Copyright (C) 2011 Thesys-Intechna
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __CDCE913_H__
#define __CDCE913_H__

/* Generic Configuration Register */
#define CDCE913_VID_OFFSET				0x00
#define CDCE913_VID_SIZE				4
#define CDCE913_RID_OFFSET				0x04
#define CDCE913_RID_SIZE				3
#define CDCE913_E_EL_OFFSET				0x07
#define CDCE913_E_EL_SIZE				1
#define CDCE913_SLAVE_ADR_OFFSET		0x08
#define CDCE913_SLAVE_ADR_SIZE			2
#define CDCE913_INCLK_OFFSET			0x0A
#define CDCE913_INCLK_SIZE				2
#define CDCE913_PWDN_OFFSET				0x0C
#define CDCE913_PWDN_SIZE				1
#define CDCE913_EELOCK_OFFSET			0x0D
#define CDCE913_EELOCK_SIZE				1
#define CDCE913_EEPIP_OFFSET			0x0E
#define CDCE913_EEPIP_SIZE				1
#define CDCE913_PDIV1_98_OFFSET			0x10
#define CDCE913_PDIV1_98_SIZE			2
#define CDCE913_Y1_ST0_OFFSET			0x12
#define CDCE913_Y1_ST0_SIZE				2
#define CDCE913_Y1_ST1_OFFSET			0x14
#define CDCE913_Y1_ST1_SIZE				2
#define CDCE913_SPICON_OFFSET			0x16
#define CDCE913_SPICON_SIZE				1
#define CDCE913_M1_OFFSET				0x17
#define CDCE913_M1_SIZE					1
#define CDCE913_PDIV1_70_OFFSET			0x18
#define CDCE913_PDIV1_70_SIZE			8
#define CDCE913_Y1_0_OFFSET				0x20
#define CDCE913_Y1_0_SIZE				1
#define CDCE913_Y1_1_OFFSET				0x21
#define CDCE913_Y1_1_SIZE				1
#define CDCE913_Y1_2_OFFSET				0x22
#define CDCE913_Y1_2_SIZE				1
#define CDCE913_Y1_3_OFFSET				0x23
#define CDCE913_Y1_3_SIZE				1
#define CDCE913_Y1_4_OFFSET				0x24
#define CDCE913_Y1_4_SIZE				1
#define CDCE913_Y1_5_OFFSET				0x25
#define CDCE913_Y1_5_SIZE				1
#define CDCE913_Y1_6_OFFSET				0x26
#define CDCE913_Y1_6_SIZE				1
#define CDCE913_Y1_7_OFFSET				0x27
#define CDCE913_Y1_7_SIZE				1
#define CDCE913_XCSEL_OFFSET			0x2B
#define CDCE913_XCSEL_SIZE				5
#define CDCE913_EEWRITE_OFFSET			0x30
#define CDCE913_EEWRITE_SIZE			1
#define CDCE913_BCOUNT_OFFSET			0x31
#define CDCE913_BCOUNT_SIZE				7
/* PLL1 Configuration Register */
#define CDCE913_SSC1_5_21_OFFSET		0x80
#define CDCE913_SSC1_5_21_SIZE			2
#define CDCE913_SSC1_6_20_OFFSET		0x80
#define CDCE913_SSC1_6_20_SIZE			3
#define CDCE913_SSC1_7_20_OFFSET		0x80
#define CDCE913_SSC1_7_20_SIZE			3
#define CDCE913_SSC1_2_2_OFFSET			0x88
#define CDCE913_SSC1_2_2_SIZE			1
#define CDCE913_SSC1_3_20_OFFSET		0x89
#define CDCE913_SSC1_3_20_SIZE			3
#define CDCE913_SSC1_4_20_OFFSET		0x8C
#define CDCE913_SSC1_4_20_SIZE			3
#define CDCE913_SSC1_5_0_OFFSET			0x8F
#define CDCE913_SSC1_5_0_SIZE			1
#define CDCE913_SSC1_0_20_OFFSET		0x90
#define CDCE913_SSC1_0_20_SIZE			3
#define CDCE913_SSC1_1_20_OFFSET		0x93
#define CDCE913_SSC1_1_20_SIZE			3
#define CDCE913_SSC1_2_10_OFFSET		0x96
#define CDCE913_SSC1_2_10_SIZE			2
#define CDCE913_FS1_0_OFFSET			0x98
#define CDCE913_FS1_0_SIZE				1
#define CDCE913_FS1_1_OFFSET			0x99
#define CDCE913_FS1_1_SIZE				1
#define CDCE913_FS1_2_OFFSET			0x9A
#define CDCE913_FS1_2_SIZE				1
#define CDCE913_FS1_3_OFFSET			0x9B
#define CDCE913_FS1_3_SIZE				1
#define CDCE913_FS1_4_OFFSET			0x9C
#define CDCE913_FS1_4_SIZE				1
#define CDCE913_FS1_5_OFFSET			0x9D
#define CDCE913_FS1_5_SIZE				1
#define CDCE913_FS1_6_OFFSET			0x9E
#define CDCE913_FS1_6_SIZE				1
#define CDCE913_FS1_7_OFFSET			0x9F
#define CDCE913_FS1_7_SIZE				1
#define CDCE913_Y2Y3_ST0_OFFSET			0xA0
#define CDCE913_Y2Y3_ST0_SIZE			2
#define CDCE913_Y2Y3_ST1_OFFSET			0xA2
#define CDCE913_Y2Y3_ST1_SIZE			2
#define CDCE913_M3_OFFSET				0xA4
#define CDCE913_M3_SIZE					2
#define CDCE913_M2_OFFSET				0xA6
#define CDCE913_M2_SIZE					1
#define CDCE913_MUX1_OFFSET				0xA7
#define CDCE913_MUX1_SIZE				1
#define CDCE913_Y2Y3_0_OFFSET			0xA8
#define CDCE913_Y2Y3_0_SIZE				1
#define CDCE913_Y2Y3_1_OFFSET			0xA9
#define CDCE913_Y2Y3_1_SIZE				1
#define CDCE913_Y2Y3_2_OFFSET			0xAA
#define CDCE913_Y2Y3_2_SIZE				1
#define CDCE913_Y2Y3_3_OFFSET			0xAB
#define CDCE913_Y2Y3_3_SIZE				1
#define CDCE913_Y2Y3_4_OFFSET			0xAC
#define CDCE913_Y2Y3_4_SIZE				1
#define CDCE913_Y2Y3_5_OFFSET			0xAD
#define CDCE913_Y2Y3_5_SIZE				1
#define CDCE913_Y2Y3_6_OFFSET			0xAE
#define CDCE913_Y2Y3_6_SIZE				1
#define CDCE913_Y2Y3_7_OFFSET			0xAF
#define CDCE913_Y2Y3_7_SIZE				1
#define CDCE913_PDIV2_OFFSET			0xB0
#define CDCE913_PDIV2_SIZE				7
#define CDCE913_SSC1DC_OFFSET			0xB7
#define CDCE913_SSC1DC_SIZE				1
#define CDCE913_PDIV3_OFFSET			0xB8
#define CDCE913_PDIV3_SIZE				7
#define CDCE913_PLL1_0N_11_4_OFFSET		0xC0
#define CDCE913_PLL1_0N_11_4_SIZE		8
#define CDCE913_PLL1_0R_8_5_OFFSET		0xC8
#define CDCE913_PLL1_0R_8_5_SIZE		4
#define CDCE913_PLL1_0N_3_0_OFFSET		0xCC
#define CDCE913_PLL1_0N_3_0_SIZE		4
#define CDCE913_PLL1_0Q_5_3_OFFSET		0xD0
#define CDCE913_PLL1_0Q_5_3_SIZE		3
#define CDCE913_PLL1_0R_4_0_OFFSET		0xD3
#define CDCE913_PLL1_0R_4_0_SIZE		5
#define CDCE913_VCO1_0_RANGE_OFFSET		0xD8
#define CDCE913_VCO1_0_RANGE_SIZE		2
#define CDCE913_PLL1_0P_2_0_OFFSET		0xDA
#define CDCE913_PLL1_0P_2_0_SIZE		3
#define CDCE913_PLL1_0Q_2_0_OFFSET		0xDD
#define CDCE913_PLL1_0Q_2_0_SIZE		3
#define CDCE913_PLL1_1N_11_4_OFFSET		0xE0
#define CDCE913_PLL1_1N_11_4_SIZE		8
#define CDCE913_PLL1_1R_8_5_OFFSET		0xE8
#define CDCE913_PLL1_1R_8_5_SIZE		4
#define CDCE913_PLL1_1N_3_0_OFFSET		0xEC
#define CDCE913_PLL1_1N_3_0_SIZE		4
#define CDCE913_PLL1_1Q_5_3_OFFSET		0xF0
#define CDCE913_PLL1_1Q_5_3_SIZE		3
#define CDCE913_PLL1_1R_4_0_OFFSET		0xF3
#define CDCE913_PLL1_1R_4_0_SIZE		5
#define CDCE913_VCO_1_1_RANGE_OFFSET	0xF8
#define CDCE913_VCO_1_1_RANGE_SIZE		2
#define CDCE913_PLL1_1P_2_0_OFFSET		0xFA
#define CDCE913_PLL1_1P_2_0_SIZE		3
#define CDCE913_PLL1_1Q_2_0_OFFSET		0xFD
#define CDCE913_PLL1_1Q_2_0_SIZE		3
/* Additional bitfields */
#define CDCE913_Y1_X_OFFSET				0x20
#define CDCE913_Y1_X_SIZE				8

/* PLL1 Selecttion (Modulation Amount) */
#define CDCE913_SSC_SEL_OFF				0x00
#define CDCE913_SSC_SEL_025				0x01
#define CDCE913_SSC_SEL_050				0x02
#define CDCE913_SSC_SEL_075				0x03
#define CDCE913_SSC_SEL_100				0x04
#define CDCE913_SSC_SEL_125				0x05
#define CDCE913_SSC_SEL_150				0x06
#define CDCE913_SSC_SEL_200				0x07

/* Bit manipulation macros */
#define CDCE913_REG(name) \
	(CDCE913_##name##_OFFSET >> 3)
#define CDCE913_BIT(name) \
	(CDCE913_##name##_OFFSET & 0x07)
#define CDCE913_BITN(name, num) \
	((CDCE913_##name##_OFFSET & 0x07) << ##num##)
#define CDCE913_BF(name,value) \
	(((value) & ((1 << CDCE913_##name##_SIZE) - 1)) << CDCE913_##name##_OFFSET)
#define CDCE913_BFEXT(name,value) \
	(((value) >> CDCE913_##name##_OFFSET) & ((1 << CDCE913_##name##_SIZE) - 1))
#define CDCE913_BFINS(name,value,old) \
	( ((old) & ~(((1 << CDCE913_##name##_SIZE) - 1) << CDCE913_##name##_OFFSET)) \
	  | CDCE913_BF(name,value))

#endif /* __CDCE913_H__ */
