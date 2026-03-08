/**
 *
 *  Copyright (C) 2026 Roman Pauer
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 */

#ifdef __cplusplus
#define REF(x) x
#else
#define REF(x) &(x)
typedef int8_t bool;
#endif

typedef enum {
    Platform_x64 = 1,
//    Platform_Arm64 = 2,
//    Platform_Arm64EC = 3,
    Platform_Arm64X = 4,
} MidiAppSDKPlatform;

enum Midi1PortFlow {
    Midi1PortFlow_MidiMessageSource = 0,          // MIDI Flow In / Input
    Midi1PortFlow_MidiMessageDestination = 1,     // MIDI Flow Out / Output
};

enum Midi1PortNamingApproach {
    Midi1PortNamingApproach_Default = 0,
    Midi1PortNamingApproach_UseClassicCompatible = 1,
    Midi1PortNamingApproach_UseNewStyle = 2,
};

enum MidiEndpointDeviceInformationFilters {
    MidiEndpointDeviceInformationFilters_StandardNativeUniversalMidiPacketFormat =   0x00000001,     // a normal MIDI 2.0 UMP endpoint
    MidiEndpointDeviceInformationFilters_StandardNativeMidi1ByteFormat =             0x00000002,     // a normal MIDI 1.0 endpoint upscaled to UMP by service or driver

    MidiEndpointDeviceInformationFilters_VirtualDeviceResponder =                    0x00000100,     // The app-side of a Virtual Device. Not normally presented to other apps.

    MidiEndpointDeviceInformationFilters_DiagnosticLoopback =                        0x00010000,     // Diagnostics loopback. Don't show this in typical apps.
    MidiEndpointDeviceInformationFilters_DiagnosticPing =                            0x00020000,     // Internal ping. Don't show this in apps. For internal use.

    MidiEndpointDeviceInformationFilters_AllStandardEndpoints = 0x3,
};

enum MidiEndpointDeviceInformationSortOrder {
    MidiEndpointDeviceInformationSortOrder_None =                                      0,

    MidiEndpointDeviceInformationSortOrder_Name =                                      1,
    MidiEndpointDeviceInformationSortOrder_EndpointDeviceId =                          2,
    MidiEndpointDeviceInformationSortOrder_DeviceInstanceId =                          3,

    MidiEndpointDeviceInformationSortOrder_ContainerThenName =                         11,
    MidiEndpointDeviceInformationSortOrder_ContainerThenEndpointDeviceId =             12,
    MidiEndpointDeviceInformationSortOrder_ContainerThenDeviceInstanceId =             13,

    MidiEndpointDeviceInformationSortOrder_TransportCodeThenName =                     21,
    MidiEndpointDeviceInformationSortOrder_TransportCodeThenEndpointDeviceId =         22,
    MidiEndpointDeviceInformationSortOrder_TransportCodeThenDeviceInstanceId =         23,
};

enum MidiEndpointDevicePurpose {
    MidiEndpointDevicePurpose_NormalMessageEndpoint =     0,

    MidiEndpointDevicePurpose_VirtualDeviceResponder =    100,

    MidiEndpointDevicePurpose_InBoxGeneralMidiSynth =     400,

    MidiEndpointDevicePurpose_DiagnosticLoopback =        500,
    MidiEndpointDevicePurpose_DiagnosticPing =            510,
};

enum MidiEndpointNativeDataFormat {
    MidiEndpointNativeDataFormat_Unknown =                   0,
    MidiEndpointNativeDataFormat_Midi1ByteFormat =           1,
    MidiEndpointNativeDataFormat_UniversalMidiPacketFormat = 2,
};

enum MidiFunctionBlockDirection {
    MidiFunctionBlockDirection_Undefined = 0x0,
    MidiFunctionBlockDirection_BlockInput = 0x1,
    MidiFunctionBlockDirection_BlockOutput = 0x2,
    MidiFunctionBlockDirection_Bidirectional = 0x3
};

enum MidiFunctionBlockRepresentsMidi10Connection {
    MidiFunctionBlockRepresentsMidi10Connection_Not10 =                     0x0,
    MidiFunctionBlockRepresentsMidi10Connection_YesBandwidthUnrestricted =  0x1,
    MidiFunctionBlockRepresentsMidi10Connection_YesBandwidthRestricted =    0x2,
    MidiFunctionBlockRepresentsMidi10Connection_Reserved =                  0x3
};

enum MidiFunctionBlockUIHint {
    MidiFunctionBlockUIHint_Unknown =       0x0,
    MidiFunctionBlockUIHint_Receiver =      0x1,
    MidiFunctionBlockUIHint_Sender =        0x2,
    MidiFunctionBlockUIHint_Bidirectional = 0x3
};

enum MidiGroupTerminalBlockDirection {
    MidiGroupTerminalBlockDirection_Bidirectional = 0x0,
    MidiGroupTerminalBlockDirection_BlockInput =    0x1,
    MidiGroupTerminalBlockDirection_BlockOutput =   0x2,
};

enum MidiGroupTerminalBlockProtocol {
    MidiGroupTerminalBlockProtocol_Unknown =                               0x00,
    MidiGroupTerminalBlockProtocol_Midi1Message64 =                        0x01,
    MidiGroupTerminalBlockProtocol_Midi1Message64WithJitterReduction =     0x02,
    MidiGroupTerminalBlockProtocol_Midi1Message128 =                       0x03,
    MidiGroupTerminalBlockProtocol_Midi1Message128WithJitterReduction =    0x04,
    MidiGroupTerminalBlockProtocol_Midi2 =                                 0x11,
    MidiGroupTerminalBlockProtocol_Midi2WithJitterReduction =              0x12
};

enum MidiMessageType {
    MidiMessageType_UtilityMessage32 =      0x0,
    MidiMessageType_SystemCommon32 =        0x1,
    MidiMessageType_Midi1ChannelVoice32 =   0x2,
    MidiMessageType_DataMessage64 =         0x3,
    MidiMessageType_Midi2ChannelVoice64 =   0x4,
    MidiMessageType_DataMessage128 =        0x5,
    MidiMessageType_FutureReserved632 =     0x6,
    MidiMessageType_FutureReserved732 =     0x7,
    MidiMessageType_FutureReserved864 =     0x8,
    MidiMessageType_FutureReserved964 =     0x9,
    MidiMessageType_FutureReservedA64 =     0xA,
    MidiMessageType_FutureReservedB96 =     0xB,
    MidiMessageType_FutureReservedC96 =     0xC,
    MidiMessageType_FlexData128 =           0xD,
    MidiMessageType_FutureReservedE128 =    0xE,
    MidiMessageType_Stream128 =             0xF
};

enum MidiProtocol {
    MidiProtocol_Default =   0,
    MidiProtocol_Midi1 =     1,
    MidiProtocol_Midi2 =     2,
};

enum MidiSystemExclusive8Status {
    MidiSystemExclusive8Status_CompleteMessageInSingleMessagePacket =  0x0,
    MidiSystemExclusive8Status_StartMessagePacket =                    0x1,
    MidiSystemExclusive8Status_ContinueMessagePacket =                 0x2,
    MidiSystemExclusive8Status_EndMessagePacket =                      0x3,
};

struct MidiDeclaredDeviceIdentity {
    uint8_t SystemExclusiveIdByte1;   // if this value is zero, the actual identity is in bytes 2 and 3
    uint8_t SystemExclusiveIdByte2;
    uint8_t SystemExclusiveIdByte3;

    uint8_t DeviceFamilyLsb;
    uint8_t DeviceFamilyMsb;

    uint8_t DeviceFamilyModelNumberLsb;
    uint8_t DeviceFamilyModelNumberMsb;

    uint8_t SoftwareRevisionLevelByte1;
    uint8_t SoftwareRevisionLevelByte2;
    uint8_t SoftwareRevisionLevelByte3;
    uint8_t SoftwareRevisionLevelByte4;
};

struct MidiDeclaredEndpointInfo {
    HSTRING Name;
    HSTRING ProductInstanceId;


    bool SupportsMidi10Protocol;
    bool SupportsMidi20Protocol;
    bool SupportsReceivingJitterReductionTimestamps;
    bool SupportsSendingJitterReductionTimestamps;

    bool HasStaticFunctionBlocks;
    uint8_t DeclaredFunctionBlockCount;

    uint8_t SpecificationVersionMajor;
    uint8_t SpecificationVersionMinor;
};

struct MidiDeclaredStreamConfiguration {
    enum MidiProtocol Protocol;
    bool ReceiveJitterReductionTimestamps;
    bool SendJitterReductionTimestamps;
};

struct MidiEndpointTransportSuppliedInfo {
    HSTRING Name;
    HSTRING Description;

    HSTRING SerialNumber;

    uint16_t VendorId;
    uint16_t ProductId;

    HSTRING ManufacturerName;

    bool SupportsMultiClient;

    enum MidiEndpointNativeDataFormat NativeDataFormat;

    GUID TransportId;
    HSTRING TransportCode;
};

struct MidiEndpointUserSuppliedInfo {
    HSTRING Name;
    HSTRING Description;

    HSTRING ImageFileName;

    bool RequiresNoteOffTranslation;
    uint16_t RecommendedControlChangeAutomationIntervalMilliseconds;
    bool SupportsMidiPolyphonicExpression;
};

struct MidiMessageStruct {
    uint32_t Word0;
    uint32_t Word1;
    uint32_t Word2;
    uint32_t Word3;
};

struct MidiSystemTimerSettings {
    uint64_t CurrentIntervalTicks;
    uint64_t MinimumIntervalTicks;
    uint64_t MaximumIntervalTicks;
};

#undef INTERFACE
#define INTERFACE IMidiClientInitializer
DECLARE_INTERFACE_(IMidiClientInitializer, IUnknown) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IMidiClientInitializer methods
    STDMETHOD(GetInstalledWindowsMidiServicesSdkVersion)(THIS_ MidiAppSDKPlatform *buildPlatform, USHORT *versionMajor, USHORT *versionMinor, USHORT *versionPatch, LPWSTR *buildSource, LPWSTR *versionName, LPWSTR *versionFullString) PURE;
    STDMETHOD(EnsureServiceAvailable)(THIS) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiClientInitializer_Release(This) (This)->Release()
    #define IMidiClientInitializer_EnsureServiceAvailable(This) (This)->EnsureServiceAvailable()
#else
    #define IMidiClientInitializer_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiClientInitializer_EnsureServiceAvailable(This) (This)->lpVtbl->EnsureServiceAvailable(This)
#endif
};

#undef INTERFACE
#define INTERFACE ITypedEventHandler
DECLARE_INTERFACE_(ITypedEventHandler, IUnknown) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // ITypedEventHandler methods
    STDMETHOD(Invoke)(THIS_ void *sender, void *args) PURE;

    END_INTERFACE
};

#undef INTERFACE
#define INTERFACE IVectorView
DECLARE_INTERFACE_(IVectorView, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IVectorView methods
    STDMETHOD(GetAt)(THIS_ uint32_t index, void **item) PURE;
    STDMETHOD(get_Size)(THIS_ uint32_t *size) PURE;
    STDMETHOD(IndexOf)(THIS_ void *value, uint32_t *index, bool *) PURE;
    STDMETHOD(GetMany)(THIS_ uint32_t startIndex, uint32_t items_size, void *items_data, uint32_t *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IVectorView_Release(This) (This)->Release()
    #define IVectorView_GetAt(This,index,item) (This)->GetAt(index,item)
    #define IVectorView_get_Size(This,size) (This)->get_Size(size)
#else
    #define IVectorView_Release(This) (This)->lpVtbl->Release(This)
    #define IVectorView_GetAt(This,index,item) (This)->lpVtbl->GetAt(This,index,item)
    #define IVectorView_get_Size(This,size) (This)->lpVtbl->get_Size(This,size)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiClockStatics
DECLARE_INTERFACE_(IMidiClockStatics, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiClockStatics methods
    STDMETHOD(get_Now)(THIS_ uint64_t *timestampNow) PURE;
    STDMETHOD(get_TimestampConstantSendImmediately)(THIS_ uint64_t *) PURE;
    STDMETHOD(get_TimestampConstantMessageQueueMaximumFutureTicks)(THIS_ uint64_t *) PURE;
    STDMETHOD(get_TimestampFrequency)(THIS_ uint64_t *) PURE;
    STDMETHOD(ConvertTimestampTicksToNanoseconds)(THIS_ uint64_t timestampValue, double *) PURE;
    STDMETHOD(ConvertTimestampTicksToMicroseconds)(THIS_ uint64_t timestampValue, double *) PURE;
    STDMETHOD(ConvertTimestampTicksToMilliseconds)(THIS_ uint64_t timestampValue, double *timeMilliseconds) PURE;
    STDMETHOD(ConvertTimestampTicksToSeconds)(THIS_ uint64_t timestampValue, double *) PURE;
    STDMETHOD(OffsetTimestampByTicks)(THIS_ uint64_t timestampValue, int64_t offsetTicks, uint64_t *) PURE;
    STDMETHOD(OffsetTimestampByMicroseconds)(THIS_ uint64_t timestampValue, int64_t offsetMicroseconds, uint64_t *) PURE;
    STDMETHOD(OffsetTimestampByMilliseconds)(THIS_ uint64_t timestampValue, int64_t offsetMilliseconds, uint64_t *offsetedTimestampValue) PURE;
    STDMETHOD(OffsetTimestampBySeconds)(THIS_ uint64_t timestampValue, int64_t offsetSeconds, uint64_t *) PURE;
    STDMETHOD(GetCurrentSystemTimerInfo)(THIS_ struct MidiSystemTimerSettings *) PURE;
    STDMETHOD(BeginLowLatencySystemTimerPeriod)(THIS_ bool *) PURE;
    STDMETHOD(EndLowLatencySystemTimerPeriod)(THIS_ bool *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiClockStatics_QueryInterface(This,riid,ppvObject) (This)->QueryInterface(riid,ppvObject)
    #define IMidiClockStatics_Release(This) (This)->Release()
    #define IMidiClockStatics_get_Now(This,timestampNow) (This)->get_Now(timestampNow)
    #define IMidiClockStatics_ConvertTimestampTicksToMilliseconds(This,timestampValue,timeMilliseconds) (This)->ConvertTimestampTicksToMilliseconds(timestampValue,timeMilliseconds)
    #define IMidiClockStatics_OffsetTimestampByMilliseconds(This,timestampValue,offsetMilliseconds,offsetedTimestampValue) (This)->OffsetTimestampByMilliseconds(timestampValue,offsetMilliseconds,offsetedTimestampValue)
#else
    #define IMidiClockStatics_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
    #define IMidiClockStatics_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiClockStatics_get_Now(This,timestampNow) (This)->lpVtbl->get_Now(This,timestampNow)
    #define IMidiClockStatics_ConvertTimestampTicksToMilliseconds(This,timestampValue,timeMilliseconds) (This)->lpVtbl->ConvertTimestampTicksToMilliseconds(This,timestampValue,timeMilliseconds)
    #define IMidiClockStatics_OffsetTimestampByMilliseconds(This,timestampValue,offsetMilliseconds,offsetedTimestampValue) (This)->lpVtbl->OffsetTimestampByMilliseconds(This,timestampValue,offsetMilliseconds,offsetedTimestampValue)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointConnection
DECLARE_INTERFACE_(IMidiEndpointConnection, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointConnection methods
    STDMETHOD(get_LogMessageDataValidationErrorDetails)(THIS_ bool *) PURE;
    STDMETHOD(put_LogMessageDataValidationErrorDetails)(THIS_ bool value) PURE;
    STDMETHOD(Open)(THIS_ bool *opened) PURE;
    STDMETHOD(get_MessageProcessingPlugins)(THIS_ IVectorView **) PURE;
    STDMETHOD(AddMessageProcessingPlugin)(THIS_ void *plugin) PURE;
    STDMETHOD(RemoveMessageProcessingPlugin)(THIS_ const GUID *id) PURE;
    STDMETHOD(SendSingleMessagePacket)(THIS_ void *message, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageStruct)(THIS_ uint64_t timestamp, uint8_t wordCount, const struct MidiMessageStruct *message, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageWordArray)(THIS_ uint64_t timestamp, uint32_t startIndex, uint8_t wordCount, uint32_t words_size, uint32_t *words_data, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageWords)(THIS_ uint64_t timestamp, uint32_t word0, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageWords2)(THIS_ uint64_t timestamp, uint32_t word0, uint32_t word1, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageWords3)(THIS_ uint64_t timestamp, uint32_t word0, uint32_t word1, uint32_t word2, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageWords4)(THIS_ uint64_t timestamp, uint32_t word0, uint32_t word1, uint32_t word2, uint32_t word3, uint32_t *) PURE;
    STDMETHOD(SendSingleMessageBuffer)(THIS_ uint64_t timestamp, uint32_t byteOffset, uint8_t byteCount, void *buffer, uint32_t *) PURE;
    STDMETHOD(SendMultipleMessagesWordList)(THIS_ uint64_t timestamp, void *words, uint32_t *) PURE;
    STDMETHOD(SendMultipleMessagesWordArray)(THIS_ uint64_t timestamp, uint32_t startIndex, uint32_t wordCount, uint32_t words_size, uint32_t *words_data, uint32_t *results) PURE;
    STDMETHOD(SendMultipleMessagesPacketList)(THIS_ void *messages, uint32_t *) PURE;
    STDMETHOD(SendMultipleMessagesStructList)(THIS_ uint64_t timestamp, void *messages, uint32_t *) PURE;
    STDMETHOD(SendMultipleMessagesStructArray)(THIS_ uint64_t timestamp, uint32_t startIndex, uint32_t messageCount, uint32_t messages_size, struct MidiMessageStruct *messages_data, uint32_t *results) PURE;
    STDMETHOD(SendMultipleMessagesBuffer)(THIS_ uint64_t timestamp, uint32_t byteOffset, uint32_t byteCount, void *buffer, uint32_t *) PURE;
    STDMETHOD(GetSupportedMaxMidiWordsPerTransmission)(THIS_ uint32_t *maxWords) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointConnection_QueryInterface(This,riid,ppvObject) (This)->QueryInterface(riid,ppvObject)
    #define IMidiEndpointConnection_Release(This) (This)->Release()
    #define IMidiEndpointConnection_Open(This,opened) (This)->Open(opened)
    #define IMidiEndpointConnection_SendMultipleMessagesWordArray(This,timestamp,startIndex,wordCount,words_size,words_data,results) (This)->SendMultipleMessagesWordArray(timestamp,startIndex,wordCount,words_size,words_data,results)
    #define IMidiEndpointConnection_GetSupportedMaxMidiWordsPerTransmission(This,maxWords) (This)->GetSupportedMaxMidiWordsPerTransmission(maxWords)
#else
    #define IMidiEndpointConnection_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
    #define IMidiEndpointConnection_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointConnection_Open(This,opened) (This)->lpVtbl->Open(This,opened)
    #define IMidiEndpointConnection_SendMultipleMessagesWordArray(This,timestamp,startIndex,wordCount,words_size,words_data,results) (This)->lpVtbl->SendMultipleMessagesWordArray(This,timestamp,startIndex,wordCount,words_size,words_data,results)
    #define IMidiEndpointConnection_GetSupportedMaxMidiWordsPerTransmission(This,maxWords) (This)->lpVtbl->GetSupportedMaxMidiWordsPerTransmission(This,maxWords)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointConnectionSource
DECLARE_INTERFACE_(IMidiEndpointConnectionSource, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointConnectionSource methods
    STDMETHOD(add_EndpointDeviceDisconnected)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_EndpointDeviceDisconnected)(THIS_ int64_t token) PURE;
    STDMETHOD(add_EndpointDeviceReconnected)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_EndpointDeviceReconnected)(THIS_ int64_t token) PURE;
    STDMETHOD(get_Tag)(THIS_ void **) PURE;
    STDMETHOD(put_Tag)(THIS_ void *value) PURE;
    STDMETHOD(get_ConnectionId)(THIS_ GUID *connectionId) PURE;
    STDMETHOD(get_ConnectedEndpointDeviceId)(THIS_ HSTRING *endpointDeviceId) PURE;
    STDMETHOD(get_Settings)(THIS_ void **) PURE;
    STDMETHOD(get_IsOpen)(THIS_ bool *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointConnectionSource_Release(This) (This)->Release()
    #define IMidiEndpointConnectionSource_get_ConnectionId(This,connectionId) (This)->get_ConnectionId(connectionId)
    #define IMidiEndpointConnectionSource_get_ConnectedEndpointDeviceId(This,endpointDeviceId) (This)->get_ConnectedEndpointDeviceId(endpointDeviceId)
#else
    #define IMidiEndpointConnectionSource_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointConnectionSource_get_ConnectionId(This,connectionId) (This)->lpVtbl->get_ConnectionId(This,connectionId)
    #define IMidiEndpointConnectionSource_get_ConnectedEndpointDeviceId(This,endpointDeviceId) (This)->lpVtbl->get_ConnectedEndpointDeviceId(This,endpointDeviceId)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceInformation
DECLARE_INTERFACE_(IMidiEndpointDeviceInformation, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceInformation methods
    STDMETHOD(get_EndpointDeviceId)(THIS_ HSTRING *endpointDeviceId) PURE;
    STDMETHOD(get_Name)(THIS_ HSTRING *name) PURE;
    STDMETHOD(get_ContainerId)(THIS_ GUID *) PURE;
    STDMETHOD(get_DeviceInstanceId)(THIS_ HSTRING *) PURE;
    STDMETHOD(get_EndpointPurpose)(THIS_ enum MidiEndpointDevicePurpose *) PURE;
    STDMETHOD(GetDeclaredEndpointInfo)(THIS_ struct MidiDeclaredEndpointInfo *) PURE;
    STDMETHOD(get_DeclaredEndpointInfoLastUpdateTime)(THIS_ int64_t *) PURE;
    STDMETHOD(GetDeclaredDeviceIdentity)(THIS_ struct MidiDeclaredDeviceIdentity *) PURE;
    STDMETHOD(get_DeclaredDeviceIdentityLastUpdateTime)(THIS_ int64_t *) PURE;
    STDMETHOD(GetDeclaredStreamConfiguration)(THIS_ struct MidiDeclaredStreamConfiguration *) PURE;
    STDMETHOD(get_DeclaredStreamConfigurationLastUpdateTime)(THIS_ int64_t *) PURE;
    STDMETHOD(GetDeclaredFunctionBlocks)(THIS_ IVectorView **vectorViewFunctionBlock) PURE;
    STDMETHOD(get_DeclaredFunctionBlocksLastUpdateTime)(THIS_ int64_t *) PURE;
    STDMETHOD(GetGroupTerminalBlocks)(THIS_ IVectorView **vectorViewGroupTerminalBlock) PURE;
    STDMETHOD(GetUserSuppliedInfo)(THIS_ struct MidiEndpointUserSuppliedInfo *) PURE;
    STDMETHOD(GetTransportSuppliedInfo)(THIS_ struct MidiEndpointTransportSuppliedInfo *transportSuppliedInfo) PURE;
    STDMETHOD(GetParentDeviceInformation)(THIS_ void **) PURE;
    STDMETHOD(GetContainerDeviceInformation)(THIS_ void **) PURE;
    STDMETHOD(get_Properties)(THIS_ void **) PURE;
    STDMETHOD(GetNameTable)(THIS_ IVectorView **) PURE;
    STDMETHOD(FindAllAssociatedMidi1PortsForThisEndpoint)(THIS_ enum Midi1PortFlow portFlow, IVectorView **) PURE;
    STDMETHOD(FindAllAssociatedMidi1PortsForThisEndpoint2)(THIS_ enum Midi1PortFlow portFlow, bool useCachedPortInformationIfAvailable, IVectorView **) PURE;
    STDMETHOD(FindAssociatedMidi1PortForGroupForThisEndpoint)(THIS_ void *group, enum Midi1PortFlow portFlow, void **) PURE;
    STDMETHOD(FindAssociatedMidi1PortForGroupForThisEndpoint2)(THIS_ void *group, enum Midi1PortFlow portFlow, bool useCachedPortInformationIfAvailable, void **) PURE;
    STDMETHOD(get_Midi1PortNamingApproach)(THIS_ enum Midi1PortNamingApproach *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceInformation_Release(This) (This)->Release()
    #define IMidiEndpointDeviceInformation_get_EndpointDeviceId(This,endpointDeviceId) (This)->get_EndpointDeviceId(endpointDeviceId)
    #define IMidiEndpointDeviceInformation_get_Name(This,name) (This)->get_Name(name)
    #define IMidiEndpointDeviceInformation_GetDeclaredFunctionBlocks(This,vectorViewFunctionBlock) (This)->GetDeclaredFunctionBlocks(vectorViewFunctionBlock)
    #define IMidiEndpointDeviceInformation_GetGroupTerminalBlocks(This,vectorViewGroupTerminalBlock) (This)->GetGroupTerminalBlocks(vectorViewGroupTerminalBlock)
    #define IMidiEndpointDeviceInformation_GetTransportSuppliedInfo(This,transportSuppliedInfo) (This)->GetTransportSuppliedInfo(transportSuppliedInfo)
#else
    #define IMidiEndpointDeviceInformation_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointDeviceInformation_get_EndpointDeviceId(This,endpointDeviceId) (This)->lpVtbl->get_EndpointDeviceId(This,endpointDeviceId)
    #define IMidiEndpointDeviceInformation_get_Name(This,name) (This)->lpVtbl->get_Name(This,name)
    #define IMidiEndpointDeviceInformation_GetDeclaredFunctionBlocks(This,vectorViewFunctionBlock) (This)->lpVtbl->GetDeclaredFunctionBlocks(This,vectorViewFunctionBlock)
    #define IMidiEndpointDeviceInformation_GetGroupTerminalBlocks(This,vectorViewGroupTerminalBlock) (This)->lpVtbl->GetGroupTerminalBlocks(This,vectorViewGroupTerminalBlock)
    #define IMidiEndpointDeviceInformation_GetTransportSuppliedInfo(This,transportSuppliedInfo) (This)->lpVtbl->GetTransportSuppliedInfo(This,transportSuppliedInfo)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceInformationStatics
DECLARE_INTERFACE_(IMidiEndpointDeviceInformationStatics, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceInformationStatics methods
    STDMETHOD(CreateFromEndpointDeviceId)(THIS_ HSTRING endpointDeviceId, IMidiEndpointDeviceInformation **) PURE;
    STDMETHOD(FindAll)(THIS_ IVectorView **) PURE;
    STDMETHOD(FindAll2)(THIS_ enum MidiEndpointDeviceInformationSortOrder sortOrder, IVectorView **) PURE;
    STDMETHOD(FindAll3)(THIS_ enum MidiEndpointDeviceInformationSortOrder sortOrder, enum MidiEndpointDeviceInformationFilters endpointTypesToInclude, IVectorView **vectorViewEndpointDeviceInformation) PURE;
    STDMETHOD(CreateFromAssociatedMidi1PortDeviceId)(THIS_ HSTRING deviceId, IMidiEndpointDeviceInformation **) PURE;
    STDMETHOD(CreateFromAssociatedMidi1PortNumber)(THIS_ uint32_t portNumber, enum Midi1PortFlow portFlow, IMidiEndpointDeviceInformation **) PURE;
    STDMETHOD(FindEndpointDeviceIdForAssociatedMidi1PortNumber)(THIS_ uint32_t portNumber, enum Midi1PortFlow portFlow, HSTRING *) PURE;
    STDMETHOD(FindAllForAssociatedMidi1PortName)(THIS_ HSTRING portName, enum Midi1PortFlow portFlow, IVectorView **) PURE;
    STDMETHOD(FindAllEndpointDeviceIdsForAssociatedMidi1PortName)(THIS_ HSTRING portName, enum Midi1PortFlow portFlow, IVectorView **) PURE;
    STDMETHOD(get_EndpointInterfaceClass)(THIS_ GUID *) PURE;
    STDMETHOD(GetAdditionalPropertiesList)(THIS_ IVectorView **) PURE;
    STDMETHOD(DeviceMatchesFilter)(THIS_ IMidiEndpointDeviceInformation *deviceInformation, enum MidiEndpointDeviceInformationFilters endpointTypesToInclude, bool *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceInformationStatics_QueryInterface(This,riid,ppvObject) (This)->QueryInterface(riid,ppvObject)
    #define IMidiEndpointDeviceInformationStatics_Release(This) (This)->Release()
    #define IMidiEndpointDeviceInformationStatics_FindAll3(This,sortOrder,endpointTypesToInclude,vectorViewEndpointDeviceInformation) (This)->FindAll3(sortOrder,endpointTypesToInclude,vectorViewEndpointDeviceInformation)
#else
    #define IMidiEndpointDeviceInformationStatics_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
    #define IMidiEndpointDeviceInformationStatics_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointDeviceInformationStatics_FindAll3(This,sortOrder,endpointTypesToInclude,vectorViewEndpointDeviceInformation) (This)->lpVtbl->FindAll3(This,sortOrder,endpointTypesToInclude,vectorViewEndpointDeviceInformation)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceInformationAddedEventArgs
DECLARE_INTERFACE_(IMidiEndpointDeviceInformationAddedEventArgs, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceInformationAddedEventArgs methods
    STDMETHOD(get_AddedDevice)(THIS_ IMidiEndpointDeviceInformation **addedDevice) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceInformationAddedEventArgs_get_AddedDevice(This,addedDevice) (This)->get_AddedDevice(addedDevice)
#else
    #define IMidiEndpointDeviceInformationAddedEventArgs_get_AddedDevice(This,addedDevice) (This)->lpVtbl->get_AddedDevice(This,addedDevice)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceInformationRemovedEventArgs
DECLARE_INTERFACE_(IMidiEndpointDeviceInformationRemovedEventArgs, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceInformationRemovedEventArgs methods
    STDMETHOD(get_EndpointDeviceId)(THIS_ HSTRING *endpointDeviceId) PURE;
    STDMETHOD(get_DeviceInformationUpdate)(THIS_ void **) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceInformationRemovedEventArgs_get_EndpointDeviceId(This,endpointDeviceId) (This)->get_EndpointDeviceId(endpointDeviceId)
#else
    #define IMidiEndpointDeviceInformationRemovedEventArgs_get_EndpointDeviceId(This,endpointDeviceId) (This)->lpVtbl->get_EndpointDeviceId(This,endpointDeviceId)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceInformationUpdatedEventArgs
DECLARE_INTERFACE_(IMidiEndpointDeviceInformationUpdatedEventArgs, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceInformationUpdatedEventArgs methods
    STDMETHOD(get_EndpointDeviceId)(THIS_ HSTRING *endpointDeviceId) PURE;
    STDMETHOD(get_IsNameUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_IsEndpointInformationUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_IsDeviceIdentityUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_IsStreamConfigurationUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_AreFunctionBlocksUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_IsUserMetadataUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_AreAdditionalCapabilitiesUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_AreUniqueIdsUpdated)(THIS_ bool *updated) PURE;
    STDMETHOD(get_DeviceInformationUpdate)(THIS_ void **) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceInformationUpdatedEventArgs_get_EndpointDeviceId(This,endpointDeviceId) (This)->get_EndpointDeviceId(endpointDeviceId)
    #define IMidiEndpointDeviceInformationUpdatedEventArgs_get_IsDeviceIdentityUpdated(This,updated) (This)->get_IsDeviceIdentityUpdated(updated)
#else
    #define IMidiEndpointDeviceInformationUpdatedEventArgs_get_EndpointDeviceId(This,endpointDeviceId) (This)->lpVtbl->get_EndpointDeviceId(This,endpointDeviceId)
    #define IMidiEndpointDeviceInformationUpdatedEventArgs_get_IsDeviceIdentityUpdated(This,updated) (This)->lpVtbl->get_IsDeviceIdentityUpdated(This,updated)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceWatcher
DECLARE_INTERFACE_(IMidiEndpointDeviceWatcher, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceWatcher methods
    STDMETHOD(Start)(THIS) PURE;
    STDMETHOD(Stop)(THIS) PURE;
    STDMETHOD(get_EnumeratedEndpointDevices)(THIS_ void **) PURE;
    STDMETHOD(get_Status)(THIS_ int32_t *) PURE;
    STDMETHOD(add_Added)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_Added)(THIS_ int64_t token) PURE;
    STDMETHOD(add_Removed)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_Removed)(THIS_ int64_t token) PURE;
    STDMETHOD(add_Updated)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_Updated)(THIS_ int64_t token) PURE;
    STDMETHOD(add_EnumerationCompleted)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_EnumerationCompleted)(THIS_ int64_t token) PURE;
    STDMETHOD(add_Stopped)(THIS_ ITypedEventHandler *handler, int64_t *token) PURE;
    STDMETHOD(remove_Stopped)(THIS_ int64_t token) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceWatcher_Release(This) (This)->Release()
    #define IMidiEndpointDeviceWatcher_Start(This) (This)->Start()
    #define IMidiEndpointDeviceWatcher_Stop(This) (This)->Stop()
    #define IMidiEndpointDeviceWatcher_add_Added(This,handler,token) (This)->add_Added(handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Added(This,token) (This)->remove_Added(token)
    #define IMidiEndpointDeviceWatcher_add_Removed(This,handler,token) (This)->add_Removed(handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Removed(This,token) (This)->remove_Removed(token)
    #define IMidiEndpointDeviceWatcher_add_Updated(This,handler,token) (This)->add_Updated(handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Updated(This,token) (This)->remove_Updated(token)
#else
    #define IMidiEndpointDeviceWatcher_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointDeviceWatcher_Start(This) (This)->lpVtbl->Start(This)
    #define IMidiEndpointDeviceWatcher_Stop(This) (This)->lpVtbl->Stop(This)
    #define IMidiEndpointDeviceWatcher_add_Added(This,handler,token) (This)->lpVtbl->add_Added(This,handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Added(This,token) (This)->lpVtbl->remove_Added(This,token)
    #define IMidiEndpointDeviceWatcher_add_Removed(This,handler,token) (This)->lpVtbl->add_Removed(This,handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Removed(This,token) (This)->lpVtbl->remove_Removed(This,token)
    #define IMidiEndpointDeviceWatcher_add_Updated(This,handler,token) (This)->lpVtbl->add_Updated(This,handler,token)
    #define IMidiEndpointDeviceWatcher_remove_Updated(This,token) (This)->lpVtbl->remove_Updated(This,token)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiEndpointDeviceWatcherStatics
DECLARE_INTERFACE_(IMidiEndpointDeviceWatcherStatics, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiEndpointDeviceWatcherStatics methods
    STDMETHOD(Create)(THIS_ IMidiEndpointDeviceWatcher **endpointDeviceWatcher) PURE;
    STDMETHOD(Create2)(THIS_ enum MidiEndpointDeviceInformationFilters endpointFilters, IMidiEndpointDeviceWatcher **endpointDeviceWatcher) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiEndpointDeviceWatcherStatics_QueryInterface(This,riid,ppvObject) (This)->QueryInterface(riid,ppvObject)
    #define IMidiEndpointDeviceWatcherStatics_Release(This) (This)->Release()
    #define IMidiEndpointDeviceWatcherStatics_Create(This,endpointDeviceWatcher) (This)->Create(endpointDeviceWatcher)
#else
    #define IMidiEndpointDeviceWatcherStatics_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
    #define IMidiEndpointDeviceWatcherStatics_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiEndpointDeviceWatcherStatics_Create(This,endpointDeviceWatcher) (This)->lpVtbl->Create(This,endpointDeviceWatcher)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiGroup
DECLARE_INTERFACE_(IMidiGroup, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiGroup methods
    STDMETHOD(get_Index)(THIS_ uint8_t *index) PURE;
    STDMETHOD(put_Index)(THIS_ uint8_t value) PURE;
    STDMETHOD(get_DisplayValue)(THIS_ uint8_t *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiGroup_Release(This) (This)->Release()
    #define IMidiGroup_get_Index(This,index) (This)->get_Index(index)
#else
    #define IMidiGroup_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiGroup_get_Index(This,index) (This)->lpVtbl->get_Index(This,index)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiFunctionBlock
DECLARE_INTERFACE_(IMidiFunctionBlock, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiFunctionBlock methods
    STDMETHOD(get_IsReadOnly)(THIS_ bool *) PURE;
    STDMETHOD(get_Number)(THIS_ uint8_t *) PURE;
    STDMETHOD(put_Number)(THIS_ uint8_t) PURE;
    STDMETHOD(get_Name)(THIS_ HSTRING *name) PURE;
    STDMETHOD(put_Name)(THIS_ HSTRING value) PURE;
    STDMETHOD(get_IsActive)(THIS_ bool *) PURE;
    STDMETHOD(put_IsActive)(THIS_ bool) PURE;
    STDMETHOD(get_Direction)(THIS_ enum MidiFunctionBlockDirection *direction) PURE;
    STDMETHOD(put_Direction)(THIS_ enum MidiFunctionBlockDirection value) PURE;
    STDMETHOD(get_UIHint)(THIS_ enum MidiFunctionBlockUIHint *) PURE;
    STDMETHOD(put_UIHint)(THIS_ enum MidiFunctionBlockUIHint value) PURE;
    STDMETHOD(get_RepresentsMidi10Connection)(THIS_ enum MidiFunctionBlockRepresentsMidi10Connection *) PURE;
    STDMETHOD(put_RepresentsMidi10Connection)(THIS_ enum MidiFunctionBlockRepresentsMidi10Connection value) PURE;
    STDMETHOD(get_FirstGroup)(THIS_ IMidiGroup **group) PURE;
    STDMETHOD(put_FirstGroup)(THIS_ IMidiGroup *value) PURE;
    STDMETHOD(get_GroupCount)(THIS_ uint8_t *) PURE;
    STDMETHOD(put_GroupCount)(THIS_ uint8_t) PURE;
    STDMETHOD(IncludesGroup)(THIS_ IMidiGroup *group, bool *) PURE;
    STDMETHOD(get_MidiCIMessageVersionFormat)(THIS_ uint8_t *) PURE;
    STDMETHOD(put_MidiCIMessageVersionFormat)(THIS_ uint8_t value) PURE;
    STDMETHOD(get_MaxSystemExclusive8Streams)(THIS_ uint8_t *) PURE;
    STDMETHOD(put_MaxSystemExclusive8Streams)(THIS_ uint8_t value) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiFunctionBlock_Release(This) (This)->Release()
    #define IMidiFunctionBlock_get_Name(This,name) (This)->get_Name(name)
    #define IMidiFunctionBlock_get_Direction(This,direction) (This)->get_Direction(direction)
    #define IMidiFunctionBlock_get_FirstGroup(This,group) (This)->get_FirstGroup(group)
#else
    #define IMidiFunctionBlock_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiFunctionBlock_get_Name(This,name) (This)->lpVtbl->get_Name(This,name)
    #define IMidiFunctionBlock_get_Direction(This,direction) (This)->lpVtbl->get_Direction(This,direction)
    #define IMidiFunctionBlock_get_FirstGroup(This,group) (This)->lpVtbl->get_FirstGroup(This,group)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiGroupTerminalBlock
DECLARE_INTERFACE_(IMidiGroupTerminalBlock, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiGroupTerminalBlock methods
    STDMETHOD(get_Number)(THIS_ uint8_t *) PURE;
    STDMETHOD(get_Name)(THIS_ HSTRING *name) PURE;
    STDMETHOD(get_Direction)(THIS_ enum MidiGroupTerminalBlockDirection *direction) PURE;
    STDMETHOD(get_Protocol)(THIS_ enum MidiGroupTerminalBlockProtocol *) PURE;
    STDMETHOD(get_FirstGroup)(THIS_ IMidiGroup **group) PURE;
    STDMETHOD(get_GroupCount)(THIS_ uint8_t *) PURE;
    STDMETHOD(IncludesGroup)(THIS_ IMidiGroup *group, bool *) PURE;
    STDMETHOD(get_MaxDeviceInputBandwidthIn4KBitsPerSecondUnits)(THIS_ uint16_t *) PURE;
    STDMETHOD(get_MaxDeviceOutputBandwidthIn4KBitsPerSecondUnits)(THIS_ uint16_t *) PURE;
    STDMETHOD(get_CalculatedMaxDeviceInputBandwidthBitsPerSecond)(THIS_ uint32_t *) PURE;
    STDMETHOD(get_CalculatedMaxDeviceOutputBandwidthBitsPerSecond)(THIS_ uint32_t *) PURE;
    STDMETHOD(AsEquivalentFunctionBlock)(THIS_ IMidiFunctionBlock **) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiGroupTerminalBlock_Release(This) (This)->Release()
    #define IMidiGroupTerminalBlock_get_Name(This,name) (This)->get_Name(name)
    #define IMidiGroupTerminalBlock_get_Direction(This,direction) (This)->get_Direction(direction)
    #define IMidiGroupTerminalBlock_get_FirstGroup(This,group) (This)->get_FirstGroup(group)
#else
    #define IMidiGroupTerminalBlock_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiGroupTerminalBlock_get_Name(This,name) (This)->lpVtbl->get_Name(This,name)
    #define IMidiGroupTerminalBlock_get_Direction(This,direction) (This)->lpVtbl->get_Direction(This,direction)
    #define IMidiGroupTerminalBlock_get_FirstGroup(This,group) (This)->lpVtbl->get_FirstGroup(This,group)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiSession
DECLARE_INTERFACE_(IMidiSession, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiSession methods
    STDMETHOD(get_SessionId)(THIS_ GUID *) PURE;
    STDMETHOD(get_Name)(THIS_ HSTRING *) PURE;
    STDMETHOD(get_IsOpen)(THIS_ bool *) PURE;
    STDMETHOD(get_Connections)(THIS_ void **) PURE;
    STDMETHOD(CreateEndpointConnection)(THIS_ HSTRING endpointDeviceId, IMidiEndpointConnection **endpointConnection) PURE;
    STDMETHOD(CreateEndpointConnection2)(THIS_ HSTRING endpointDeviceId, void *settings, IMidiEndpointConnection **) PURE;
    STDMETHOD(DisconnectEndpointConnection)(THIS_ const GUID *endpointConnectionId) PURE;
    STDMETHOD(UpdateName)(THIS_ HSTRING newName, bool *) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiSession_Release(This) (This)->Release()
    #define IMidiSession_CreateEndpointConnection(This,endpointDeviceId,endpointConnection) (This)->CreateEndpointConnection(endpointDeviceId,endpointConnection)
    #define IMidiSession_DisconnectEndpointConnection(This,endpointConnectionId) (This)->DisconnectEndpointConnection(endpointConnectionId)
#else
    #define IMidiSession_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiSession_CreateEndpointConnection(This,endpointDeviceId,endpointConnection) (This)->lpVtbl->CreateEndpointConnection(This,endpointDeviceId,endpointConnection)
    #define IMidiSession_DisconnectEndpointConnection(This,endpointConnectionId) (This)->lpVtbl->DisconnectEndpointConnection(This,endpointConnectionId)
#endif
};

#undef INTERFACE
#define INTERFACE IMidiSessionStatics
DECLARE_INTERFACE_(IMidiSessionStatics, IInspectable) {
    BEGIN_INTERFACE

    // IUnknown methods
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppvObject) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;

    // IInspectable methods
    STDMETHOD(GetIids)(THIS_ ULONG *iidCount, IID **iids) PURE;
    STDMETHOD(GetRuntimeClassName)(THIS_ HSTRING *className) PURE;
    STDMETHOD(GetTrustLevel)(THIS_ TrustLevel *trustLevel) PURE;

    // IMidiSessionStatics methods
    STDMETHOD(Create)(THIS_ HSTRING sessionName, IMidiSession **session) PURE;

    END_INTERFACE

#ifdef __cplusplus
    #define IMidiSessionStatics_QueryInterface(This,riid,ppvObject) (This)->QueryInterface(riid,ppvObject)
    #define IMidiSessionStatics_Release(This) (This)->Release()
    #define IMidiSessionStatics_Create(This,sessionName,session) (This)->Create(sessionName,session)
#else
    #define IMidiSessionStatics_QueryInterface(This,riid,ppvObject) (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
    #define IMidiSessionStatics_Release(This) (This)->lpVtbl->Release(This)
    #define IMidiSessionStatics_Create(This,sessionName,session) (This)->lpVtbl->Create(This,sessionName,session)
#endif
};


static const GUID IID_IMidiClientInitializer = { 0x8087b303, 0xd551, 0xbce2, { 0x1e,0xad, 0xa2,0x50,0x0d,0x50,0xc5,0x80 } }; // 8087b303-d551-bce2-1ead-a2500d50c580
static const GUID IID_MidiClientInitializerUuid = { 0xc3263827, 0xc3b0, 0xbdbd, { 0x25,0x00, 0xce,0x63,0xa3,0xf3,0xf2,0xc3 } }; // c3263827-c3b0-bdbd-2500-ce63a3f3f2c3

static const GUID IID_IMidiClockStatics = { 0x8087B303, 0x0519, 0x31D1,{ 0xC0,0xDE, 0xEE,0x00,0x00,0x00,0x60,0x00 } }; // 8087B303-0519-31D1-C0DE-EE0000006000
static const GUID IID_IMidiEndpointConnectionSource = { 0x8087B303, 0x0519, 0x31D1,{ 0xC0,0xDE, 0xFF,0x00,0x00,0x00,0x00,0x30 } }; // 8087B303-0519-31D1-C0DE-FF0000000030
static const GUID IID_IMidiEndpointDeviceInformationStatics = { 0x8087B303, 0x0519, 0x31D1,{ 0xC0,0xDE, 0xEE,0x00,0x00,0x00,0xA0,0x00 } }; // 8087B303-0519-31D1-C0DE-EE000000A000
static const GUID IID_IMidiEndpointDeviceWatcherStatics = { 0x8087B303, 0x0519, 0x31D1,{ 0xC0,0xDE, 0xEE,0x00,0x00,0x00,0xE0,0x00 } }; // 8087B303-0519-31D1-C0DE-EE000000E000
static const GUID IID_IMidiSessionStatics = { 0x8087B303, 0x0519, 0x31D1,{ 0xC0,0xDE, 0xEE,0x00,0x00,0x08,0x00,0x00 } }; // 8087B303-0519-31D1-C0DE-EE0000080000

static const GUID IID_Midi2LoopbackMidiTransport = { 0x942bf02d, 0x93c0, 0x4ea8, { 0xb0,0x3e, 0xd5,0x11,0x56,0xca,0x75,0xe1 } }; // 942bf02d-93c0-4ea8-b03e-d51156ca75e1

static const wchar_t name_MidiClock[] = L"Microsoft.Windows.Devices.Midi2.MidiClock";
static const wchar_t name_MidiEndpointDeviceInformation[] = L"Microsoft.Windows.Devices.Midi2.MidiEndpointDeviceInformation";
static const wchar_t name_MidiEndpointDeviceWatcher[] = L"Microsoft.Windows.Devices.Midi2.MidiEndpointDeviceWatcher";
static const wchar_t name_MidiSession[] = L"Microsoft.Windows.Devices.Midi2.MidiSession";

