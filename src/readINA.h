#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <INA233.h>
#include <Common.h>
#include <EspComms.h>

namespace readINA
{
  void init();
  void readAndSend();
  void print();
}