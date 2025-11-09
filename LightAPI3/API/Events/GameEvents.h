#pragma once

#include <windows.h>
#include <iostream>
#include <functional>
#include <mutex>
#include <unordered_map>
#include <cstdint>

#ifdef LIGHT_EXPORTS
#define GAMEEVENTS_API __declspec(dllexport)
#else
#define GAMEEVENTS_API __declspec(dllimport)
#endif

#include "BaseEvent.h"
#include "Event/WndProcEvent.h"
#include "Event/LoaderFinishedEvent.h"

// All events here are sigless and wont ever break
class GAMEEVENTS_API GameEvents {
public:
    static size_t subscribe(EventID id, std::function<void(BaseEvent*)> cb);
    static void unsubscribe(EventID id, size_t token);

    static void dispatch(BaseEvent* ev);
};
