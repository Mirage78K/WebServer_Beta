/*
 * WebUi_String.c
 *
 *  Created on: May 5, 2025
 *      Author: RAYA_4
 */

#include "WebUi_String.h"


/*Must Be sync by ali's Defines*/
const WebUiStringHmi_t WebUiStringHmi =
{
/**************Parameter*************/
    .Parameter.fluidName=
    {
        {"Gas"},
        {"Gas oil"},
        {"Water"}    //Not used
    },

    .Parameter.unitVolume=
    {
        {"m3"},
        {"10^3 * m3"},
        {"10^6 * m3"},
        {"L"},
        {"mL"}
    },

    .Parameter.floweterType=
    {
        {"No Flowmeter"},
        {"Pulse meter"},
        {"Orifice meter"},
        {"Ultrasonic"},
        {"Current meter"}
    },

    .Parameter.unitFlowrate=
    {
        {"m3/h"},
        {"10^3 * m3/h"},
        {"10^6 * m3/h"},
        {"L/h"},
        {"mL/h"},
        {"m3/s"},
    },
/**********************************/
/*************Orifice**************/
    .Orifice.pipeMaterial=
    {
        {"Steel_304"},
        {"Steel_316"},
        {"Steel_304_316"},
        {"Monol"},
        {"Carbon"}
    },

    .Orifice.orificeMaterial=
    {
        {"Steel_304"},
        {"Steel_316"},
        {"Steel_304_316"},
        {"Monol"},
        {"Carbon"}
    },

    .Orifice.tapConfig=
    {
        {"Corner tap"},
        {"Flange tap"},
        {"D-D/2 tap"}
    },

    .Orifice.pressureTap=
    {
        {"Upstream"},
        {"Downstream"}
    },

/*************CurrentFlowMeter**************/
    .CurrentFlowMeter.typeInput=
    {
        {"UVOL Flowrate"},
        {"CVOL Flowrate"},
        {"MASS Flowrate"}
    },

    /*************Meter Correction**************/
    .MeterCorrection.correctionType=
    {
        {"No Correction"},
        {"Qm/Dev"},
        {"Qc/Dev"},
        {"Qm/Qref"}
    },

/*******************************/
/*******************************/
/**************IoDi*************/
    .IoDi.typeContract=
    {
        {"Normal open"},
        {"Normal close"}
    },

    .IoDi.log=
    {
        {"Inactive"},
        {"Active"}
    },

    .IoDi.event=
    {
        {"Inactive"},
        {"Active"}
    },

    .IoDi.typeTrigger=
    {
        {"Rising edge"},
        {"Falling edge"}
    },

    .IoDi.sensevityPickup=
    {
        {"10 mV"},
        {"100 mV"}
    },
/*******************************/
/**************IoAi*************/
    .IoAi.typeInput=
    {
        {"Inactive"},
        {"Pressure"},
        {"Temperature"},
        {"Diff-Pressure"},
        {"Flowrate"},
        {"Density"},
        {"Other"}
    },

    .IoAi.unitInputPressure=
    {
        {"bar"},
        {"mbar"},
        {"Kpa"},
        {"Mpa"},
        {"mmH2O"},
        {"PSIG"}
    },

    .IoAi.unitInputTemp=
    {
		{0xb0,0x43,0x00},	//°C
		{0xb0,0x46,0x00},	//°F
        {"K"}
    },

    .IoAi.unitInputDiffPressure=
    {
        {"mbar"},
        {"pa"},
        {"Kpa"},
        {"mmH2O"},
        {"inH2O"},
        {"bar"}
    },

    .IoAi.unitInputFlowrate=
    {
        {"m3/h"},
        {"10^3 * m3/h"},
        {"10^6 * m3/h"},
        {"m3/s"},
        {"cm3/h"},
        {"cm3/s"}
    },

    .IoAi.unitInputDensity=
    {
        {"Kg/m3"},
        {"g/cm3"}
    },

    .IoAi.pressureTransmitterType=
    {
        {"Gauge"},
        {"Absolute"}
    },

    .IoAi.channelUnit=
    {
        {" "},                  //Inactive
        {"bar"},                //Pressure
        {0xb0,0x43,0x00},       //Temperature
        {"mbar"},               //Diff-Pressure
        {"m3/h"},               //flowrate
        {"Kg/m3"},              //Density
        {" "},                  //Other
    },
/*******************************/
/**************IoHart*************/
    .IoHart.typeInput=
    {
        {"Inactive"},
        {"Pressure"},
        {"Temperature"},
        {"Diff-Pressure"},
        {"Flowrate"},
        {"Density"},
        {"Other"}
    },

    .IoHart.unitInputPressure=
    {
        {"bar"},
        {"mbar"},
        {"Kpa"},
        {"Mpa"},
        {"mmH2O"},
        {"PSIG"}
    },

    .IoHart.unitInputTemp=
    {
        {0xb0,0x43,0x00},	//°C
        {0xb0,0x46,0x00},	//°F
        {"K"}
    },

    .IoHart.unitInputDiffPressure=
    {
        {"mbar"},
        {"pa"},
        {"Kpa"},
        {"mmH2O"},
        {"inH2O"},
        {"bar"},
    },

    .IoHart.unitInputFlowrate=
    {
        {"m3/h"},
        {"10^3 * m3/h"},
        {"10^6 * m3/h"},
        {"m3/s"},
        {"cm3/h"},
        {"cm3/s"}
    },

    .IoHart.unitInputDensity=
    {
        {"Kg/m3"},
        {"g/cm3"}
    },

    .IoHart.pressureTransmitterType=
    {
        {"Gauge"},
        {"Absolute"}
    },

    .IoHart.channelUnit=
    {
        {" "},                  //Inactive
        {"bar"},                //Pressure
        {0xb0,0x43,0x00},       //Temperature
        {"mbar"},               //Diff-Pressure
        {"m3/h"},               //flowrate
        {"Kg/m3"},              //Density
        {" "},                  //Other
    },

/*******************************/
/**************IoAo*************/
    .IoAo.typeOutut=
    {
        {"Inactive"},
        {"Voltage"},
        {"Current"},
    },

    .IoAo.output=
    {
        {"Keypad value"},
        {"Station"},
        {"Channel"},
        {"Cvol Flowrate"},
        {"Uvol flowrate"},
        {"Mass flowrate"},
        {"Energy"},
        {"Density"}
    },
/*******************************/
/**************IoDo*************/
    .IoDo.typeContact=
    {
        {"Normal open"},
        {"Normal close"},
    },

    .IoDo.outputType=
    {
        {"CVOL"},
        {"UVOL"},
        {"Mass"},
        {"Energy"},
        {"CVOL fail"},
        {"UVOL fail"},
        {"Mass fail"},
        {"Energy fail"},
        {"Station"},
    },

    .IoDo.alarmType=
    {
        {"Temp case alarm"},
        {"Stream1 alarm"},
        {"Stream2 alarm"},
        {"Stream3 alarm"},
        {"System alarm"},
    },

    .IoDo.initialState=
    {
        {"Inactive"},
        {"Active"},
    },

    .IoDo.outputDrive=
    {
        {"Automate"},
        {"Manual"},
    },

    .IoDo.unitStrokeVolume=
    {
        {"m^3"},
        {"Nm^3"},
        {"Kg"},
        {"Gj"},
        {"m^3"},
        {"Nm^3"},
        {"Kg"},
        {"Gj"},
        {""},
    },
/*******************************/
/**************IoModbusCom*************/
.IoModbusCom.baudrate=
{
    {"1200"},
    {"2400"},
    {"4800"},
    {"9600"},
    {"19200"},
    {"38400"},
    {"57600"},
    {"115200"},
},

.IoModbusCom.driveMode=
{
    {"RS485"},
    {"RS422"},
},

.IoModbusCom.dataConfig=
{
    {"8N1"},
    {"8N2"},
    {"8E1"},
    {"8O1"},
},

.IoModbusCom.modbusType=
{
    {"RTU"},
    {"ASCII"},
},

.IoModbusCom.function=
{
    {"Inactive"},
    {"Modbus Slave"},
    {"USM"},
    {"GC"},
    {"Modbus Master"},
},

.IoModbusCom.deviceTypeUSM=
{
    {""},
},

.IoModbusCom.deviceTypeGC=
{
    {""},
},
/*******************************/
/**************IoModbusMasterChannel*************/
.IoModbusMasterChannel.selectCom=
{
    {"Inactive"},
    {"COM 1"},
    {"COM 2"},
    {"COM 3"},
},

.IoModbusMasterChannel.functionRead=
{
    {"Read Coil"},
    {"Read Discrete Input"},
    {"Read Holding Registers"},
    {"Read Input Registers"},
},

.IoModbusMasterChannel.typeOfData=
{
    {"coil"},
    {"int_16"},
    {"uint_16"},
    {"float(Big endian)"},
    {"float(Little endian)"},
    {"float(Big endian byte swap)"},
    {"float(Little endian byte swap)"},
    {"int_32(Big endian)"},
    {"int_32(Little endian)"},
    {"int_32(Big endian byte swap)"},
    {"int_32(Little endian byte swap) "},
    {"uint_32(Big endian)"},
    {"uint_32(Little endian)"},
    {"uint_32(Big endian byte swap)"},
    {"uint_32(Little endian byte swap)"},
    {"double(Big endian)"},
    {"double(Little endian)"},
    {"double(Big endian byte swap)"},
    {"double(Little endian byte swap)"},
    {"int_64(Big endian)"},
    {"int_64(Little endian)"},
    {"int_64(Big endian byte swap)"},
    {"int_64(Little endian byte swap)"},
    {"uint_64(Big endian)"},
    {"uint_64(Little endian)"},
    {"uint_64(Big endian byte swap)"},
    {"uint_64(Little endian byte swap)"},
},
/*******************************/
};


