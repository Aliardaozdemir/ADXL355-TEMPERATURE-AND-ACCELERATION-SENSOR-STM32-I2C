#include "ADXL355.h"


uint8_t ADXL355_Init(ADXL355 *dev, I2C_HandleTypeDef *i2cHandle){

	/*INIT STRUCT*/
	dev->acceleration_x = 0.0f;
	dev->acceleration_y = 0.0f;
	dev->acceleration_z = 0.0f;
	dev->temperature = 0.0f;

	dev->i2cHandle = i2cHandle;

	uint8_t error_number = 0;
	HAL_StatusTypeDef status;

	/*check device id , memsid , partid*/

	uint8_t register_data; //to store resulting register data


	//CHEKC DEVICE ID
	status = ReadRegister(dev, DEVID_AD, &register_data);

	//IF STATUS IS NOT HAL OKEY ADD 1 TO THE ERROR NUMBER
	error_number += (status != HAL_OK);

	if(register_data != DEVICE_ID){
		return 255;
	}

	//CHECK MEMS ID

	status = ReadRegister(dev, DEVID_MST, &register_data);

	error_number += (status != HAL_OK);

	if(register_data != MEMS_ID){
		255;
	}

	//CHECK PARTID

	status = ReadRegister(dev, PARTID, &register_data);

	error_number += (status != HAL_OK);

	if(register_data != PART_ID){
		return 255;
	}

	//FILTER SETTINGS DATASHEET PAGE 37
	//		7 BITS - LAST BIT RESERVED
	//		BITS [6:4]		000 : NO HIGH PASS FILTER ENABLED
	//		BITS [3:0]		0000 : 4000HZ & 1000HZ ; 0001: 2000HZ & 500HZ ; 0010: 1000HZ & 250HZ ; 0011: 500HZ & 125HZ
	// 						0100: 250HZ & 62.5HZ ; 0101: 125HZ & 31.25HZ; 0110: 62.5HZ & 15.625HZ ; 0111: 31.25HZ & 7.813HZ

	// SET UP 0 000 0110 = 0X6		NO HIGH PASS FILTER ENABLED 62.5HZ AND 15.625HZ

	register_data = 0x06;

	status = WriteRegister(dev, FILTER , &register_data);

	error_number += (status != HAL_OK);

	//SET CONTROL REGISTER 00000000(0x0) TO MEASUREMENT MODE

	register_data = 0x0;

	status = WriteRegister(dev, POWER_CTL, &reg_data);

	error_number += (status != HAL_OK);

	return error_number;

}


//READ TEMPERATURE				TEMP2 REGISTER FIRST 4 BITS IS USELESS FOR READING TEMPERATURE --> USE: TEMP2 BITWISE AND 00001111 ( 0X0F)
// TEMP2 REGISTER VALUES HIGH VALUES OF DATA ADN TEMP1 IS LOW VALUES --> USE: (TEMP2 << 8) BITWISE OR TEMP1 TO GET UINT16_T VALUE

HAL_StatusTypeDef ReadTemperature(ADXL355 *dev){

	//read raw values from two 8 bit registers
	uint8_t registerData[2];

	HAL_StatusTypeDef status = ReadRegisters(dev, TEMP2, regData, 2);

	//combine register values to get raw data

	uint16_t temperature_raw = ( ( (registerData[0] & 0x0F) << 8 )  | registerData[1] );

	//convert raw value to celcius

	dev->temperature = -0.11049624756f * ((float) temperature_raw - 1852.0f) + 25.0f;

	return status;
}


//READ ACCELERATION X Y Z
// FOR EACH AXIS THERE IS THREE REGISTERS (24 BITS FOR EACH AXIS , ONLY 20 BITS IMPORTANT FOR READING DATA )
//DATA IS LEFT JUSTIFIED ADN FORMATTED AS TWOS COMPLEMENT : DATASHEET PAGE 33
//20 BITS FOR EACH AXIS SO WE NEED SIGNED 32 BIT VALUE TO STORE AXIS3 MOST SIGNIFICANT BITS AXIS1 IS LEAST SIGNIFICANT AND LSB 3 BITS HAS TO MASKED
//COMBINED VALUE IS 32 BITS , VALUE IS LEFT JUSTIFIED 20 BITS SO RIGT SHIFT BY 12 COMBINED VALUE TO GET 20 BIT LEFT JUSTIFIED VALUE

HAL_StatusTypeDef ReadAcceleration(ADXL355 *dev){

	//READ RAW VALUES FROM 9 ACCELERATION REGISTERS
	uint8_t registerData[9];

	//READ VALUES STARTING FROM REGISTER XDATA3
	HAL_StatusTypeDef status = ReadRegisters(dev, XDATA3, registerData, 9 );

	int32_t accelerationsRaw[3];

	accelerationsRaw[0] = (((int32_t) registerData[0] << 24) | ((int32_t) registerData[1] << 16) | ((int32_t) (registerData[2] & 0xF0) << 8) >> 12); // x axis
	accelerationsRaw[1] = (((int32_t) registerData[3] << 24) | ((int32_t) registerData[4] << 16) | ((int32_t) (registerData[5] & 0xF0) << 8) >> 12); // y axis
	accelerationsRaw[2] = (((int32_t) registerData[6] << 24) | ((int32_t) registerData[7] << 16) | ((int32_t) (registerData[8] & 0xF0) << 8) >> 12); // z axis

	//SENSITIVITY = RANGE / NUMBER OF BITS
	dev->acceleration_x = 9.81f * 0.00000390625f * accelerationsRaw[0];
	dev->acceleration_y = 9.81f * 0.00000390625f * accelerationsRaw[1];
	dev->acceleration_z = 9.81f * 0.00000390625f * accelerationsRaw[2];

	return status;
}















/*LOW LEVEL REGISTER FUNCTIONS*/

//PARAMS: STRUCT; ADDRESS OF REGISTER AND ADDRESS OF DATA OUT ; READ SINGLE BYTE
HAL_StatusTypeDef Read_Register(ADXL355 *dev, uint8_t reg, uint8_t *data){

	//PARAMS: I2C INSTANCE ; DEVADDRESS ; MEMADDRESS ; MEMADDRESSSIZE ; *DATA ; SIZE ; TIMEOUT
	return HAL_I2C_Mem_Read(dev-> i2cHandle, I2C_ADDRESS_LOW, reg, I2C_MEMADD_SIZE_8BIT, data, 1 , HAL_MAX_DELAY);
}
//READ MULTIPLE BYTES ; PARAMS: START REGISTER ADDRESS ; ADRESS OF DATA OUT ; NUMBER OF BYTES
HAL_StatusTypeDef ReadRegisters(ADXL355 *dev, uint8_t reg, uint8_t *data, uint8_t length){
	return HAL_I2C_Mem_Read(dev-> i2cHandle, I2C_ADDRESS_LOW, reg, I2C_MEMADD_SIZE_8BIT, data, length , HAL_MAX_DELAY);
}
//WRITE SINGLE BYTE TO REGISTER ;
HAL_StatusTypeDef Write_Register(ADXL355 *dev, uint8_t reg, uint8_t *data){
	return HAL_I2C_Mem_Write(dev->i2cHandle, I2C_ADDRESS_LOW, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}
//WRITE MULTIPLE BYTES
HAL_StatusTypeDef Write_Registers(ADXL355 *dev, uint8_t reg, uint8_t *data, uint8_t lenght){
	return HAL_I2C_Mem_Write(dev->i2cHandle, I2C_ADDRESS_LOW, reg, I2C_MEMADD_SIZE_8BIT, data, lenght, HAL_MAX_DELAY);
}



