// Personal-use rebuild based on the public predictive-input-buffer recreation.
// Logs the current stage and buffer values to the Geode console when O is pressed; no in-game HUD.
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <deque>
#include <cstdio>
#ifdef _WIN32
#include <Windows.h>
#endif

using namespace geode::prelude;

namespace pib {
    struct Stage { int all; int wave; };
    constexpr Stage STAGES[4] = {{2, 2}, {4, 4}, {8, 8}, {16, 8}};
    constexpr int OFF = 4;
    inline int g_stage = 0;
    inline bool verbose() {
        return Mod::get()->getSettingValue<bool>("verbose-log");
    }
    inline void openConsole() {
#ifdef _WIN32
        if (GetConsoleWindow() == nullptr) {
            if (!AllocConsole()) return;
        }
        SetConsoleTitleA("Predictive Input Buffer");
        FILE* stream = nullptr;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        std::printf("Predictive Input Buffer console\nPress O in-level to cycle stages.\n");
        std::fflush(stdout);
#endif
    }

    inline void logStage() {
        if (g_stage == OFF) {
            std::printf("\n========== PREDICTIVE INPUT BUFFER ==========\nCURRENT STAGE: OFF\nGeneral buffer: OFF (raw input)\nWave buffer: OFF (raw input)\nPress O to change stage\n=============================================\n");
        } else {
            auto s = STAGES[g_stage];
            std::printf("\n========== PREDICTIVE INPUT BUFFER ==========\nCURRENT STAGE: %d\nGeneral buffer: %d ticks (%.2f frames @ 60 FPS)\nWave buffer: %d ticks (%.2f frames @ 60 FPS)\nPress O to change stage\n=============================================\n", g_stage + 1, s.all, s.all / 4.0f, s.wave, s.wave / 4.0f);
        }
        std::fflush(stdout);
    }
}

struct PendingInput { bool down; int button; bool player1; int remaining; };

class $modify(PIBBaseGameLayer, GJBaseGameLayer) {
    struct Fields { std::deque<PendingInput> queue; bool applying = false; };

    void handleButton(bool down, int button, bool isPlayer1) {
        auto f = m_fields.self();
        if (f->applying || pib::g_stage == pib::OFF) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }
        auto* player = isPlayer1 ? m_player1 : m_player2;
        bool isWave = player && player->m_isDart;
        auto stage = pib::STAGES[pib::g_stage];
        int delay = isWave ? stage.wave : stage.all;
        f->queue.push_back({down, button, isPlayer1, delay});
        if (pib::verbose()) {
            log::info("*** BUFFERED +{} TICKS ({:.2f} frames @ 60fps) {}! [Limit: {}] ***", delay, delay / 4.0f, down ? "PRESS" : "RELEASE", delay);
        }
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        auto f = m_fields.self();
        for (auto it = f->queue.begin(); it != f->queue.end();) {
            it->remaining -= 1;
            if (it->remaining <= 0) {
                f->applying = true;
                GJBaseGameLayer::handleButton(it->down, it->button, it->player1);
                f->applying = false;
                it = f->queue.erase(it);
            } else ++it;
        }
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
    }
};

class $modify(PIBPlayLayer, PlayLayer) {
    void resetLevel() {
        static_cast<PIBBaseGameLayer*>(static_cast<GJBaseGameLayer*>(this))->m_fields->queue.clear();
        PlayLayer::resetLevel();
    }
};

class $modify(PIBKeyboard, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat, double timestamp) {
        if (down && !repeat && key == KEY_O) {
            pib::g_stage = (pib::g_stage + 1) % 5;
            pib::logStage();
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, timestamp);
    }
};

$on_mod(Loaded) {
    pib::openConsole();
    pib::logStage();
    log::info("[InputBuffer] Loaded. A dedicated Windows console is requested; stage status is printed there when O is pressed.");
}
