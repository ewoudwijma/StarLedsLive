/*
   @title     StarBase
   @file      UserModLive.h
   @date      20240411
   @repo      https://github.com/ewowi/StarBase, submit changes to this file as PRs to ewowi/StarBase
   @Authors   https://github.com/ewowi/StarBase/commits/main
   @Copyright © 2024 Github StarBase Commit Authors, asmParser © https://github.com/hpwit/ASMParser
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
*/


#define __RUN_CORE 0

#define NUM_LEDS_PER_STRIP 256
#define NUMSTRIPS 1
#define NUM_LEDS (NUM_LEDS_PER_STRIP * NUMSTRIPS)
#include "FastLED.h"
// #include "I2SClocklessLedDriver.h"
#include "parser.h"

CRGB leds[NUMSTRIPS * NUM_LEDS_PER_STRIP];

// int pins[NUMSTRIPS] = {23};
#define DATA_PIN 23
// I2SClocklessLedDriver driver;

static void clearleds()
{
    memset(leds, 0, NUM_LEDS * 3);
}
long time1;
static float _min = 9999;
static float _max = 0;
static uint32_t _nb_stat = 0;
static float _totfps;
static void show()
{
    // SKIPPED: check nargs (must be 3 because arg[0] is self)
    long time2 = ESP.getCycleCount();
    // driver.showPixels(WAIT);
    FastLED.show();
    float k = (float)(time2 - time1) / 240000000;
    float fps = 1 / k;
    _nb_stat++;
    if (_min > fps && fps > 10 && _nb_stat > 10)
        _min = fps;
    if (_max < fps && fps < 200 && _nb_stat > 10)
        _max = fps;
    if (_nb_stat > 10)
        _totfps += fps;
    // Serial.printf("current fps:%.2f  average:%.2f min:%.2f max:%.2f\r\n", fps, _totfps / (_nb_stat - 10), _min, _max);
    time1 = ESP.getCycleCount();

    // SKIPPED: check that both v1 and v2 are int numbers
    // RETURN_VALUE(VALUE_FROM_INT(0), rindex);
}
static CRGB POSV(uint8_t h, uint8_t s, uint8_t v)
{
    return CHSV(h, s, v);
}
static void resetShowStats()
{
    float min = 999;
    float max = 0;
    _nb_stat = 0;
    _totfps = 0;
}

class UserModLive:public SysModule {

public:

  Parser p = Parser();

  UserModLive() :SysModule("Live") {
    isEnabled = false; //need to enable after fresh setup
  };

  void setup() override {
    SysModule::setup();

    parentVar = ui->initUserMod(parentVar, name, 6310);

    ui->initSelect(parentVar, "script", UINT16_MAX, false ,[this](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onUI: {
        // ui->setComment(var, "Fixture to display effect on");
        JsonArray options = ui->setOptions(var);
        options.add("None");
        files->dirToJson(options, true, ".sc"); //only files containing F(ixture), alphabetically

        return true; }
      case onChange: {
        //set script
        uint8_t fileNr = var["value"];

        SCExecutable._kill(); //kill any old tasks

        ppf("%s script f:%d f:%d\n", name, funType, fileNr);

        if (fileNr > 0) { //not None

          fileNr--;  //-1 as none is no file

          char fileName[32] = "";

          files->seqNrToName(fileName, fileNr, ".sc");

          // ppf("%s script f:%d f:%d\n", name, funType, fileNr);

          if (strcmp(fileName, "") != 0) {

            File f = files->open(fileName, "r");
            if (!f)
              ppf("UserModLive setup script  open %s for %s failed", fileName, "r");
            else {

              string script = string(f.readString().c_str());

              ppf("%s\n", script);

              if (p.parse_c(&script))
              {
                  SCExecutable.executeAsTask("main");
              }
              f.close();
            }
          }
          else
            ppf("UserModLive setup file for %d not found", fileNr);
        }
        return true; }
      default: return false; 
    }}); //fixture

    // ui->initButton

    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(30);
    addExternal("leds", externalType::value, (void *)leds);
    addExternal("show", externalType::function, (void *)&show);
    addExternal("hsv", externalType::function, (void *)POSV);
    addExternal("clear", externalType::function, (void *)clearleds);
    addExternal("resetStat", externalType::function, (void *)&resetShowStats);

  }

};

extern UserModLive *live;


//asm_parser.h:325:1: warning: control reaches end of non-void function 