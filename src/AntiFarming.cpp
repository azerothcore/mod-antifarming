/*
- Made by mthsena -
- Edited by Jameyboor -
- Adapted and Fixed by Erictemponi -
*/

#include "AntiFarming.h"

class AntiFarmingPlayerScript : public PlayerScript {
public: AntiFarmingPlayerScript() : PlayerScript("AntiFarmingPlayerScript") {}

        void OnLogin(Player* player) override {
            if (sConfigMgr->GetBoolDefault("AntiFarming.Enable", true)) {
                if (sConfigMgr->GetBoolDefault("AntiFarming.LoginMessage", false))
                    ChatHandler(player->GetSession()).PSendSysMessage("This server is running the |cff4CFF00Antifarming|r Module.");
            }
        }

        void OnPVPKill(Player* killer, Player* killed) override {
            if (sConfigMgr->GetBoolDefault("AntiFarming.Enable", true)) {
                uint32 KillerGUID = killer->GetGUID();
                uint32 VictimGUID = killed->GetGUID();

                if (KillerGUID == VictimGUID) // Suicide
                    return;
				
                if (killer->GetSession()->GetRemoteAddress() == killed->GetSession()->GetRemoteAddress() || killed->GetMaxHealth() < (uint32)sConfigMgr->GetIntDefault("AntiFarming.MinHealth", 10000)) { // about 1/4 of the maximum hp a character can have on your server to make this fully functional.
                    sAntiFarming->dataMap[KillerGUID]++;
                    std::string str = "|cFFFFFC00[Anti-Farm System]|cFF00FFFF[|cFF60FF00" + killer->GetName() + "|cFF00FFFF] Possible Farmer!";
                    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
                    data << str;
                    sWorld->SendGlobalGMMessage(&data);

                    // Kick player
                    if (sConfigMgr->GetBoolDefault("AntiFarming.KickPlayer", true)) {
                        uint32 kickWarnings = sConfigMgr->GetIntDefault("AntiFarming.KickPlayer.Warnings", 5);
                        if (sAntiFarming->dataMap[KillerGUID] >= kickWarnings)
                            killer->GetSession()->KickPlayer();
                    }

                    // Ban player
                    if (sConfigMgr->GetBoolDefault("AntiFarming.BanPlayer", false)) {
                        std::string banTime = sConfigMgr->GetStringDefault("AntiFarming.BanTime", "1") + "d";
                        uint32 banWarnings = sConfigMgr->GetIntDefault("AntiFarming.BanPlayer.Warnings", 8);
                        if (sAntiFarming->dataMap[KillerGUID] >= banWarnings)
                            sBan->BanCharacter(killer->GetName(), banTime, "Ignored warnings of the Anti-Farm System", "Anti-Farm System");
                    }
                }
            }
        }
};

class AntiFarmingWorldScript : public WorldScript {
public:
    AntiFarmingWorldScript() : WorldScript("AntiFarmingWorldScript") { }

    void OnBeforeConfigLoad(bool reload) override {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/AntiFarming.conf";
            #ifdef WIN32
                cfg_file = "AntiFarming.conf";
            #endif // WIN32
            std::string cfg_def_file = cfg_file + ".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
    void OnAfterConfigLoad(bool /*reload*/) override {
        sLog->outString("Antifarming Module Loaded.");
    }
};

void AddAntiFarmingScripts() {
    new AntiFarmingPlayerScript();
    new AntiFarmingWorldScript();
    AddSC_antifarming_commandscript();
}
