#include <windows.h>
#include <stdio.h>
#include "resource/resource.h"
#include "registry.h"
#include "util.h"

// TODO: Cleanup Decompiled Code

typedef struct CLGenMode
{
    const char *displayName;
    const char *options;
} CLGenMode;

char *g_CLGenDat_Buffer;
unsigned int g_CLGenDat_FileSize;
char *g_CLGen_Title;
char *g_CLGen_Instruction;
char *g_CLGen_WriteKey;
CLGenMode *g_Modes_TABLE;
unsigned int g_Modes_COUNT;
unsigned int g_Modes_CAPACITY;
int g_Modes_CURSEL;

CLGenMode *__cdecl CLGen_AddModeItem(const char *modeName, const char *options)
{
    unsigned int capacity; // eax
    CLGenMode *result; // eax

    capacity = g_Modes_CAPACITY;
    if ( g_Modes_COUNT == g_Modes_CAPACITY )
    {
        g_Modes_CAPACITY += 10;
        g_Modes_TABLE = (CLGenMode *)realloc(g_Modes_TABLE, sizeof(CLGenMode) * (capacity + 10));
    }
    result = &g_Modes_TABLE[g_Modes_COUNT++];
    result->displayName = modeName;
    result->options = options;
    return result;
}

const char *__cdecl CLGen_ParseCommand(int numParts, char **parts)
{
    const char *result; // eax

    if ( !_strcmpi(*parts, "TITLE") )
    {
        if ( numParts == 2 )
        {
            result = 0;
            g_CLGen_Title = parts[1];
        }
        else
        {
            result = "Usage TITLE|<Window title text>";
        }
    }
    else if ( !_strcmpi(*parts, "INSTRUCTION") )
    {
        if ( numParts == 2 )
        {
            g_CLGen_Instruction = parts[1];
            result = 0;
        }
        else
        {
            result = "Usage INSTRUCTION|<Instruction message text>";
        }
    }
    else if ( !_strcmpi(*parts, "ADDITEM") )
    {
        if ( numParts == 3 )
        {
            CLGen_AddModeItem(parts[1], parts[2]);
            result = 0;
        }
        else
        {
            result = "Usage ADDITEM|<Mode Name>|<Options>";
        }
    }
    else if ( !_strcmpi(*parts, "ACTION") )
    {
        if ( !_strcmpi(parts[1], "WRITEKEY") )
        {
            if ( numParts == 3 )
            {
                g_CLGen_WriteKey = parts[2];
                result = 0;
            }
            else
            {
                result = "Usage ACTION|WRITEKEY|<registry key>";
            }
        }
        else
        {
            result = "Unknown action";
        }
    }
    else
    {
        result = *parts;
    }
    return result;
}

BOOL __cdecl CLGen_Open(const char *filename)
{
    FILE *file; // eax
    FILE *v2; // esi
    unsigned int v3; // esi
    char *v4; // edx
    unsigned int i; // eax
    char v6; // cl
    char *v7; // esi
    BOOL isEOL; // ecx
    unsigned int v9; // edi
    char v10; // dl
    int v11; // eax
    const char *v12; // eax
    char *Str2; // [esp+8h] [ebp-604h]
    char *stringParts[128]; // [esp+Ch] [ebp-600h] BYREF
    CHAR errorMessage[1024]; // [esp+20Ch] [ebp-400h] BYREF

    file = fopen(filename, "rb");
    v2 = file;
    if ( !file )
        return FALSE;
    fseek(file, 0, 2);
    g_CLGenDat_FileSize = ftell(v2);
    fseek(v2, 0, 0);
    g_CLGenDat_Buffer = (char *)malloc(g_CLGenDat_FileSize + 1);
    fread(g_CLGenDat_Buffer, 1u, g_CLGenDat_FileSize, v2);
    fclose(v2);
    v3 = g_CLGenDat_FileSize;
    v4 = g_CLGenDat_Buffer;
    for ( i = 0; i < v3; ++i )
    {
        v6 = v4[i];
        if ( v6 == '\r' || v6 == '\n' )
        {
            v4[i] = '\0';
            v4 = g_CLGenDat_Buffer;
            v3 = g_CLGenDat_FileSize;
        }
    }
    v4[v3] = '\0';
    //v7 = Str2;
    isEOL = TRUE;
    v9 = 0;
    do
    {
        v10 = g_CLGenDat_Buffer[v9];
        if ( isEOL )
        {
            if ( v10 )
            {
                v7 = &g_CLGenDat_Buffer[v9];
                isEOL = FALSE;
            }
        }
        else if ( !v10 )
        {
            v11 = Util_StringSplit(v7, stringParts, "|");
            v12 = CLGen_ParseCommand(v11, stringParts);
            if ( v12 )
            {
                sprintf(errorMessage, "Error in \"%s\":\n%s", filename, v12);
                MessageBoxA(0, errorMessage, "Error", 0);
            }
            isEOL = TRUE;
        }
        ++v9;
    }
    while ( v9 <= g_CLGenDat_FileSize );
    return TRUE;
}

void __cdecl CLGen_ApplyRegistryChanges(const CLGenMode *node)
{
    const char *v1; // esi
    const char *v2; // ebx
    char *buffer; // eax
    HKEY phkResult; // [esp+Ch] [ebp-404h]
    char Buffer[1024]; // [esp+10h] [ebp-400h] BYREF

    if ( g_CLGen_WriteKey )
    {
        v1 = 0;
        sprintf(Buffer, "%s", (const char *)g_CLGen_WriteKey);
        //v2 = (const char *)phkResult;
        buffer = Buffer;
        if ( Buffer[0] )
        {
            do
            {
                if ( *buffer == '\\' )
                {
                    if ( !v1 )
                    {
                        *buffer = '\0';
                        v2 = buffer + 1;
                    }
                    v1 = buffer + 1;
                }
            }
            while ( *++buffer );
        }
        *((unsigned char *)v1 - 1) = 0;
        if ( _strcmpi(Buffer, "HKEY_LOCAL_MACHINE") )
            MessageBoxA(0, "Can only write to HKEY_LOCAL_MACHINE", "Error", 0);
        else
            Registry_SetValue_OnLocalMachine(v2, v1, REGISTRY_STRING, node->options, strlen(node->options));
    }
}

BOOL __cdecl Selector_HandleMessage_InitDialog(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    HWND hCtrlCombobox; // ebx
    HWND hCtrlLabel; // edi
    unsigned int i; // edi

    hCtrlCombobox = GetDlgItem(hDlg, IDC_SELECT_DROPDOWN);
    hCtrlLabel = GetDlgItem(hDlg, IDC_SELECT_LABEL);
    SendMessageA(hDlg, WM_SETTEXT, 0, (LPARAM)g_CLGen_Title);
    SendMessageA(hCtrlLabel, WM_SETTEXT, 0, (LPARAM)g_CLGen_Instruction);
    while ( SendMessageA(hCtrlCombobox, CB_DELETESTRING, 0, 0) != -1 )
        ;
    for ( i = 0; i < g_Modes_COUNT; ++i )
        SendMessageA(hCtrlCombobox, CB_ADDSTRING, 0, (LPARAM)g_Modes_TABLE[i].displayName);
    SendMessageA(hCtrlCombobox, CB_SETCURSEL, 0, 0);
    return 0;
}

BOOL __cdecl Selector_HandleMessage_MenuCommand(HWND hDlg, WPARAM wParam)
{
    BOOL v2; // esi

    v2 = FALSE;
    if ( !(short)wParam || (unsigned __int16)wParam > (unsigned int)IDCANCEL )
        return v2;
    v2 = TRUE;
    EndDialog(hDlg, (unsigned __int16)wParam);
    return v2;
}

BOOL __cdecl Selector_HandleMessage_AcceleratorCommand(HWND hDlg, WORD idCaller, HWND hCtrl)
{
    BOOL v3; // esi

    v3 = FALSE;
    if ( idCaller != IDC_SELECT_DROPDOWN )
        return v3;
    v3 = TRUE;
    g_Modes_CURSEL = SendMessageA(hCtrl, CB_GETCURSEL, 0, 0);
    return v3;
}

BOOL __cdecl Selector_HandleMessage_Command(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
    BOOL result; // eax

    result = 0;
    if ( !HIWORD(wParam) )
        return Selector_HandleMessage_MenuCommand(hDlg, wParam);
    if ( HIWORD(wParam) == 1 )
        result = Selector_HandleMessage_AcceleratorCommand(hDlg, wParam, (HWND)lParam);
    return result;
}

INT_PTR __stdcall Selector_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    INT_PTR v4; // esi

    v4 = 0;
    switch ( message )
    {
        case WM_CLOSE:
            v4 = TRUE;
            EndDialog(hDlg, IDCANCEL);
            break;
        case WM_INITDIALOG:
            return Selector_HandleMessage_InitDialog(hDlg, wParam, lParam);
        case WM_COMMAND:
            return Selector_HandleMessage_Command(hDlg, wParam, lParam);
    }
    return v4;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CHAR errorMessage[1024];

    if ( CLGen_Open("CLGen.dat") )
    {
        g_Modes_CURSEL = g_Modes_COUNT;
        if ( DialogBoxParamA(hInstance, (LPCSTR)IDD_CLGEN_DIALOG, 0, Selector_DialogProc, 0) == IDOK
             && g_Modes_CURSEL != g_Modes_COUNT
             && g_Modes_CURSEL != -1 )
        {
            CLGen_ApplyRegistryChanges(&g_Modes_TABLE[g_Modes_CURSEL]);
            return 0;
        }
    }
    else
    {
        sprintf(errorMessage, "Cannot find data file \"%s\"", "CLGen.dat");
        MessageBoxA(NULL, errorMessage, "Error", MB_OK);
    }
    return 0;
}
