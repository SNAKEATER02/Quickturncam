#include <algorithm>
#include <cmath>
#include <cstdio>

#include <mod/amlmod.h>
#include <mod/config.h>
#include <mod/logger.h>
#include <mod/menu/menu.h>

#include <plugin-sdk/game_sa/engine/Cam.h>
#include <plugin-sdk/game_sa/engine/Pad.h>

MYMODCFG(quickturncamera, QuickTurnCamera, 1.0, OpenAI)

namespace {
using FollowCarFn = void (*)(CCam *, const CVector &, float, float, float, bool);

FollowCarFn g_originalFollowCar = nullptr;
ConfigEntry *g_turnSpeedConfig = nullptr;
float g_turnSpeedMultiplier = 2.0f;

constexpr int kSliderMin = 10; // 1.0x
constexpr int kSliderMax = 50; // 5.0x
constexpr int kSteeringDeadzone = 4;

void Hook_ProcessFollowCar(CCam *cam, const CVector &target, float targetOrientation,
                           float speedVar, float speedVarDesired, bool scriptSetAngles) {
    // GetSteeringLeftRight reports the vehicle steering input (including touch controls).
    // Leave both original camera speed values untouched while the player is not steering.
    CPad *pad = CPad::GetPad(0);
    const int steering = pad ? static_cast<int>(pad->GetSteeringLeftRight()) : 0;

    if (std::abs(steering) > kSteeringDeadzone) {
        speedVar *= g_turnSpeedMultiplier;
        speedVarDesired *= g_turnSpeedMultiplier;
    }

    if (g_originalFollowCar) {
        g_originalFollowCar(cam, target, targetOrientation, speedVar, speedVarDesired, scriptSetAngles);
    }
}

void OnTurnSpeedChanged(int, int newValue, void *) {
    const int clamped = std::clamp(newValue, kSliderMin, kSliderMax);
    g_turnSpeedMultiplier = static_cast<float>(clamped) / 10.0f;

    if (g_turnSpeedConfig) {
        g_turnSpeedConfig->SetFloat(g_turnSpeedMultiplier);
        cfg->Save();
    }
}

const char *FormatTurnSpeed(int value, void *) {
    static char text[16];
    std::snprintf(text, sizeof(text), "%.1fx", static_cast<float>(value) / 10.0f);
    return text;
}

void RegisterSettings() {
    float stored = g_turnSpeedConfig ? g_turnSpeedConfig->GetFloat() : 2.0f;
    if (!std::isfinite(stored))
        stored = 2.0f;

    g_turnSpeedMultiplier = std::clamp(stored, 1.0f, 5.0f);
    if (g_turnSpeedConfig && g_turnSpeedMultiplier != stored) {
        g_turnSpeedConfig->SetFloat(g_turnSpeedMultiplier);
        cfg->Save();
    }

    const int initial = static_cast<int>(std::lround(g_turnSpeedMultiplier * 10.0f));
    Menu::AddSlider(Menu::Tab::Controls, "Camera turn speed", kSliderMin, kSliderMax, initial,
                    OnTurnSpeedChanged, nullptr, FormatTurnSpeed);
}
} // namespace

ON_MOD_LOAD() {
    if (logger) {
        logger->SetTag("Quick Turn Camera");
        logger->Info("Quick Turn Camera loading...");
    }

    g_turnSpeedConfig = cfg->Bind("TurnSpeed", 2.0f, "Settings");
    if (g_turnSpeedConfig) {
        float stored = g_turnSpeedConfig->GetFloat();
        if (!std::isfinite(stored))
            stored = 2.0f;
        g_turnSpeedMultiplier = std::clamp(stored, 1.0f, 5.0f);
        if (g_turnSpeedMultiplier != stored) {
            g_turnSpeedConfig->SetFloat(g_turnSpeedMultiplier);
            cfg->Save();
        }
    }

    const uintptr_t followCarAddress = GET_THISCALL_ADDR(CCam, Process_FollowCar_SA);
    const bool hookInstalled =
        aml && followCarAddress &&
        aml->Hook(reinterpret_cast<void *>(followCarAddress),
                  reinterpret_cast<void *>(Hook_ProcessFollowCar),
                  reinterpret_cast<void **>(&g_originalFollowCar));

    if (logger) {
        if (hookInstalled)
            logger->Info("Hooked CCam::Process_FollowCar_SA at %p", reinterpret_cast<void *>(followCarAddress));
        else
            logger->Error("Could not hook CCam::Process_FollowCar_SA; camera behaviour is unchanged");
    }

    Menu::Init();
    RegisterSettings();

    if (logger)
        logger->Info("Quick Turn Camera loaded");
}
