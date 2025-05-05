/*
 * WebUi_String.h
 *
 *  Created on: May 5, 2025
 *      Author: RAYA_4
 */

#ifndef WEBSTRING_WEBUI_STRING_H_
#define WEBSTRING_WEBUI_STRING_H_


#define WEB_UI_STRING_MAX_SIZE		20
#define WEB_UI_STRING_MAX_PRAM_NUM	10


#define WEB_UI_STRING_MODBUS_MASTER_CHANNEL_SIZE        40  //very long
#define WEB_UI_STRING_MODBUS_MASTER_CHANNEL_PRAM_NUM    30  //very long

typedef struct
{
    struct
    {
        char fluidName[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitVolume[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char floweterType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitFlowrate[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }Parameter;

    struct
    {
        char orificeMaterial[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char pipeMaterial[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char tapConfig[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char pressureTap[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }Orifice;

    struct
    {
        char typeInput[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }CurrentFlowMeter;   //Current Flowmeter

    struct
    {
        char correctionType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }MeterCorrection;



    struct
    {
        char typeContract[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char log[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char event[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char typeTrigger[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char sensevityPickup[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoDi;

    struct
    {
        char typeInput[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputPressure[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputTemp[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputDiffPressure[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputFlowrate[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputDensity[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char pressureTransmitterType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char channelUnit[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoAi;

    struct
    {
        char typeInput[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputPressure[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputTemp[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputDiffPressure[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputFlowrate[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitInputDensity[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char pressureTransmitterType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char channelUnit[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoHart;

    struct
    {
        char typeOutut[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char output[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoAo;

    struct
    {
        char typeContact[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char outputType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char alarmType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char initialState[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char outputDrive[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char unitStrokeVolume[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoDo;

    struct
    {
        char baudrate[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char driveMode[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char dataConfig[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char modbusType[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char function[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char deviceTypeUSM[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
        char deviceTypeGC[WEB_UI_STRING_MAX_PRAM_NUM][WEB_UI_STRING_MAX_SIZE];
    }IoModbusCom;

    struct
    {
        char selectCom[WEB_UI_STRING_MODBUS_MASTER_CHANNEL_PRAM_NUM][WEB_UI_STRING_MODBUS_MASTER_CHANNEL_SIZE];
        char functionRead[WEB_UI_STRING_MODBUS_MASTER_CHANNEL_PRAM_NUM][WEB_UI_STRING_MODBUS_MASTER_CHANNEL_SIZE];
        char typeOfData[WEB_UI_STRING_MODBUS_MASTER_CHANNEL_PRAM_NUM][WEB_UI_STRING_MODBUS_MASTER_CHANNEL_SIZE];
    }IoModbusMasterChannel;


}WebUiStringHmi_t;


extern const WebUiStringHmi_t WebUiStringHmi;

#endif /* WEBSTRING_WEBUI_STRING_H_ */
