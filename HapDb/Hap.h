#ifndef _HAP_H_
#define _HAP_H_

#define sizeofarr(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <stdio.h>
#include <stdint.h>

#define Log printf

#include "HapJson.h"
#include "HapDb.h"
#include "HapAppleCharacteristics.h"
#include "HapAppleServices.h"

#endif