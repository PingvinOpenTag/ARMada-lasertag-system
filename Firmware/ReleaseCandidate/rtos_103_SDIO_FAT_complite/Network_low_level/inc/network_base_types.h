#ifndef NETWORK_BASE_TYPES_H
#define NETWORK_BASE_TYPES_H

#include "types.h"
#ifdef SI4432_ENABLE

#include<stdio.h>
#include"stdint.h"

#ifndef bool

                        #define bool unsigned char

                        #define true 1

                        #define false 0

#endif

typedef uint16_t  tPackageId;

struct DeviceAddress
{
	uint8_t address[3];
};

typedef struct DeviceAddress tDeviceAddress;

#pragma pack(push, 1)
struct PackageDetails
{
	tPackageId packageId;
	uint8_t TTL : 7;
uint8_t needAck : 1;

};
#pragma pack(pop)

typedef struct PackageDetails tPackageDetails;

#define payloadLength 23

#pragma pack(push, 1)
struct Package
{
	tDeviceAddress sender;
	tDeviceAddress target;
	tPackageDetails details;
	uint8_t payload[payloadLength];

};
#pragma pack(pop)

typedef struct Package tPackage;


#define acknoledgementCode 0xFFFF
#pragma pack(push, 1)
	struct AckPayload
	{
		//constexpr static uint16_t acknoledgementCode = 0xFFFF;
		uint8_t payload_size;// = 0x02;// sizeof(uint16_t);
		uint16_t operationCode;// = acknoledgementCode;
		uint16_t packageId;
//		bool isAck;//() { return operationCode == acknoledgementCode; }
	};
#pragma pack(pop)

typedef struct AckPayload tAckPayload;




#define defaultTimeout 10000000
#define defaultResendTime 200000
#define  defaultResendTimeDelta 100000
struct PackageTimings
{
//	constexpr static uint32_t defaultTimeout = 10000000;
//	constexpr static uint32_t defaultResendTime = 200000;
//	constexpr static uint32_t defaultResendTimeDelta = 100000;
/*
	PackageTimings(
			bool _infiniteResend = false,
			uint32_t _timeout = defaultTimeout,
			uint32_t _resendTime = defaultResendTime,
			uint32_t _resendTimeDelta = defaultResendTimeDelta
			) : infiniteResend(_infiniteResend), timeout(_timeout), resendTime(_resendTime), resendTimeDelta(_resendTimeDelta)
	{ }
*/
	bool infiniteResend;
	uint32_t timeout;
	uint32_t resendTime;
	uint32_t resendTimeDelta;
};

typedef struct PackageTimings tPackageTimings;


typedef void (*tPackageSendingDoneCallback)(uint16_t packageId);

#pragma pack(push, 1)
struct LastReceivedPackagesItem
{
	 tPackageId id;
	 tDeviceAddress sender;
	 uint32_t removalTime;
};
#pragma pack(pop)

typedef struct LastReceivedPackagesItem tLastReceivedPackagesItem;


struct WaitingPackage
	{
		uint32_t wasCreated;
		uint32_t nextTransmission;
		bool isBroadcast;// = false;
		bool isDeliveryComplite;
		tPackageTimings timings;
		//PackageTimings timings;
		/*
		uint32_t timeout = 0;
		uint32_t resendTime = 0;
		uint32_t resendTimeDelta = 0;
		*/
		tPackageSendingDoneCallback callback;
//		void (*callback)(uint16_t packageId);
		tPackage package;
};

typedef struct WaitingPackage tWaitingPackage;

#endif //[#ifdef SI4432_ENABLE]
#endif
