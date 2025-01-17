/*
	ATT_IOT.cpp - SmartLiving.io LoRa Arduino library 
*/

#include "ATT_LoRa_IOT.h"
//#include <arduino.h>


//create the object
ATTDevice::ATTDevice(LoRaModem* modem): _maxRetries(SEND_MAX_RETRY)
{
	_modem = modem;
	_lastTimeSent = 0;
}

//connect with the to the lora gateway
bool ATTDevice::Connect(unsigned char* devAddress, unsigned char* appKey, unsigned char*  nwksKey, bool adr)
{
	_modem->Stop();								//stop any previously running modems
	_modem->SetLoRaWan(adr);						//switch to LoRaWan mode instead of peer to peer
	_modem->SetDevAddress(devAddress);
	_modem->SetAppKey(appKey);
	_modem->SetNWKSKey(nwksKey);
	bool result = _modem->Start();								//start the modem up 
	if(result == true)
		Serial.println("successfully connected");
	else
		Serial.println("failed to connect to LoRa");
	return result;									//we have created a connection succesfully.
}

//check for any incoming data
void ATTDevice::Process()
{
}


//send a data value to the cloud server for the sensor with the specified id.
void ATTDevice::Send(String value, short id, bool ack)
{
	_data.Add(value);
	Send(id, ack);
}

void ATTDevice::Send(bool value, short id, bool ack)
{
	_data.Add(value);
	Send(id, ack);
}

void ATTDevice::Send(short value, short id, bool ack)
{
	_data.Add(value);
	Send(id, ack);
}

void ATTDevice::Send(float value, short id, bool ack)
{
	_data.Add(value);
	Send(id, ack);
}

//sends the previously built complex data packet to the cloud for the sensor with the specified
void ATTDevice::Send(short id, bool ack)
{
	_data.SetId(id);
	short nrRetries = 0;
	unsigned long curTime = millis();
	if(_lastTimeSent != 0 && _lastTimeSent + MIN_TIME_BETWEEN_SEND > curTime)
	{
		Serial.println("pausing between 2 consecutive messages...");
		Serial.print("curTime = "); Serial.print(curTime); Serial.print(", prevTime = "); Serial.print(_lastTimeSent); Serial.print(", dif = ");
		Serial.println(MIN_TIME_BETWEEN_SEND + _lastTimeSent - curTime);
		delay(MIN_TIME_BETWEEN_SEND + _lastTimeSent - curTime);
	}
	while(_modem->Send(&_data, ack) == false && (nrRetries < _maxRetries || _maxRetries == -1)) 
	{
		nrRetries++;
		delay(2000);
		Serial.println("resending");
	}
	_data.Reset();				//make certain packet doesn't contain any values any more for the next run. This allows us to easily build up partials as well
	_lastTimeSent = millis();
}

//loads a bool data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(bool value)
{
	_data.Add(value);
}

//loads a bool data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(short value)
{
	_data.Add(value);
}

//loads a string data value into the data packet that is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(String value)
{
	_data.Add(value);
}

//loads a float data value into the data packet tha is being prepared to send to the
//cloud server.
//the packet is sent after calling Send(id_of_sensor)
void ATTDevice::Queue(float value)
{
	_data.Add(value);
}


