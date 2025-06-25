#include "ptl.h"

PTL_Globs ptlGlobs = { 0 };

B32 PTL_Initialize(const char* filename, const char* gameName)
{
    ptlGlobs.count = 0;
    lpConfig prop = Config_Load(filename);
    if (prop != NULL)
    {
        lpConfig conf = Config_FindArray(prop, gameName);
        U32 i = ptlGlobs.count;
        while (conf != NULL)
        {
            ptlGlobs.count = i + 1;
            ptlGlobs.table[i].fromType = Message_LookupPTLEventIndex(conf->itemName);
            ptlGlobs.table[i].toType = Message_LookupPTLEventIndex(conf->dataString);
            conf = Config_GetNextItem(conf);
            i = ptlGlobs.count;
        }
        ptlGlobs.count = i;

        Config_Free(prop);
        return TRUE;
    }
    return FALSE;
}
