/*
 * Conf_EPD.cpp
 * Copyright (C) 2019-2022 Linar Yusupov
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// this is modified from Text_EPD.

#include "../system/SoC.h"

#if defined(USE_EPAPER)

#include "../driver/EPD.h"

#include <TimeLib.h>

#include "../TrafficHelper.h"
#include "../driver/EEPROM.h"
#include "../protocol/data/NMEA.h"
#include "../protocol/data/GDL90.h"
#include "../driver/GNSS.h"
#include "../driver/LED.h"
#include "../driver/RF.h"

#include <protocol.h>
#include "../protocol/radio/Legacy.h"

#include <gfxfont.h>
#include <FreeMonoBold9pt7b.h>

static void EPD_Draw_Conf()
{
  char info_line [CONF_VIEW_LINE_LENGTH];
  char id_text   [CONF_VIEW_LINE_LENGTH];

#if defined(USE_EPD_TASK)
  if (EPD_update_in_progress != EPD_UPDATE_NONE)
      return;
#endif

    display->setFont(&FreeMonoBold9pt7b);

/*
Normal  Glider
Device: 123456
Aircft: 123456 >>
I:123456 F:123456
US TX P:LEG A:LEG
NMEA: USB GLD
*/
    {
      uint16_t x = 6;
      uint16_t y = 20;

      int16_t  tbx, tby;
      uint16_t tbw, tbh;

      display->fillScreen(GxEPD_WHITE);

      Serial.println();

      snprintf(info_line, sizeof(info_line), "%s  %s",
          (settings->mode == SOFTRF_MODE_NORMAL ? "Normal" : "Other"),
          Aircraft_Type[settings->aircraft_type]);
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      y += CONF_VIEW_LINE_SPACING;

      snprintf(info_line, sizeof(info_line), "%s %s P:%s A:%s",
          Region_Label[settings->band],
          (settings->txpower == RF_TX_POWER_FULL ? "TX" :
          (settings->txpower == RF_TX_POWER_LOW  ? "tx" : "--" )),
          Protocol_ID[settings->rf_protocol],
          (settings->alarm == TRAFFIC_ALARM_LEGACY ? "LEG" :
          (settings->alarm == TRAFFIC_ALARM_VECTOR ? "VCT" :
          (settings->alarm == TRAFFIC_ALARM_DISTANCE ? "DST" : "---"))));
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      y += CONF_VIEW_LINE_SPACING;

      if (settings->id_method == ADDR_TYPE_FLARM)
        snprintf(info_line, sizeof(info_line), "Device: %06X >>", ThisAircraft.addr);
      else
        snprintf(info_line, sizeof(info_line), "Device: %06X", SoC->getChipId() & 0x00FFFFFF);
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      y += CONF_VIEW_LINE_SPACING;

      if (settings->id_method == ADDR_TYPE_ICAO)
        snprintf(info_line, sizeof(info_line), "Aircft: %06X >>", ThisAircraft.addr);
      else
        snprintf(info_line, sizeof(info_line), "Aircft: %06X", settings->aircraft_id);
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      y += CONF_VIEW_LINE_SPACING;

      snprintf(info_line, sizeof(info_line), "--%06X ++%06X",
          settings->ignore_id, settings->follow_id);
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      y += CONF_VIEW_LINE_SPACING;

      char nmeas[6];
      int i = 0;
      if (settings->nmea_g)
        nmeas[i++] = 'G';
      if (settings->nmea_p)
        nmeas[i++] = 'P';
      if (settings->nmea_l)
        nmeas[i++] = 'L';
      if (settings->nmea_s)
        nmeas[i++] = 'S';
      if (settings->nmea_d)
        nmeas[i++] = 'D';
      if (i == 0)
        nmeas[i++] = '-';
      nmeas[i] = '\0';
      snprintf(info_line, sizeof(info_line), "NMEA:%s  %s",
          (settings->nmea_out == NMEA_UART ? "SER" :
          (settings->nmea_out == NMEA_USB  ? "USB" :
          (settings->nmea_out == NMEA_BLUETOOTH ? "BLT" : "---"))),
          nmeas);
      display->getTextBounds(info_line, 0, 0, &tbx, &tby, &tbw, &tbh);
      y += tbh;
      display->setCursor(x, y);
      display->print(info_line);
      Serial.println(info_line);

      Serial.println();
    }

#if defined(USE_EPD_TASK)
    /* a signal to background EPD update task */
    EPD_update_in_progress = EPD_UPDATE_FAST;
#else
    display->display(true);
#endif
}

void EPD_conf_setup() {}

void EPD_conf_loop()
{
  if (isTimeToEPD()) {
    if (EPD_prev_view != VIEW_MODE_CONF)   /* no need to draw same settings repeatedly */
      EPD_Draw_Conf();
  }
  EPDTimeMarker = millis();
}

void EPD_conf_next() {}

void EPD_conf_prev() {}


#endif /* USE_EPAPER */
