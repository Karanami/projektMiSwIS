/*
 * onewire.h
 *
 *  Created on: Jan 30, 2025
 *      Author: user
 */

#ifndef INC_WIRE_H_
#define INC_WIRE_H_

#include "main.h"
#include "usart.h"
#include "string.h"

#define DS18B20_SCRATCHPAD_SIZE    9
#define DS18B20_READ_ROM           0x33
#define DS18B20_MATCH_ROM          0x55
#define DS18B20_SKIP_ROM           0xCC
#define DS18B20_CONVERT_T          0x44
#define DS18B20_READ_SCRATCHPAD    0xBE

static void set_baudrate(uint32_t baudrate)
{
	  huart7.Instance = UART7;
	  huart7.Init.BaudRate = baudrate;
	  huart7.Init.WordLength = UART_WORDLENGTH_8B;
	  huart7.Init.StopBits = UART_STOPBITS_1;
	  huart7.Init.Parity = UART_PARITY_NONE;
	  huart7.Init.Mode = UART_MODE_TX_RX;
	  huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	  huart7.Init.OverSampling = UART_OVERSAMPLING_16;
	  huart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	  huart7.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	  huart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	  if (HAL_HalfDuplex_Init(&huart7) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_UARTEx_SetTxFifoThreshold(&huart7, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_UARTEx_SetRxFifoThreshold(&huart7, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_UARTEx_DisableFifoMode(&huart7) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

HAL_StatusTypeDef wire_reset(void)
{
	uint8_t data_out = 0xF0;
	uint8_t data_in = 0;
	set_baudrate(9600);
	HAL_UART_Transmit(&huart7, &data_out, 1, HAL_MAX_DELAY);
	HAL_UART_Receive(&huart7, &data_in, 1, HAL_MAX_DELAY);
	set_baudrate(115200);
	if (data_in != 0xF0) return HAL_OK;
	else return HAL_ERROR;
}

static void write_bit(int value)
{
	if (value)
	{
		uint8_t data_out = 0xff;
		HAL_UART_Transmit(&huart7, &data_out, 1, HAL_MAX_DELAY);
	}
	else
	{
		uint8_t data_out = 0x0;
		HAL_UART_Transmit(&huart7, &data_out, 1, HAL_MAX_DELAY);
	}
}
static int read_bit(void)
{
	uint8_t data_out = 0xFF;
	uint8_t data_in = 0;
	HAL_UART_Transmit(&huart7, &data_out, 1, HAL_MAX_DELAY);
	HAL_UART_Receive(&huart7, &data_in, 1, HAL_MAX_DELAY);
	return data_in & 0x01;
}

uint8_t wire_read(void)
{
	uint8_t value = 0;
	int i;
	for (i = 0; i < 8; i++)
	{
		value >>= 1;
		if (read_bit())
			value |= 0x80;
	}
	return value;
}

void wire_write(uint8_t byte)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		write_bit(byte & 0x01);
		byte >>= 1;
	}
}

static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		uint8_t b = crc ^ byte;
		crc >>= 1;
		if (b & 0x01) crc ^= 0x8c;
		byte >>= 1;
	}
	return crc;
}
uint8_t wire_crc(const uint8_t* data, int len)
{
	int i;
	uint8_t crc = 0;
	for (i = 0; i < len; i++) crc = byte_crc(crc, data[i]);
	return crc;
}

//======================================================================

#define DS18B20_ROM_CODE_SIZE		8

HAL_StatusTypeDef ds18b20_init(void)
{
  return wire_reset();
}

HAL_StatusTypeDef ds18b20_read_address(uint8_t* rom_code)
{
	int i;
	uint8_t crc;
	if (wire_reset() != HAL_OK) return HAL_ERROR;
	wire_write(DS18B20_READ_ROM);

	for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++) rom_code[i] = wire_read();

	crc = wire_crc(rom_code, DS18B20_ROM_CODE_SIZE - 1);

	if (rom_code[DS18B20_ROM_CODE_SIZE - 1] == crc) return HAL_OK;
	else return HAL_ERROR;
}

static HAL_StatusTypeDef send_cmd(const uint8_t* rom_code, uint8_t cmd)
{
	int i;
	if (wire_reset() != HAL_OK) return HAL_ERROR;
	if (!rom_code)
	{
		wire_write(DS18B20_SKIP_ROM);
		}
		else
		{
		wire_write(DS18B20_MATCH_ROM);
		for (i = 0; i < DS18B20_ROM_CODE_SIZE; i++) wire_write(rom_code[i]);
	}
	wire_write(cmd);
	return HAL_OK;
}

HAL_StatusTypeDef ds18b20_start_measure(const uint8_t* rom_code)
{
  return send_cmd(rom_code, DS18B20_CONVERT_T);
}

static HAL_StatusTypeDef ds18b20_read_scratchpad(const uint8_t* rom_code, uint8_t* scratchpad)
{
	int i;
	uint8_t crc;

	if (send_cmd(rom_code, DS18B20_READ_SCRATCHPAD) != HAL_OK) return HAL_ERROR;

	for (i = 0; i < DS18B20_SCRATCHPAD_SIZE; i++) scratchpad[i] = wire_read();
	crc = wire_crc(scratchpad, DS18B20_SCRATCHPAD_SIZE - 1);

	if (scratchpad[DS18B20_SCRATCHPAD_SIZE - 1] == crc) return HAL_OK;
	else return HAL_ERROR;
}

float ds18b20_get_temp(const uint8_t* rom_code)
{
	uint8_t scratchpad[DS18B20_SCRATCHPAD_SIZE];
	int16_t temp;
	if (ds18b20_read_scratchpad(rom_code, scratchpad) != HAL_OK) return 85.0f;
	memcpy(&temp, &scratchpad[0], sizeof(temp));
	return temp / 16.0f;
}

#endif /* INC_WIRE_H_ */
