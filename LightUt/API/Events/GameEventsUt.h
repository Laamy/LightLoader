#pragma once

#include <windows.h>
#include <iostream>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <cstdint>

#ifdef LIGHT_EXPORTS
#define GAMEEVENTSUT_API __declspec(dllexport)
#else
#define GAMEEVENTSUT_API __declspec(dllimport)
#endif

#include <Events/GameEvents.h>

#include "BaseEventUt.h"

#include "Event/RakNetSendToServer.h"
