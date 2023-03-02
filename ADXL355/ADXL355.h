
#ifndef INC_ADXL355_H_
#define INC_ADXL355_H_

#include "stm32f4xx_hal.h"

/*I2C ADDRESSES , LAST BIT IS READ/WRITE BIT SO ADDRESS IS LEFT SHIFTED BY ONE TO MAKE SURE LAST IS FREE
 *IF ASEL IS LOW I2C ADDRESS IS 0X1D AND IF ASEL IS HIGH I2C ADDRESS IS 0X53*/
#define I2C_ADDRESS_LOW		(0x1D << 1)		//DATASHEET PAGE 26
#define I2C_ADDRESS_HIGH	(0x53 << 1)

/*TO BE SURE COMMUNICATION IS WORKING USE DEVICE ID MEMSID AND PARTID*/
#define DEVICE_ID	0xAD	//DATASHEET PAGE 32
#define MEMS_ID		0x1D
#define PART_ID		0xED


/*REGISTER MAP DATASHEET PAGE 31*/
#define DEVID_AD		0x00
#define DEVID_MST		0x01
#define PARTID			0x02
#define REVID			0x03
#define STATUS			0x04
#define FIFO_ENTRIES	0x05
#define TEMP2			0x06
#define TEMP1			0x07
#define XDATA3			0x08
#define XDATA2			0x09
#define XDATA1			0x0A
#define YDATA3			0x0B
#define YDATA2			0x0C
#define YDATA1			0x0D
#define ZDATA3			0x0E
#define ZDATA2			0x0F
#define ZDATA1			0x10
#define FIFO_DATA		0x11
#define OFFSET_X_H		0x1E
#define OFFSET_X_L		0x1F
#define OFFSET_Y_H		0x20
#define OFFSET_Y_L		0x21
#define OFFSET_Z_H		0x22
#define OFFSET_Z_L		0x23
#define ACT_EN			0x24
#define ACT_THRESH_H	0x25
#define ACT_THRESH_L	0x26
#define ACT_COUNT		0x27
#define FILTER			0x28
#define FIFO_SAMPLES	0x29
#define INT_MAP			0x2A
#define SYNC			0x2B
#define RANGE			0x2C
#define POWER_CTL		0x2D
#define SELF_TEST		0x2E
#define RESET			0x2F


typedef struct{
	I2C_HandleTypeDef *i2cHandle;

	/*ACCELERATION DATA X Y Z */
	float acceleration_x, acceleration_y, acceleration_z; //NOT RAW DATA

	/*TEMPERATURE DATA*/
	float temperature;	//NOT ROW DATA

}ADXL355;



/*LOW LEVEL REGISTER FUNCTIONS*/
//PARAMS: STRUCT; ADDRESS OF REGISTER AND ADDRESS OF DATA OUT ; READ SINGLE BYTE
HAL_StatusTypeDef Read_Register(ADXL355 *dev, uint8_t reg, uint8_t *data); //RETURNS STATUS - OK, ERROR, BUSY OR TIMEOUT
//READ MULTIPLE BYTES ; PARAMS: START REGISTER ADDRESS ; ADRESS OF DATA OUT ; NUMBER OF BYTES
HAL_StatusTypeDef ReadRegisters(ADXL355 *dev, uint8_t reg, uint8_t *data, uint8_t length);
//WRITE SINGLE BYTE TO REGISTER ;
HAL_StatusTypeDef Write_Register(ADXL355 *dev, uint8_t reg, uint8_t *data);
//WRITE MULTIPLE BYTES
HAL_StatusTypeDef Write_Registers(ADXL355 *dev, uint8_t reg, uint8_t *data, uint8_t lenght);


/*READ DATA*/
HAL_StatusTypeDef ReadTemperature( ADXL355 *dev);
HAL_StatusTypeDef ReadAcceleration( ADXL355 *dev);

/*INITIALIZATION*/
/*PARAMS: POINTER TO STRUCT - POINTER TO I2CHANDLE THAT USED
 *CHECK IF COMMUNICATION WORKS CORRETLY ; SET UP SENSOR ; RETURNS ERROR CODE */
uint8_t ADXL355_Init(ADXL355 *dev, I2C_HandleTypeDef *i2cHandle);


#endif /* INC_ADXL355_H_ */
