#include "GameEvents.h"

static std::mutex mutex{};
static std::unordered_map<EventID, std::vector<ListenerRecord>> _listeners{};

inline size_t GameEvents::subscribe(EventID id, std::function<void(BaseEvent*)> cb) {
    static size_t tokenCount = 0;
    std::lock_guard<std::mutex> lock(mutex);

    if (_listeners.find(id) == _listeners.end())
        _listeners[id] = std::vector<ListenerRecord>();

    size_t token = tokenCount++;

    _listeners[id].push_back({ token, std::move(cb) });
    return token;
}

inline void GameEvents::unsubscribe(EventID id, size_t token) {
    std::lock_guard<std::mutex> lock(mutex);
    auto it = _listeners.find(id);
    if (it == _listeners.end())
        throw std::exception("You need to subscribe to an event to unsubscribe");

    auto& ting = it->second;

    ting.erase(std::remove_if(ting.begin(), ting.end(),
        [token](const ListenerRecord& r) { return r.token == token; }), ting.end());
}

inline void GameEvents::dispatch(BaseEvent* ev) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& rec : _listeners[ev->id()])
        rec.callback(ev);
}
