MorikawaSDK
===========

An arduino base application programming environment for satellite's mission.

currently supported APIs:

```cpp
// singleton design
static & getInstance(void);

// memory environment and memory state
static TSTError getMemorySpec(MemorySpec* result);
static TSTError getMemoryInfo(MemoryInfo* result);
static TSTError getMemoryLog(MemoryInfo* result);
static void saveMemoryLog(void);

// self test
static TSTError getSelfTestLog(SelfTestLog* result);
static void eraseSelfTestLog(void);

// memory size information
static unsigned long getSizeEEPROM(void);
static unsigned long getSizeSharedMemory(void);
static unsigned long getSizeFRAM(void);
static unsigned long getSizeFlashROM(void);
static unsigned int getPageSizeEEPROM(void);
static unsigned int getPageSizeSharedMemory(void);
static unsigned int getPageSizeFRAM(void);
static unsigned int getPageSizeFlashROM(void);
static unsigned long getSectorSizeEEPROM(void);
static unsigned long getSectorSizeSharedMemory(void);
static unsigned long getSectorSizeFRAM(void);
static unsigned long getSectorSizeFlashROM(void);

// camera format information
static TSTError getCameraFormat(CameraType mode, CameraFormat* result);

// boot parameter
unsigned long getBootTime(void) const;
unsigned char getBootCount(void) const;
unsigned char getBootMode(void) const;

// peripheral parameter
TSTError getParamNote(NoteParam* result);
TSTError getParamMorse(MorseParam* result);
TSTError getParamDigitalker(DigitalkerParam* result);
TSTError getParamCamera(CameraParam* result);

// telemetry data
TSTError getTelemetryTime(TimeType type, unsigned long* result) const;
TSTError getTelemetryVoltage(VoltageType type, unsigned char* result) const;
TSTError getTelemetryVoltage(VoltageType type, double* result) const;
TSTError getTelemetryCurrent(CurrentType type, unsigned char* result) const;
TSTError getTelemetryCurrent(CurrentType type, double* result) const;
TSTError getTelemetryTemperature(TemperatureType type, unsigned char* result) const;
TSTError getTelemetryTemperature(TemperatureType type, double* result) const;
TSTError getTelemetryGyro(GyroType type, unsigned char* result) const;
TSTError getTelemetryGyro(GyroType type, double* result) const;
TSTError getTelemetryMagnet(MagnetType type, unsigned char* result) const;
TSTError getTelemetryMagnet(MagnetType type, double* result) const;

// FlashROM mode
TSTError setFlashROMEraseMode(bool param);
bool getFlashROMEraseMode(void) const;

// shared memory text setter and text getter
TSTError setText(TextType index, char const* text, int length = -1);
TSTError setTextPGM(TextType index, char const PROGMEM* text, int length = -1);
TSTError getText(TextType index, char* text, unsigned int length, int* result = NULL);

// peripheral LED device
TSTError setLED(LEDType index, unsigned char pwm);
unsigned char getLED(LEDType index) const;

// peripheral mode
TSTError setNoteBPM(int param = -1);
int getNoteBPM(void) const;
TSTError setMorseWPM(int param = -1);
int getMorseWPM(void) const;
TSTError setSpeakAsyncMode(bool param);
bool getSpeakAsyncMode(void) const;

// validation
bool isValid(void) const;
bool isValidSharedMemory(void) const;
bool isValidFRAM(void) const;
bool isValidFlashROM(void) const;
bool isValidLED(void) const;
bool isValidTone(void) const;
bool isValidDigitalker(void) const;
bool isValidCamera(void) const;

// status
TSTError isBusyDigitalker(bool* result) const;
bool hasTelemetryUpdate(void) const;
bool hasAbnormalShutdown(void) const;

// core function
TSTError setup(void);
void cleanup(void);
static void shutdown(void);
void loop(void);

// EEPROM
static TSTError writeEEPROM(unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
static TSTError writeEEPROMPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
static TSTError readEEPROM(unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
static TSTError formatEEPROM(void);

// Shared Memory
TSTError writeSharedMemory(unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
TSTError writeSharedMemoryPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
TSTError readSharedMemory(unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
TSTError formatSharedMemory(void);

// FRAM
TSTError writeFRAM(unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
TSTError writeFRAMPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
TSTError readFRAM(unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
TSTError formatFRAM(void);

// FlashROM
TSTError writeFlashROM(unsigned long address, void const* data, unsigned int size, unsigned int* result = NULL);
TSTError writeFlashROMPGM(unsigned long address, void const PROGMEM* data, unsigned int size, unsigned int* result = NULL);
TSTError readFlashROM(unsigned long address, void* data, unsigned int size, unsigned int* result = NULL);
TSTError formatFlashROM(void);

// note and morse
TSTError playFrequency(FrequencyType frequency, unsigned long duration);
TSTError playFrequency(FrequencySequence const* sequence, int length = -1);
TSTError playFrequencyPGM(FrequencySequence const PROGMEM* sequence, int length = -1);
TSTError playNote(NoteType note, DurationType duration, QualifierType qualifier = QUALIFIER_NONE);
TSTError playNote(NoteSequence const* sequence, int length = -1);
TSTError playNotePGM(NoteSequence const PROGMEM* sequence, int length = -1);
TSTError playMorse(NoteType note, char character);
TSTError playMorse(NoteType note, char const* sequence, int length = -1);
TSTError playMorsePGM(NoteType note, char const PROGMEM* sequence, int length = -1);

// digitalker
TSTError speakPhrase(char const* phrase, int length = -1);
TSTError speakPhrasePGM(char const PROGMEM* phrase, int length = -1);
TSTError waitPhrase(void);
TSTError stopPhrase(void);

// camera
TSTError snapshotCamera(CameraType mode, StorageType storage, unsigned long address, unsigned long size, unsigned long* result);

// data compression / decompression
TSTError freezeFastLZ(StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, StorageType wstorage, unsigned long waddress, unsigned long wsize, unsigned long* result);
TSTError meltFastLZ(StorageType istorage, unsigned long iaddress, unsigned long isize, StorageType ostorage, unsigned long oaddress, unsigned long osize, unsigned long* result);

// audio power control
TSTError enableAudioBus(void);
void disableAudioBus(void);
```
