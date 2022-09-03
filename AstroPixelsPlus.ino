/*****
 * 
 * AstroPixelsPlus sketch operates as a I2C master that can optionally be connected to one or more
 * Adafruit PCA9685 servo controllers to control dome panels. The sketch also provides serial commands
 * on Serial2.
 * 
 */

#define USE_DEBUG                     // Define to enable debug diagnostic
#define USE_WIFI                      // Define to enable Wifi support
#define USE_SPIFFS
#ifdef USE_WIFI
#define USE_DROID_REMOTE              // Define for droid remote support
#define USE_MDNS
#define USE_OTA
#define USE_WIFI_WEB
#define USE_WIFI_MARCDUINO
//#define LIVE_STREAM
#endif

////////////////////////////////

// Replace with your network credentials
#ifdef USE_WIFI
#define REMOTE_ENABLED       false    // default disabled
#define WIFI_ENABLED         true     // default enabled
// Set these to your desired WiFi credentials.
#define WIFI_AP_NAME         "AstroPixels"
#define WIFI_AP_PASSPHRASE   "Astromech"
#define WIFI_ACCESS_POINT    true  /* true if access point: false if joining existing wifi */
#endif

// SMQ device name for ESPNOW
#define SMQ_HOSTNAME         "Astro"
#define SMQ_SECRET           "Astromech"

///////////////////////////////////

#if __has_include("build_version.h")
#include "build_version.h"
#endif

#if __has_include("reeltwo_build_version.h")
#include "reeltwo_build_version.h"
#endif

////////////////////////////////

#define PREFERENCE_REMOTE_ENABLED       "remote"
#define PREFERENCE_REMOTE_HOSTNAME      "rhost"
#define PREFERENCE_REMOTE_SECRET        "rsecret"

#define PREFERENCE_WIFI_ENABLED         "wifi"
#define PREFERENCE_WIFI_SSID            "ssid"
#define PREFERENCE_WIFI_PASS            "pass"
#define PREFERENCE_WIFI_AP              "ap"

#define PREFERENCE_MARCSERIAL1          "mserial1"
#define PREFERENCE_MARCSERIAL2          "mserial2"
#define PREFERENCE_MARCSERIAL_PASS      "mserialpass"
#define PREFERENCE_MARCSERIAL_ENABLED   "mserial"

#define PREFERENCE_MARCWIFI_ENABLED     "mwifi"
#define PREFERENCE_MARCWIFI_SERIAL_PASS "mwifipass"

////////////////////////////////

#define CONSOLE_BUFFER_SIZE     300

////////////////////////////////

#if defined(USE_LCD_SCREEN) || defined(USE_DROID_REMOTE)
#define USE_MENUS                     // Define if using menu system
#endif

////////////////////////////////

#ifdef USE_DROID_REMOTE
#include "ReelTwoSMQ32.h"
#else
#include "ReelTwo.h"
#endif
#include "dome/Logics.h"
#include "dome/LogicEngineController.h"
#include "dome/HoloLights.h"
#include "dome/NeoPSI.h"
#include "dome/FireStrip.h"
#include "dome/BadMotivator.h"
#include "dome/TeecesPSI.h"
#include "dome/TeecesLogics.h"
#include "body/DataPanel.h"
#include "body/ChargeBayIndicator.h"

#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "core/Marcduino.h"

#include <Preferences.h>

////////////////////////////////

#define SERIAL2_RX_PIN 16
#define SERIAL2_TX_PIN 17
#define COMMAND_SERIAL Serial2

////////////////////////////////

#define MARC_SERIAL2_BAUD_RATE          9600
#define MARC_SERIAL_PASS                true
#define MARC_SERIAL_ENABLED             true
#define MARC_WIFI_ENABLED               true
#define MARC_WIFI_SERIAL_PASS           true

#include "wifi/WifiAccess.h"

////////////////////////////////

#ifdef USE_MDNS
#include <ESPmDNS.h>
#endif
#ifdef USE_WIFI_WEB
#include "wifi/WifiWebServer.h"
#endif
#ifdef USE_WIFI_MARCDUINO
#include "wifi/WifiMarcduinoReceiver.h"
#endif
#ifdef USE_OTA
#include <ArduinoOTA.h>
#endif
#ifdef USE_SPIFFS
#include "SPIFFS.h"
#define USE_FS SPIFFS
#elif defined(USE_FATFS)
#include "FFat.h"
#define USE_FS FFat
#elif defined(USE_LITTLEFS)
#include "LITTLEFS.h"
#define USE_FS LITTLEFS
#endif
#include "FS.h"

////////////////////////////////

#define PIN_SDA 21
#define PIN_SCL 22
#define PIN_FRONT_LOGIC 15
#define PIN_REAR_LOGIC 33
#define PIN_FRONT_PSI 32
#define PIN_REAR_PSI 23
#define PIN_FRONT_HOLO 25
#define PIN_REAR_HOLO 26
#define PIN_TOP_HOLO 27
#define PIN_AUX1 2
#define PIN_AUX2 4
#define PIN_AUX3 5
#define PIN_AUX4 18
#define PIN_AUX5 19

#define CBI_DATAIN_PIN      PIN_AUX3
#define CBI_CLOCK_PIN       PIN_AUX2
#define CBI_LOAD_PIN        PIN_AUX1

////////////////////////////////

AstroPixelRLD<PIN_REAR_LOGIC> RLD(LogicEngineRLDDefault, 3);
AstroPixelFLD<PIN_FRONT_LOGIC> FLD(LogicEngineFLDDefault, 1);

AstroPixelFrontPSI<PIN_FRONT_PSI> frontPSI(LogicEngineFrontPSIDefault, 4);
AstroPixelRearPSI<PIN_REAR_PSI> rearPSI(LogicEngineRearPSIDefault, 5);

HoloLights frontHolo(PIN_FRONT_HOLO, HoloLights::kRGB);
HoloLights rearHolo(PIN_REAR_HOLO, HoloLights::kRGB);
HoloLights topHolo(PIN_TOP_HOLO, HoloLights::kRGB);

// FireStrip fireStrip(PIN_AUX4);
// BadMotivator badMotivator(PIN_AUX5);

// LedControlMAX7221<5> ledChain1(CBI_DATAIN_PIN, CBI_CLOCK_PIN, CBI_LOAD_PIN);
// ChargeBayIndicator chargeBayIndicator(ledChain1);
// DataPanel dataPanel(ledChain1);
// TeecesPSI teecesPSI(ledChain1);
// TeecesRearLogics teecesRLD(ledChain1);
// TeecesFrontLogics teecesTFLD(ledChain1);

////////////////////////////////

#define SMALL_PANEL         0x0001
#define MEDIUM_PANEL        0x0002
#define BIG_PANEL           0x0004
#define PIE_PANEL           0x0008
#define TOP_PIE_PANEL       0x0010
#define MINI_PANEL          0x0020

#define HOLO_HSERVO         0x1000
#define HOLO_VSERVO         0x2000

#define DOME_PANELS_MASK        (SMALL_PANEL|MEDIUM_PANEL|BIG_PANEL)
#define PIE_PANELS_MASK         (PIE_PANEL)
#define ALL_DOME_PANELS_MASK    (MINI_PANEL|DOME_PANELS_MASK|PIE_PANELS_MASK|TOP_PIE_PANEL)
#define DOME_DANCE_PANELS_MASK  (DOME_PANELS_MASK|PIE_PANELS_MASK)
#define HOLO_SERVOS_MASK        (HOLO_HSERVO|HOLO_VSERVO)

#define PANEL_GROUP_1      (1L<<14)
#define PANEL_GROUP_2      (1L<<15)
#define PANEL_GROUP_3      (1L<<16)
#define PANEL_GROUP_4      (1L<<17)
#define PANEL_GROUP_5      (1L<<18)
#define PANEL_GROUP_6      (1L<<19)
#define PANEL_GROUP_7      (1L<<20)
#define PANEL_GROUP_8      (1L<<21)
#define PANEL_GROUP_9      (1L<<22)
#define PANEL_GROUP_10     (1L<<23)

////////////////////////////////
// These values will be configurable through the WiFi interface and stored in the preferences.
const ServoSettings servoSettings[] PROGMEM = {
    // First PCA9685 controller
    { 1,  800, 2200, PANEL_GROUP_4|SMALL_PANEL },  /* 0: door 4 */
    { 2,  800, 2200, PANEL_GROUP_3|SMALL_PANEL },  /* 1: door 3 */
    { 3,  800, 2200,  PANEL_GROUP_2|SMALL_PANEL }, /* 2: door 2 */
    { 4,  800, 2200, PANEL_GROUP_1|SMALL_PANEL },  /* 3: door 1 */
    { 5,  800, 2200, PANEL_GROUP_5|MEDIUM_PANEL }, /* 4: door 5 */
    { 6,  800, 2200, PANEL_GROUP_6|BIG_PANEL },    /* 5: door 9 */
    { 7,  800, 2200, MINI_PANEL },                 /* 6: mini door 2 */
    { 8,  800, 2200, MINI_PANEL },                 /* 7: mini front psi door */
    { 9,  800, 2200, PANEL_GROUP_10|PIE_PANEL },   /* 8: pie panel 1 */
    { 10, 800, 2200, PANEL_GROUP_9|PIE_PANEL },    /* 9: pie panel 2 */
    { 11, 800, 2200, PANEL_GROUP_8|PIE_PANEL },    /* 10: pie panel 3 */
    { 12, 800, 2200, PANEL_GROUP_7|PIE_PANEL },    /* 11: pie panel 4 */
    { 13, 800, 2200, TOP_PIE_PANEL },              /* 12: dome top panel */

    // Second PCA9685 controller
    { 16, 800, 2200, HOLO_HSERVO },                /* 13: horizontal front holo */
    { 17, 800, 2200, HOLO_VSERVO },                /* 14: vertical front holo */
    { 18, 800, 2200, HOLO_HSERVO },                /* 15: horizontal top holo */
    { 19, 800, 2200, HOLO_VSERVO },                /* 16: vertical top holo */
    { 20, 800, 2200, HOLO_VSERVO },                /* 17: vertical rear holo */
    { 21, 800, 2200, HOLO_HSERVO },                /* 18: horizontal rear holo */
};

ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
AnimationPlayer player(servoSequencer);
MarcduinoSerial<> marcduinoSerial(player);

/////////////////////////////////////////////////////////////////////////

#define NUM_LEDS 28*4
uint32_t lastEvent;
CRGB leds[NUM_LEDS];
#ifdef LIVE_STREAM
AsyncUDP udp;
#endif

enum
{
    SDBITMAP = 100,
    PLASMA,
    METABALLS,
    FRACTAL,
    FADEANDSCROLL
};

#include "effects/BitmapEffect.h"
#include "effects/FadeAndScrollEffect.h"
#include "effects/FractalEffect.h"
#include "effects/MeatBallsEffect.h"
#include "effects/PlasmaEffect.h"

////////////////////////////////
// Standard LogicEngine sequences are in the range 0-99. Custom sequences start at 100
LogicEffect CustomLogicEffectSelector(unsigned selectSequence)
{
    static const LogicEffect sCustomLogicEffects[] = {
        LogicEffectBitmap,
        LogicEffectPlasma,
        LogicEffectMetaBalls,
        LogicEffectFractal,
        LogicEffectFadeAndScroll
    };
    if (selectSequence >= 100 && selectSequence-100 <= SizeOfArray(sCustomLogicEffects))
    {
        return LogicEffect(sCustomLogicEffects[selectSequence-100]);
    }
    return LogicEffectDefaultSelector(selectSequence);
}

////////////////////////////////

Preferences preferences;

////////////////////////////////

bool mountReadOnlyFileSystem()
{
#ifdef USE_SPIFFS
    return (SPIFFS.begin(true));
#endif
    return false;
}

void unmountFileSystems()
{
#ifdef USE_SPIFFS
    SPIFFS.end();
#endif
}

////////////////////////////////
// This function is called when settings have been changed and needs a reboot
void reboot()
{
    DEBUG_PRINTLN("Restarting...");
    unmountFileSystems();
    preferences.end();
    delay(1000);
    ESP.restart();
}

////////////////////////////////
// This function is called when aborting or ending Marcduino sequences. It should reset all droid devices to Normal
void resetSequence()
{
    Marcduino::send(F("$s"));
    CommandEvent::process(F( 
        "LE000000|0\n"   // LogicEngine devices to normal
        "FSOFF\n"        // Fire Stripe Off
        "BMOFF\n"        // Bad Motiviator Off
        "HPA000|0\n"     // Holo Projectors to Normal
        "CB00000\n"      // Charge Bay to Normal
        "DP00000\n"));   // Data Panel to Normal
}

////////////////////////////////

int32_t strtol(const char* cmd, const char** endptr)
{
    bool sign = false;
    int32_t result = 0;
    if (*cmd == '-')
    {
        cmd++;
        sign = true;
    }
    while (isdigit(*cmd))
    {
        result = result*10L + (*cmd-'0');
        cmd++;
    }
    *endptr = cmd;
    return (sign) ? -result : result;
}

////////////////////////////////

bool numberparams(const char* cmd, uint8_t &argcount, int32_t* args, uint8_t maxcount)
{
    for (argcount = 0; argcount < maxcount; argcount++)
    {
        args[argcount] = strtol(cmd, &cmd);
        if (*cmd == '\0')
        {
            argcount++;
            return true;
        }
        else if (*cmd != ',')
        {
            return false;
        }
        cmd++;
    }
    return true;
}

////////////////////////////////

#include "MarcduinoHolo.h"
#include "MarcduinoLogics.h"
#include "MarcduinoSequence.h"
#include "MarcduinoPanel.h"
#include "MarcduinoPSI.h"

////////////////////////////////

#ifdef USE_WIFI
WifiAccess wifiAccess;
bool wifiEnabled;
bool wifiActive;
bool remoteEnabled;
bool remoteActive;
TaskHandle_t eventTask;
bool otaInProgress;
#endif

#ifdef USE_WIFI_MARCDUINO
WifiMarcduinoReceiver wifiMarcduinoReceiver(wifiAccess);
#endif

////////////////////////////////

#ifdef USE_MENUS

#include "Screens.h"
#include "menus/CommandScreen.h"

#ifdef USE_LCD_SCREEN

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define SCREEN_ADDRESS 0x3C

#include "menus/CommandScreenHandlerSSD1306.h"
CommandScreenHandlerSSD1306 sDisplay(sPinManager);

#else

#include "menus/CommandScreenHandlerSMQ.h"
CommandScreenHandlerSMQ sDisplay;

#endif

#include "menus/utility/ChoiceIntArrayScreen.h"
#include "menus/utility/ChoiceStrArrayScreen.h"
#include "menus/utility/UnsignedValueScreen.h"
#include "menus/utility/MenuScreen.h"

#include "menus/MainScreen.h"
#include "menus/SplashScreen.h"
#include "menus/SequenceScreen.h"
#include "menus/LogicsScreen.h"
#include "menus/HoloScreen.h"

#endif

////////////////////////////////

#ifdef USE_WIFI_WEB
#include "WebPages.h"
#endif

////////////////////////////////

void scan_i2c()
{
    unsigned nDevices = 0;
    for (byte address = 1; address < 127; address++)
    {
        String name = "<unknown>";
        Wire.beginTransmission(address);
        byte error = Wire.endTransmission();
        if (address == 0x70)
        {
            // All call address for PCA9685
            name = "PCA9685:all";
        }
        if (address == 0x40)
        {
            // Adafruit PCA9685
            name = "PCA9685";
        }
        if (address == 0x14)
        {
            // IA-Parts magic panel
            name = "IA-Parts Magic Panel";
        }
        if (address == 0x20)
        {
            // IA-Parts periscope
            name = "IA-Parts Periscope";
        }
        if (address == 0x16)
        {
            // PSIPro
            name = "PSIPro";
        }

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.print(" ");
            Serial.println(name);
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
}

////////////////////////////////

void setup()
{
    REELTWO_READY();

    if (!preferences.begin("astro", false))
    {
        DEBUG_PRINTLN("Failed to init prefs");
    }
#ifdef USE_WIFI
    wifiEnabled = wifiActive = preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED);
    remoteEnabled = remoteActive = preferences.getBool(PREFERENCE_REMOTE_ENABLED, REMOTE_ENABLED);
#endif
    PrintReelTwoInfo(Serial, "AstroPixelsPlus");

    if (preferences.getBool(PREFERENCE_MARCSERIAL_ENABLED, MARC_SERIAL_ENABLED))
    {
        COMMAND_SERIAL.begin(preferences.getInt(PREFERENCE_MARCSERIAL2, MARC_SERIAL2_BAUD_RATE));//, SERIAL_8N1, SERIAL2_RX_PIN, SERIAL2_TX_PIN);
        // if (preferences.getBool(PREFERENCE_MARCSERIAL_PASS, MARC_SERIAL_PASS))

        marcduinoSerial.setStream(&COMMAND_SERIAL);
    }
    if (!mountReadOnlyFileSystem())
    {
        DEBUG_PRINTLN("Failed to mount read only filesystem");
    }

    Wire.begin();
    // scan_i2c();
    SetupEvent::ready();

    // dataPanel.setSequence(DataPanel::kDisabled);
    // chargeBayIndicator.setSequence(ChargeBayIndicator::kDisabled);

#ifdef USE_LCD_SCREEN
    sDisplay.setEnabled(sDisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS));
    if (sDisplay.isEnabled())
    {
        sDisplay.invertDisplay(false);
        sDisplay.clearDisplay();
        sDisplay.setRotation(2);
    }
#endif

    RLD.selectScrollTextLeft("... AstroPixels ....", LogicEngineRenderer::kBlue, 0, 15);
    FLD.selectScrollTextLeft("... R2D2 ...", LogicEngineRenderer::kRed, 0, 15);

#ifdef USE_WIFI
    if (remoteEnabled)
    {
    #ifdef USE_SMQ
        WiFi.mode(WIFI_MODE_APSTA);
        if (SMQ::init(preferences.getString(PREFERENCE_REMOTE_HOSTNAME, SMQ_HOSTNAME),
                        preferences.getString(PREFERENCE_REMOTE_SECRET, SMQ_SECRET)))
        {
            printf("Droid Remote Enabled\n");
            SMQ::setHostDiscoveryCallback([](SMQHost* host) {
                if (host->hasTopic("LCD"))
                {
                    printf("Remote Discovered: %s\n", host->getHostName().c_str());
                }
            });

            SMQ::setHostLostCallback([](SMQHost* host) {
                printf("Lost: %s\n", host->getHostName().c_str());
            });
        }
        else
        {
            printf("Failed to activate Droid Remote\n");
        }
    #endif
    }
    if (wifiEnabled)
    {
    #ifdef USE_WIFI_WEB
        // In preparation for adding WiFi settings web page
        wifiAccess.setNetworkCredentials(
            preferences.getString(PREFERENCE_WIFI_SSID, WIFI_AP_NAME),
            preferences.getString(PREFERENCE_WIFI_PASS, WIFI_AP_PASSPHRASE),
            preferences.getBool(PREFERENCE_WIFI_AP, WIFI_ACCESS_POINT),
            preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED));
    #ifdef USE_WIFI_MARCDUINO
        wifiMarcduinoReceiver.setEnabled(preferences.getBool(PREFERENCE_MARCWIFI_ENABLED, MARC_WIFI_ENABLED));
        if (wifiMarcduinoReceiver.enabled())
        {
            wifiMarcduinoReceiver.setCommandHandler([](const char* cmd) {
                printf("cmd: %s\n", cmd);
                Marcduino::processCommand(player, cmd);
                if (preferences.getBool(PREFERENCE_MARCWIFI_SERIAL_PASS, MARC_WIFI_SERIAL_PASS))
                {
                    COMMAND_SERIAL.print(cmd); COMMAND_SERIAL.print('\r');
                }
            });
        }
    #endif
        wifiAccess.notifyWifiConnected([](WifiAccess &wifi) {
        #ifdef STATUSLED_PIN
            statusLED.setMode(sCurrentMode = kWifiMode);
        #endif
            Serial.print("Connect to http://"); Serial.println(wifi.getIPAddress());
        #ifdef USE_MDNS
            // No point in setting up mDNS if R2 is the access point
            if (!wifi.isSoftAP())
            {
                String mac = wifi.getMacAddress();
                String hostName = mac.substring(mac.length()-5, mac.length());
                hostName.remove(2, 1);
                hostName = String(WIFI_AP_NAME)+String("-")+hostName;
                if (webServer.enabled())
                {
                    Serial.print("Host name: "); Serial.println(hostName);
                    if (!MDNS.begin(hostName.c_str()))
                    {
                        DEBUG_PRINTLN("Error setting up MDNS responder!");
                    }
                }
            }
        #endif
        });
    #endif
    #ifdef USE_OTA
        ArduinoOTA.onStart([]()
        {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            {
                type = "sketch";
            }
            else // U_SPIFFS
            {
                type = "filesystem";
            }
            DEBUG_PRINTLN("OTA START");
        })
        .onEnd([]()
        {
            DEBUG_PRINTLN("OTA END");
        })
        .onProgress([](unsigned int progress, unsigned int total)
        {
            // float range = (float)progress / (float)total;
        })
        .onError([](ota_error_t error)
        {
            String desc;
            if (error == OTA_AUTH_ERROR) desc = "Auth Failed";
            else if (error == OTA_BEGIN_ERROR) desc = "Begin Failed";
            else if (error == OTA_CONNECT_ERROR) desc = "Connect Failed";
            else if (error == OTA_RECEIVE_ERROR) desc = "Receive Failed";
            else if (error == OTA_END_ERROR) desc = "End Failed";
            else desc = "Error: "+String(error);
            DEBUG_PRINTLN(desc);
        });
    #endif
    }
#endif
#ifdef USE_WIFI_WEB
    // For safety we will stop the motors if the web client is connected
    webServer.setConnect([]() {
        // Callback for each connected web client
        // DEBUG_PRINTLN("Hello");
    });
#endif

    RLD.setLogicEffectSelector(CustomLogicEffectSelector);
    FLD.setLogicEffectSelector(CustomLogicEffectSelector);
    frontPSI.setLogicEffectSelector(CustomLogicEffectSelector);
    rearPSI.setLogicEffectSelector(CustomLogicEffectSelector);

#ifdef USE_WIFI
    xTaskCreatePinnedToCore(
          eventLoopTask,
          "Events",
          10000,    // shrink stack size?
          NULL,
          1,
          &eventTask,
          0);
#endif
    DEBUG_PRINTLN("Ready");
}

////////////////

MARCDUINO_ACTION(DirectCommand, ~RT, ({
    // Direct ReelTwo command
    CommandEvent::process(Marcduino::getCommand());
}))

////////////////

MARCDUINO_ACTION(WifiByeBye, #WIBYE, ({
#ifdef USE_WIFI
    if (wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, false);
        DEBUG_PRINT("Disabling WiFi. ");
        reboot();
    }
#endif
}))

////////////////

MARCDUINO_ACTION(WifiHiHi, #WIHI, ({
#ifdef USE_WIFI
    if (!wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, true);
        DEBUG_PRINT("Enabling WiFi. ");
        reboot();
    }
#endif
}))

////////////////

MARCDUINO_ACTION(RemoteByeBye, #WIREMOTEBYE, ({
#ifdef USE_DROID_REMOTE
    if (remoteEnabled)
    {
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, false);
        DEBUG_PRINT("Disabling droid remote. ");
        reboot();
    }
#endif
}))

////////////////

MARCDUINO_ACTION(RemoteHiHI, #WIREMOTEHI, ({
#ifdef USE_DROID_REMOTE
    if (!remoteEnabled)
    {
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, true);
        DEBUG_PRINT("Enabling droid remote. ");
        reboot();
    }
#endif
}))

////////////////

MARCDUINO_ACTION(ClearPrefs, #WZERO, ({
#ifdef USE_DROID_REMOTE
    preferences.clear();
    DEBUG_PRINT("Clearing preferences. ");
    reboot();
#endif
}))

////////////////

#ifdef USE_SMQ
// SMQ messages are received via ESPNOW.
SMQMESSAGE(DIAL, {
    long newValue = msg.get_int32("new");
    long oldValue = msg.get_int32("old");
    sDisplay.remoteDialEvent(newValue, oldValue);
})

///////////////////////////////////////////////////////////////////////////////

SMQMESSAGE(BUTTON, {
    uint8_t id = msg.get_uint8("id");
    bool pressed = msg.get_uint8("pressed");
    bool repeat = msg.get_uint8("repeat");
    sDisplay.remoteButtonEvent(id, pressed, repeat);
})

///////////////////////////////////////////////////////////////////////////////

SMQMESSAGE(SELECT, {
    DEBUG_PRINTLN("REMOTE ACTIVE");
    sDisplay.setEnabled(true);
    sDisplay.switchToScreen(kMainScreen);
    sMainScreen.init();
})
#endif

////////////////

static unsigned sPos;
static char sBuffer[CONSOLE_BUFFER_SIZE];

////////////////

void mainLoop()
{
    AnimatedEvent::process();
#ifdef USE_MENUS
    sDisplay.process();
#endif

    if (Serial.available())
    {
        int ch = Serial.read();
        if (ch == 0x0A || ch == 0x0D)
        {
            Marcduino::processCommand(player, sBuffer);
            sPos = 0;
        }
        else if (sPos < SizeOfArray(sBuffer)-1)
        {
            sBuffer[sPos++] = ch;
            sBuffer[sPos] = '\0';
        }
    }

#ifdef COMMAND_SERIAL
    // Serial commands are processed in the same buffer as the console serial
    if (COMMAND_SERIAL.available())
    {
        int ch = COMMAND_SERIAL.read();
        printf("ch: %d\n", ch);
        if (ch == 0x0A || ch == 0x0D)
        {
            Marcduino::processCommand(player, sBuffer);
            sPos = 0;
        }
        else if (sPos < SizeOfArray(sBuffer)-1)
        {
            sBuffer[sPos++] = ch;
            sBuffer[sPos] = '\0';
        }
    }
#endif
}

////////////////

#ifdef USE_WIFI
void eventLoopTask(void* )
{
    // TODO remove delay
    delay(5000);
    for (;;)
    {
        mainLoop();
        vTaskDelay(1);
    }
}
#endif

////////////////

void loop()
{
#ifdef USE_WIFI
    if (wifiActive)
    {
    #ifdef USE_OTA
        ArduinoOTA.handle();
    #endif
    #ifdef USE_WIFI_WEB
        webServer.handle();
    #endif
    }
    if (remoteActive)
    {
    #ifdef USE_SMQ
        SMQ::process();
    #endif
    }
#else
    mainLoop();
#endif
}
