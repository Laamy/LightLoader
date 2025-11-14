#include "GameEvents.h"

static std::mutex mutex{};
static std::unordered_map<EventID, std::vector<ListenerRecord>> _listeners{};

static bool isReadonly = false;

inline size_t GameEvents::subscribe(EventID id, std::function<void(BaseEvent*)> cb) {
    if (isReadonly)
		throw std::logic_error("Event system is locked and readonly");

    static size_t tokenCount = 0;
    std::lock_guard<std::mutex> lock(mutex);

    if (_listeners.find(id) == _listeners.end())
        _listeners[id] = std::vector<ListenerRecord>();

    size_t token = tokenCount++;

    _listeners[id].push_back({ token, std::move(cb) });
    return token;
}

inline void GameEvents::unsubscribe(EventID id, size_t token) {
    if (isReadonly)
		throw std::logic_error("Event system is locked and readonly");

    std::lock_guard<std::mutex> lock(mutex);
    auto it = _listeners.find(id);
    if (it == _listeners.end())
        throw std::exception("You need to subscribe to an event to unsubscribe");

    auto& ting = it->second;

    ting.erase(std::remove_if(ting.begin(), ting.end(),
        [token](const ListenerRecord& r) { return r.token == token; }), ting.end());
}

inline void GameEvents::dispatch(BaseEvent* ev) {
    if (!isReadonly)
        return;
    //if (!isReadonly)
	//	throw std::logic_error("Event system must be locked before dispatching events (Dispatch while or after load event)");
    
    //std::lock_guard<std::mutex> lock(mutex);
    auto it = _listeners.find(ev->id());
    if (it == _listeners.end())
        return;

    for (auto& rec : it->second)
        rec.callback(ev);
}

void GameEvents::lock()
{
	isReadonly = true;
}
