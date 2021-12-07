/**
 * ====================
 *     Zombie Riot
 *   File: humandata.inc
 *   Author: Oylsister
 * ==================== 
 */

enum struct ZRiot_HumanData
{
    bool human_enable;
    char human_name[32];
    char human_description[128];
    bool human_default;
    char human_model[256];
    char human_arms[256]; // csgo only
    char human_hvision[256];
    int human_health;
    float human_speed;
    float human_gravity;
    float human_jump;
    int human_fov;
    char human_adminflags[64];
}

#define MAXHUMAN 25

Handle g_hHumanClassCookie = INVALID_HANDLE;

int g_iSelectedClass[MAXPLAYERS+1];

Handle kvHumans = INVALID_HANDLE;

ZRiot_HumanData arrayHumans[MAXHUMAN];
int hCount;

void LoadHumanData(bool defaultconfig)
{
    char path[PLATFORM_MAX_PATH];
    Format(path, sizeof(path), "%s/humans.txt", gMapConfig);
    
    if (!defaultconfig && !FileExists(path))
    {
        return;
    }
    
    if (kvHumans != INVALID_HANDLE)
    {
        CloseHandle(kvHumans);
    }
    
    kvHumans = CreateKeyValues("humans");
    
    if (!FileToKeyValues(kvHumans, path))
    {
        SetFailState("\"%s\" failed to load", path);
    }
    
    KvRewind(kvHumans);
    if (!KvGotoFirstSubKey(kvHumans))
    {
        SetFailState("No zombie data defined in \"%s\"", path);
    }

    char name[64];
    char enable[32];
    char desc[128];
    char sDefault[32];
    char model[256];
    char arms[256];
    char hvision[256];
    char adminflags[64];

    hCount = 0;

    do
    {
        KvGetSectionName(kvHumans, name, sizeof(name));
        strcopy(arrayHumans[hCount].human_name, 64, name);

        KvGetString(kvHumans, "enable", enable, sizeof(enable));
        arrayHumans[hCount].human_enable = GetEnableDataBool(enable);

        KvGetString(kvHumans, "description", desc, sizeof(desc));
        strcopy(arrayHumans[hCount].human_description, 128, desc);

        KvGetString(kvHumans, "default_class", sDefault, sizeof(sDefault));
        arrayHumans[hCount].human_default = = GetEnableDataBool(sDefault);

        KvGetString(kvHumans, "model", model, sizeof(model));
        strcopy(arrayHumans[hCount].human_model, 256, model);

        KvGetString(kvHumans, "arms", arms, sizeof(arms));
        strcopy(arrayHumans[hCount].human_arms, 256, arms);

        KvGetString(kvHumans, "hvision", hvision, sizeof(hvision));
        strcopy(arrayHumans[hCount].human_hvision, 256, hvision);

        arrayHumans[hCount].human_health = KvGetNum(kvHumans, "health", 100);

        arrayHumans[hCount].human_speed = KvGetFloat(kvHumans, "speed", 300.0);

        arrayHumans[hCount].human_gravity = KvGetFloat(kvHumans, "gravity", 1.0);

        arrayHumans[hCount].human_jump = KvGetFloat(kvHumans, "jump", 1.0);

        arrayHumans[hCount].human_fov = KvGetNum(kvHumans, "fov", 90);

        KvGetString(kvHumans, "sm_flags", adminflags, sizeof(adminflags));
        strcopy(arrayHumans[hCount].human_adminflags, 64, adminflags);

        hCount++;
    } while (KvGotoNextKey(kvHumans));
}

int FindHumanIDByName(const char[] name)
{
    for(int x = 0; x < hCount; x++)
    {
        if(StrEqual(name, arrayHumans[x].human_name, false))
        {
            return x;
        }
    }

    return -1;
}

bool IsValidHumanID(int humanid)
{
    if(humanid > -1 && humanid < hCount)
    {
        return true;
    }

    return false;
}

public Action HumanSpawnPost(Handle timer, any client)
{
    return Plugin_Handled;
}

void ApplyHumanModel(int client, int humanid)
{
    if(IsValidHumanID(humanid))
    {
        char model[256];
        strcopy(model, sizeof(model), arrayHumans[humanid].human_model);

        if(!StrEqual(model, "default", false) || model[0] == '\0')
        {
            PrecacheModel(model)
            SetEntityModel(client, model);
        }
    }
}

void ApplyHumanArms(int client, int humanid)
{
    if(!csgo)
    {
        return;
    }

    if(IsValidHumanID(humanid))
    {
        char model[256];
        strcopy(model, sizeof(model), arrayHumans[humanid].human_arms);

        if(!StrEqual(model, "default", false) || model[0] == '\0')
        {
            PrecacheModel(model)

            int arms = GetEntProp(client, Prop_Send, "m_hMyWearables");
            if(arms != -1)
            {
                AcceptEntityInput(arms, "KillHierarchy");
            }

            SetEntPropString(client, Prop_Send, "m_szArmsModel", model);
        }
    }
}

void GetHumanHVision(int humanid, char[] hvision, int maxlen)
{
    if(IsValidHumanID(humanid))
    {
        strcopy(hvision, maxlen, arrayHumans[humanid].human_hvision);
    }
}

void ApplyHumanHealth(int client, int humanid)
{
    if(IsValidHumanID(humanid))
    {
        SetEntityHealth(client, arrayHumans[humanid].human_health);
    }
}

void ApplyHumanSpeed(int client, int humanid)
{
    if(IsValidHumanID(humanid))
    {
        SetPlayerSpeed(client, arrayHumans[humanid].human_speed);
    }
}

void ApplyHumanGravity(int client, int humanid)
{
    if(IsValidHumanID(humanid))
    {
        SetEntityGravity(client, arrayHumans[humanid].human_gravity);
    }
}

float GetHumanJump(int humanid)
{
    if(IsValidHumanID(humanid))
    {
        return arrayHumans[humanid].human_jump;
    }

    return 0.0;
}

void ApplyHumanFOV(int client, int humanid)
{
    if(IsValidHumanID(humanid))
    {
        SetPlayerFOV(client, arrayHumans[humanid].human_fov);
    }
}

void HumanClassInit()
{
    if(g_hHumanClassCookie == INVALID_HANDLE)
    {
        g_hHumanClassCookie = RegClientCookie("zriot_humanclass", "[ZRiot] Human Class Cookies", CookieAccess_Protected);
    }
}

void HumanClassOnCookiesCahced(int client)
{
    char sBuffer[32];
    GetClientCookie(client, g_hHumanClassCookie, sBuffer, sizeof(sBuffer));

    if(sBuffer[0] != '\0')
    {
        g_iSelectedClass[client] = FindHumanIDByName(sBuffer);
    }
    else
    {
        char sDefault[64];
        GetHumanDefaultClass(sDefault, sizeof(sDefault));

        g_iSelectedClass[client] = FindHumanIDByName(sDefault);
        SaveHumanClassCookie(client, g_iSelectedClass[client]);
    }
}

void GetHumanDefaultClass(const char[] name, int maxlen)
{
    for (int x = 0; x < hCount; h++)
    {
        if(arrayHumans[x].human_default)
        {
            strcopy(name, maxlen, arrayHumans[x].human_name);
        }
    }
}

void SaveHumanClassCookie(int client, int humanid)
{
    char sCookie[64];
    FormatEx(sCookie, sizeof(sCookie), "%d", g_iSelectedClass[client]);
    SetClientCookie(client, g_hHumanClassCookie, sCookie);
}

bool GetEnableDataBool(const char[] text)
{
    if(StrEqual(text, "yes", false))
    {
        return true;
    }

    else if(StrEqual(text, "no", false))
    {
        return false
    }

    return false;
}
