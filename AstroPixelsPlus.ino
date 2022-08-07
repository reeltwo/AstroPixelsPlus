/*****
 * 
 * AstroPixelsPlus sketch operates as a I2C master that can optionally be connected to one or more
 * Adafruit PCA9685 servo controllers to control dome panels. The sketch also provides serial commands
 * on Serial2.
 * 
 */

#define USE_DEBUG                     // Define to enable debug diagnostic
#define USE_WIFI                      // Define to enable Wifi support
#define USE_DROID_REMOTE              // Define for droid remote support
#define USE_SPIFFS
#ifdef USE_WIFI
#define USE_MDNS
#define USE_OTA
#define USE_WIFI_WEB
#define USE_WIFI_MARCDUINO
//#define LIVE_STREAM
#endif

////////////////////////////////

#if __has_include("build_version.h")
#include "build_version.h"
#else
#define BUILD_VERSION "custom"
#endif

////////////////////////////////

#define PREFERENCE_REMOTE_ENABLED       "remote"
#define PREFERENCE_REMOTE_HOSTNAME      "rhost"
#define PREFERENCE_REMOTE_SECRET        "remote"

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

#include "ServoDispatchPCA9685.h"
#include "ServoSequencer.h"
#include "core/Marcduino.h"

#include <Preferences.h>

////////////////////////////////

#define SERIAL2_RX_PIN 16
#define SERIAL2_TX_PIN 17
#define COMMAND_SERIAL Serial2

////////////////////////////////

// Replace with your network credentials
#ifdef USE_WIFI
#define REMOTE_ENABLED       false
#define WIFI_ENABLED         true
// Set these to your desired credentials.
#define WIFI_AP_NAME         "AstroPixels"
#define WIFI_AP_PASSPHRASE   "Astromech"
#define WIFI_ACCESS_POINT    true  /* true if access point: false if joining existing wifi */
#endif

#define SMQ_HOSTNAME         "Astro"
#define SMQ_SECRET           "Astromech"

////////////////////////////////

#define MARC_SERIAL1_BAUD_RATE          9600
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

AstroPixelRLD<> RLD(LogicEngineRLDDefault, 3);
AstroPixelFLD<> FLD(LogicEngineFLDDefault, 1);

AstroPixelFrontPSI<> frontPSI(LogicEngineFrontPSIDefault, 4);
AstroPixelRearPSI<> rearPSI(LogicEngineRearPSIDefault, 5);

HoloLights frontHolo(25, HoloLights::kRGB);
HoloLights rearHolo(26, HoloLights::kRGB);
HoloLights topHolo(27, HoloLights::kRGB);   

////////////////////////////////

#include "MarcduinoHolo.h"
#include "MarcduinoLogics.h"

////////////////////////////////

const ServoSettings servoSettings[] PROGMEM = {
};

ServoDispatchPCA9685<SizeOfArray(servoSettings)> servoDispatch(servoSettings);
ServoSequencer servoSequencer(servoDispatch);
AnimationPlayer player(servoSequencer);
MarcduinoSerial<> marcduinoSerial(player);

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

void reboot()
{
    DEBUG_PRINTLN("Restarting...");
    unmountFileSystems();
    preferences.end();
    ESP.restart();
}

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

#endif

////////////////////////////////

#ifdef USE_WIFI_WEB
#include "WebPages.h"
#endif

////////////////////////////////

void setup()
{
    REELTWO_READY();

    if (!preferences.begin("astro", false))
    {
        DEBUG_PRINTLN("Failed to init prefs");
    }
    wifiEnabled = wifiActive = preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED);
    remoteEnabled = remoteActive = preferences.getBool(PREFERENCE_REMOTE_ENABLED, REMOTE_ENABLED);
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
    SetupEvent::ready();

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

    if (remoteEnabled)
    {
    #ifdef USE_SMQ
        WiFi.mode(WIFI_MODE_STA);
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
    else if (wifiEnabled)
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
                // Marcduino::processCommand(player, cmd);
                printf("PROCESS: \"%s\"\n", cmd);
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
#ifdef USE_WIFI_WEB
    // For safety we will stop the motors if the web client is connected
    webServer.setConnect([]() {
        // Callback for each connected web client
        // DEBUG_PRINTLN("Hello");
    });
#endif

#ifdef USE_WIFI
    xTaskCreatePinnedToCore(
          eventLoopTask,
          "Events",
          5000,    // shrink stack size?
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
    if (wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, false);
        DEBUG_PRINT("Disabling WiFi. ");
        reboot();
    }
}))

////////////////

MARCDUINO_ACTION(WifiHiHi, #WIHI, ({
    if (!wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, true);
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, false);
        DEBUG_PRINT("Enabling WiFi. ");
        reboot();
    }
}))

////////////////

MARCDUINO_ACTION(RemoteByeBye, #WIREMOTEBYE, ({
    if (remoteEnabled)
    {
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, false);
        DEBUG_PRINT("Disabling droid remote. ");
        reboot();
    }
}))

////////////////

MARCDUINO_ACTION(RemoteHiHI, #WIREMOTEHI, ({
    if (!remoteEnabled)
    {
        preferences.putBool(PREFERENCE_REMOTE_ENABLED, true);
        preferences.putBool(PREFERENCE_WIFI_ENABLED, false);
        DEBUG_PRINT("Enabling droid remote. ");
        reboot();
    }
}))

////////////////

#ifdef USE_SMQ
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

void mainLoop()
{
    AnimatedEvent::process();
#ifdef USE_MENUS
    sDisplay.process();
#endif
}

////////////////

#ifdef USE_WIFI
void eventLoopTask(void* )
{
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
    else if (remoteActive)
    {
    #ifdef USE_SMQ
        SMQ::process();
    #endif
    }
#else
    mainLoop();
 #ifdef ESP32
    vTaskDelay(1);
 #endif
#endif
}
