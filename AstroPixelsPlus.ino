/*****
 * 
 * AstroPixelsPlus sketch operates as a I2C master that can optionally be connected to one or more
 * Adafruit PCA9685 servo controllers to control dome panels. The sketch also provides serial commands
 * on Serial2.
 * 
 */

#define USE_DEBUG
#define USE_WIFI
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

#include "ReelTwo.h"
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

////////////////////////////////

// Replace with your network credentials
#ifdef USE_WIFI
#define WIFI_ENABLED         true
// Set these to your desired credentials.
#define WIFI_AP_NAME         "AstroPixels"
#define WIFI_AP_PASSPHRASE   "Astromech"
#define WIFI_ACCESS_POINT    true  /* true if access point: false if joining existing wifi */
#endif

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

#ifdef USE_WIFI
WifiAccess wifiAccess;
TaskHandle_t webTask;
bool otaInProgress;
#endif

#ifdef USE_WIFI_WEB
#include "WebPages.h"
#endif

#ifdef USE_WIFI_MARCDUINO
WifiMarcduinoReceiver wifiMarcduinoReceiver(wifiAccess);
#endif

////////////////////////////////

void setup()
{
    REELTWO_READY();

    if (!preferences.begin("astro", false))
    {
        DEBUG_PRINTLN("Failed to init prefs");
    }
#ifdef USE_WIFI_WEB
    wifiAccess.setNetworkCredentials(
        preferences.getString(PREFERENCE_WIFI_SSID, WIFI_AP_NAME),
        preferences.getString(PREFERENCE_WIFI_PASS, WIFI_AP_PASSPHRASE),
        preferences.getBool(PREFERENCE_WIFI_AP, WIFI_ACCESS_POINT),
        preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED));
#endif
#ifdef USE_WIFI_MARCDUINO
    wifiMarcduinoReceiver.setEnabled(preferences.getBool(PREFERENCE_MARCWIFI_ENABLED, MARC_WIFI_ENABLED));
    if (wifiMarcduinoReceiver.enabled())
    {
        wifiMarcduinoReceiver.setCommandHandler([](const char* cmd) {
            DEBUG_PRINTLN(cmd);
            Marcduino::processCommand(player, cmd);
            if (preferences.getBool(PREFERENCE_MARCWIFI_SERIAL_PASS, MARC_WIFI_SERIAL_PASS) &&
                preferences.getBool(PREFERENCE_MARCSERIAL_ENABLED, MARC_SERIAL_ENABLED) &&
                preferences.getBool(PREFERENCE_MARCSERIAL_PASS, MARC_SERIAL_PASS))
            {
                Serial2.print(cmd); Serial2.print('\r');
            }
        });
    }
#endif
    if (preferences.getBool(PREFERENCE_MARCSERIAL_ENABLED, MARC_SERIAL_ENABLED))
    {
        Serial2.begin(preferences.getInt(PREFERENCE_MARCSERIAL2, MARC_SERIAL2_BAUD_RATE));//, SERIAL_8N1, SERIAL2_RX_PIN, SERIAL2_TX_PIN);
        // if (preferences.getBool(PREFERENCE_MARCSERIAL_PASS, MARC_SERIAL_PASS))

        marcduinoSerial.setStream(&Serial2);
    }
    if (!mountReadOnlyFileSystem())
    {
        DEBUG_PRINTLN("Failed to mount read only filesystem");
    }

    Wire.begin();

    SetupEvent::ready();
    RLD.selectScrollTextLeft("... AstroPixels ....", LogicEngineRenderer::kBlue, 0, 15);
    FLD.selectScrollTextLeft("... R2D2 ...", LogicEngineRenderer::kRed, 0, 15);

#ifdef USE_WIFI_WEB
    wifiAccess.notifyWifiConnected([](WifiAccess &wifi) {
        Serial.print("Connect to http://"); Serial.println(wifi.getIPAddress());
    #ifdef USE_MDNS
        // No point in setting up mDNS if R2 is the access point
        if (!wifi.isSoftAP())
        {
            String mac = wifi.getMacAddress();
            String hostName = mac.substring(mac.length()-5, mac.length());
            hostName.remove(2, 1);
            hostName = "WReactor32-"+hostName;
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
    // For safety we will stop the motors if the web client is connected
    webServer.setConnect([]() {
        // Callback for each connected web client
        // DEBUG_PRINTLN("Hello");
    });
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
    bool wifiEnabled = preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED);
    if (wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, false);
        preferences.end();
        DEBUG_PRINTLN("Enabling WiFi. Restarting");
        ESP.restart();
    }
}))

////////////////

MARCDUINO_ACTION(WifiHiHi, #WIHI, ({
    bool wifiEnabled = preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED);
    if (!wifiEnabled)
    {
        preferences.putBool(PREFERENCE_WIFI_ENABLED, true);
        preferences.end();
        DEBUG_PRINTLN("Disabling WiFi. Restarting");
        ESP.restart();
    }
}))

////////////////

void loop()
{
#ifdef USE_OTA
    if (otaInProgress)
        ArduinoOTA.handle();
#endif
    if (!otaInProgress)
    {
        AnimatedEvent::process();
    }
#ifdef USE_WIFI_WEB
    if (preferences.getBool(PREFERENCE_WIFI_ENABLED, WIFI_ENABLED))
    {
        webServer.handle();
    }
#endif
}