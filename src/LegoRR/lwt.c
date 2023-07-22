#include "lwt.h"
#include "mem.h"
#include "error.h"
#include <Windows.h>
#include <stdio.h>
#include <d3d.h>

char* lwExtractString(char* str1, char* str2, U32 *pos, U32 *len)
{
    U32 len1, len2;
    U32 i;
    char *str;

    len1 = strlen(str1);
    len2 = strlen(str2);

    if (!len1 || !len2)
        return NULL;

    for (i = 0; i < len1; i++)
    {
        str = str1 + i;
        if (!strnicmp(str, str2, len2))
        {
            if (pos)
                *pos = i;
            if (len)
                *len = len2;
            return str;
        }
    }

    return NULL;
}

char* stringAlloc(char* str)
{
    U32 len;
    char* sa;

    len = strlen(str) + 1;
    len += (len & 1);
    sa = (char*) Mem_Alloc(len);
    ZeroMemory(sa, len);
    strcpy(sa, str);
    return sa;
}

U32 texMapType(char* str)
{
    U32 ret, len;

    char* tTypes[] = {"PLANAR", "CYLINDRICAL", "SPHERICAL"};

    for (ret = 0; ret < MT_MAX; ret++)
    {
        len = strlen(tTypes[ret]);
        if (!strnicmp(str, tTypes[ret], len))
            break;
    }

    if (ret == MT_MAX)
        ret = 0;

    return ret;
}

void surfFree(LWSURFACE* srf, B32 flag)
{
    char *ptr;
    LWSURFACE *srfN;

    ptr = srf->srfName;
    if (ptr)
        Mem_Free(ptr);
    ptr = srf->srfPath;
    if (ptr)
        Mem_Free(ptr);
    srfN = srf->srfNextSurf;
    if (srfN)
        surfFree(srfN, TRUE);
    if (flag)
        Mem_Free(srf);
}

B32 LWD3D(char **p, D3DVALUE *f)
{
    char *fv;

    fv = (char*)f;

    fv[3] = *(*p)++;
    fv[2] = *(*p)++;
    fv[1] = *(*p)++;
    fv[0] = *(*p)++;

    return TRUE;
}

B32 PNTSprc(lpFile file, LWSIZE *sizeData, S32 csize, F32 **verts, B32 dflag)
{
    char *sp, *cp;
    S32 i = 0;
    char cbuff[10] = {0};
    char buff[256];
    D3DVECTOR pos;
    F32 *vptr;

    sp = Mem_Alloc(csize + 16);
    cp = sp;

    File_Read(sp, csize, 1, file);
    sizeData->lwVertCount = csize / 12;

    if (!verts)
    {
        Mem_Free(sp);
        return TRUE;
    }

    *verts = Mem_Alloc(csize + 4);
    vptr = *verts;

    while (csize > 0)
    {
        LWD3D(&cp, &pos.x);
        LWD3D(&cp, &pos.y);
        LWD3D(&cp, &pos.z);
        *vptr++ = pos.x;
        *vptr++ = pos.y;
        *vptr++ = pos.z;

        if (dflag)
        {
            sprintf(buff, "Point %d > %f %f %f\n", i, pos.x, pos.y, pos.z);
            Error_Debug(buff);
        }
        i++;
        csize -= 12;
    }

    Mem_Free(sp);
    return TRUE;
}

B32 CRVSprc(lpFile file, S32 csize, B32 dflag)
{
    char *sp, *cp;
    U16 *swp;
    U16 vcount, vcur, vsurf, vflags;

    sp = Mem_Alloc(csize + 16);
    cp = sp;
    swp = (U16*)sp;
    File_Read(sp, csize, 1, file);

    while (csize > 0)
    {
        vcount = LWSPVALUE(swp);
        swp++;
        csize -= 2;
        while (vcount)
        {
            vcur = LWSPVALUE(swp);
            swp++;
            csize -= 2;
            vcount--;
        }
        vsurf = LWSPVALUE(swp);
        vflags = LWSPVALUE(swp);
        swp++;
        swp++;
        csize -= 4;
    }
    Mem_Free(sp);
    return TRUE;
}

B32 POLSprc(lpFile file, LWSIZE *sizeData, LWPOLY **polys, S32 csize, B32 dflag)
{
    char *sp, *cp;
    U16 *swp;
    U16 vcount, vcur, vsurf;
    U16 pc;
    LWPOLY *ply;
    U16* plyd;
    char buff[256], buff2[64];

    sp = Mem_Alloc(csize + 16);
    cp = sp;
    swp = sp;
    File_Read(sp, csize, 1, file);
    pc = 0;
    while (csize > 0)
    {
        vcount = LWSPVALUE(swp);
        swp++;
        csize -= 2;

        if (polys)
        {
            if (!pc)
            {
                ply = Mem_Alloc(sizeof(LWPOLY));
                *polys = ply;
            } else
            {
                ply = Mem_Alloc((pc + 1) * sizeof(LWPOLY));
                memcpy(ply, *polys, (pc * sizeof(LWPOLY)));
                Mem_Free(*polys);
                *polys = ply;
                ply += pc;
            }
            plyd = Mem_Alloc(vcount * sizeof(U16));
            if (dflag)
                sprintf(buff, "Poly %d, Vertex Count %d > ", pc, vcount);
            ply->plyCount = vcount;
            ply->plyData = plyd;
            while (vcount)
            {
                vcur = LWSPVALUE(swp);
                if (dflag)
                {
                    sprintf(buff2, "%d ", vcur);
                    strcat(buff, buff2);
                }
                *plyd++ = vcur;
                swp++;
                csize -= 2;
                vcount--;
            }
            vsurf = LWSPVALUE(swp);
            vsurf--;
            ply->plySurface = vsurf;
            if (dflag)
            {
                sprintf(buff2, "Surface :- %d\n", vsurf);
                strcat(buff, buff2);

                Error_Debug(buff);
            }
        }
        swp++;
        csize -= 2;
        pc++;
    }
    sizeData->lwPolyCount = pc;
    Mem_Free(sp);

    return TRUE;
}

B32 SRFSprc(lpFile file, LWSIZE *sizeData, LWSURFLIST **srfl, S32 csize, U32 *srflCount, B32 dflag)
{
    char* sp, *cp;
    char buff[512];
    char cbuff[10] = {0};
    U32 len;
    S32 nc = 0;
    LWSURFLIST *srfb;

    sp = Mem_Alloc(csize + 16);
    cp = sp;
    File_Read(sp, csize, 1, file);

    if (srfl)
    {
        srfb = Mem_Alloc(sizeof(LWSURFLIST));
        ZeroMemory(srfb, sizeof(LWSURFLIST));
    }

    while (csize > 0)
    {
        if (dflag)
        {
            sprintf(buff, "Surface %d > %s\n", nc, cp);
            Error_Debug(buff);
        }

        len = strlen(cp) + 1;
        if (len & 1)
            len++;

        if (srfl)
        {
            char *snp, **snpp;

            snp = Mem_Alloc(len);
            snpp = Mem_Alloc((nc + 1) * sizeof(char*));
            memcpy(snp, cp, len);

            if (nc)
            {
                memcpy(snpp, srfb->srflName, nc * sizeof(char*));
                Mem_Free(srfb->srflName);
            }
            *(snpp + nc) = snp;
            srfb->srflName = snpp;
            *srfl = srfb;
            srfb->srflCount = nc + 1;
        }
        nc++;
        *srflCount = nc;
        cp += len;
        csize -= len;
    }
    Mem_Free(sp);
    return TRUE;
}

B32 SURFprc(lpFile file, LWSIZE *sizeData, LWSURFACE **surf, S32 csize, B32 dflag)
{
    char *sp, *cp;
    char buff[512];
    char buff2[512];
    char cbuff[10] = {0};
    U32 len;
    LWSURFACE *srfb;
    S32 sc;
    U32 setVal;

    sp = Mem_Alloc(csize + 16);
    cp = sp;
    File_Read(sp, csize, 1, file);

    sc = sizeData->lwSurfaceCount;

    if (surf)
    {
        if (!sc)
        {
            srfb = Mem_Alloc(sizeof(LWSURFACE));
            *surf = srfb;
            ZeroMemory(srfb, sizeof(LWSURFACE));
            srfb->srfName = stringAlloc(cp);
        } else
        {
            srfb = Mem_Alloc(sizeof(LWSURFACE) * (sc + 1));
            memcpy(srfb, *surf, sc * sizeof(LWSURFACE));
            Mem_Free(*surf);
            *surf = srfb;
            srfb += sc;
            ZeroMemory(srfb, sizeof(LWSURFACE));
            srfb->srfName = stringAlloc(cp);
        }
    }

    if (dflag)
    {
        sprintf(buff, "Surface %d :- %s\n", sc, cp);
        Error_Debug(buff);
    }

    len = strlen(cp) + 1;
    if (len & 1)
        len++;
    cp += len;
    csize -= len;

    while(csize > 0)
    {
        strncpy(cbuff, cp, 4);
        cp += 4;
        len = *(U16*)cp;
        len = LWSVALUE(len);
        sprintf(buff, " %s (%d)", cbuff, len);
        cp += 2;

        if (!strnicmp(cbuff, "TIMG", 4))
        {
            strcat(buff, cp);
            if (surf)
            {
                U32 pos, len;
                char cps[128];

                strcpy(cps, cp);
                if (lwExtractString(cps, "(sequence)", &pos, &len))
                {
                    *(cps + pos - 1) = 0; // Get rid of sequence
                    srfb->srfTexFlags |= TFM_SEQUENCE;
                }
                srfb->srfPath = stringAlloc(cps);
            }
        } else if (!strnicmp(cbuff, "COLR", 4))
        {
            sprintf(buff2, " RGBA %d,%d,%d,%d", cp[0], cp[1], cp[2], cp[3]);
            strcat(buff, buff2);
            if (surf)
            {
                srfb->srfCol.colRed = cp[0];
                srfb->srfCol.colGreen = cp[1];
                srfb->srfCol.colBlue = cp[2];
                srfb->srfCol.colAlpha = cp[3];
            }
        } else if (!strnicmp(cbuff, "CTEX", 4))
        {
            strcat(buff, cp);
            if (surf)
            {
                srfb->srfTexType = texMapType(cp);
            }
        } else if (!strnicmp(cbuff, "TTEX", 4))
        {
            strcat(buff, cp);
            if (surf)
            {
                LWSURFACE *srfbN;

                srfbN = Mem_Alloc(sizeof(LWSURFACE));
                srfb->srfNextSurf = srfbN;
                srfb = srfbN;
                ZeroMemory(srfb, sizeof(LWSURFACE));

                srfb->srfTexType = texMapType(cp);
            }
        } else if (!strnicmp(cbuff, "TFLG", 4))
        {
            U16 sval, i, f = 0;
            if (surf)
            {
                char *ttexName[] = {
                    "Axis_x",
                    "Axis_y",
                    "Axis_z",
                    "World Coord",
                    "Negative Image",
                    "Pixel Blending",
                    "Anti Aliasing",
                };

                sval = LWSPVALUE((U16*)cp);
                srfb->srfTexFlags |= sval;
                sprintf(buff2, "%xH - ", sval);
                for (i = 0; i < TF_MAX; i++)
                {
                    if (sval & (1 << i))
                    {
                        if (f)
                            strcat(buff2, ",");
                        f++;
                        strcat(buff2, ttexName[i]);
                    }
                }

                strcat(buff, buff2);
            }
        } else if (!strnicmp(cbuff, "FLAG", 4))
        {
            U16 sval, i, f = 0;
            if (surf)
            {
                char *tName[] = {
                    "Luminous",
                    "Outline",
                    "Smoothing",
                    "Color Highlights",
                    "Color Filter",
                    "Opaque Edge", //\ these two should not be set together
                    "Transparent Edge", ///
                    "Sharp Terminator",
                    "Double Sided",
                    "Additive"
                };

                sval = LWSPVALUE((U16*) cp);
                srfb->srfFlags = sval;
                sprintf(buff2, "%xH - ", sval);
                for (i = 0; i < SF_MAX; i++)
                {
                    if (sval & (1 << i))
                    {
                        if (f)
                            strcat(buff2, ",");
                        f++;
                        strcat(buff2, tName[i]);
                    }
                }

                strcat(buff, buff2);
            }
        } else if (!strnicmp(buff, "TSIZ", 4))
        {
            TEXDATA td;
            if (surf)
            {
                char *ccp = cp;
                LWD3D(&ccp, &td.tdX);
                LWD3D(&ccp, &td.tdY);
                LWD3D(&ccp, &td.tdZ);
                sprintf(buff2, " (%f,%f,%f)", td.tdX, td.tdY, td.tdZ);
                srfb->srfTexSize.tdX = td.tdX;
                srfb->srfTexSize.tdY = td.tdY;
                srfb->srfTexSize.tdZ = td.tdZ;
                strcat(buff, buff2);
            }
        } else if (!strnicmp(cbuff, "TCTR", 4))
        {
            TEXDATA td;
            if (surf)
            {
                char* ccp = cp;
                LWD3D(&ccp, &td.tdX);
                LWD3D(&ccp, &td.tdY);
                LWD3D(&ccp, &td.tdZ);
                sprintf(buff2, " (%f,%f,%f)", td.tdX, td.tdY, td.tdZ);
                srfb->srfTexCentre.tdX = td.tdX;
                srfb->srfTexCentre.tdY = td.tdY;
                srfb->srfTexCentre.tdZ = td.tdZ;
                strcat(buff, buff2);
            }
        }
        setVal = 0;
        if (!strnicmp(cbuff, "TRAN", 4))
            setVal = 1;
        else if (!strnicmp(cbuff, "DIFF", 4))
            setVal = 2;
        else if (!strnicmp(cbuff, "LUMI", 4))
            setVal = 3;
        else if (!strnicmp(cbuff, "REFL", 4))
            setVal = 4;
        else if (!strnicmp(cbuff, "SPEC", 4))
            setVal = 5;
        else if (!strnicmp(cbuff, "GLOS", 4))
            setVal = 6;

        if (setVal)
        {
            U16 sval;
            F32 fval;
            if (surf)
            {
                sval = LWSPVALUE((U16*)cp);
                fval = (F32) sval / 256;
                switch (setVal)
                {
                    case 1:
                        srfb->srfTransparent = fval;
                        break;
                    case 2:
                        srfb->srfDiffuse = fval;
                        break;
                    case 3:
                        srfb->srfLuminous = fval;
                        break;
                    case 4:
                        srfb->srfReflect = fval;
                        break;
                    case 5:
                        srfb->srfSpecular = fval;
                        break;
                    case 6:
                        srfb->srfSpecPower = fval;
                        break;
                }
                sprintf(buff2, "%f", fval);
                strcat(buff, buff2);
            }
        }

        if (dflag)
        {
            Error_Debug(buff);
        }
        cp += len;
        csize -= (len + 6);
    }
    if (dflag)
    {
        Error_Debug("\n");
    }
    sizeData->lwSurfaceCount++;
    Mem_Free(sp);
    return TRUE;
}

B32 LoadLWOB(char* fn, LWSIZE *sd, F32 **verts, LWPOLY **polys, LWSURFACE **surfs, lpFile *fileUV, B32 dflag)
{
    lpFile file;
    char buff[1024];
    char cbuff[10] = {0};
    char fname[LBUF_LEN];
    U32 size, csize;
    S32 length;
    U32 srflCount = 0;
    LWSURFLIST *srfl = NULL;
    LWSIZE *sizeData;
    U32 slCount = 0;

    sprintf(fname, "%s.lwo", fn);
    file = File_Open(fname, "rb");
    if (!file)
    {
        Error_Warn(TRUE, Error_Format("Cannot load Lightwave object file %s", fname));
        return FALSE;
    } else
    {
        Error_Debug(Error_Format("Loading Lightwave Object file %s\n", fname));
    }

    sizeData = Mem_Alloc(sizeof(LWSIZE));
    sizeData->lwVertCount = sizeData->lwPolyCount = sizeData->lwSurfaceCount = 0;

    if (fileUV)
    {
        sprintf(fname, "%s.uv", fn);

        if (File_Exists(fname))
        {
            *fileUV = File_Open(fname, "rb");
            if (*fileUV)
            {
                // Un-needed debug message
                //Error_Debug(Error_Format("UV file \"%s\" found\n", fname));
            }
        } else
        {
            *fileUV = NULL;
        }
    }

    File_Read(cbuff, 4, 1, file);
    if (stricmp(cbuff, "FORM"))
        goto err;
    File_Read(&size, 4, 1, file);
    size = LWVALUE(size);
    File_Read(cbuff, 4, 1, file);
    if (stricmp(cbuff, "LWOB"))
        goto err;
    length = size - 4;
    while (length > 0)
    {
        File_Read(cbuff, 4, 1, file);
        File_Read(&csize, 4, 1, file);
        csize = LWVALUE(csize);

        if (dflag)
        {
            sprintf(buff, "LW OBJECT %s %d\n", cbuff, csize);
            Error_Debug(buff);
        }

        if (!stricmp(cbuff, "PNTS"))
            PNTSprc(file, sizeData, csize, verts, dflag);
        else if (!stricmp(cbuff, "SRFS"))
            SRFSprc(file, sizeData, &srfl, csize, &srflCount, dflag);
        else if (!stricmp(cbuff, "SURF"))
            SURFprc(file, sizeData, surfs, csize, dflag);
        else if (!stricmp(cbuff, "CRVS"))
            CRVSprc(file, csize, dflag);
        else if (!stricmp(cbuff, "POLS"))
            POLSprc(file, sizeData, polys, csize, dflag);
        else
            File_Seek(file, csize, SEEK_CUR);

        length -= (8 + csize);
    }

    slCount = sizeData->lwSurfaceCount;

    //****************************************
    //**
    //** Place surfaces in correct order for polys using surface list
    //**
    //****************************************

    if (surfs && srfl && sizeData)
    {
        U32 n, n2;
        char* sn;
        LWSURFACE *surfb, *sb;

        surfb = Mem_Alloc(srflCount * sizeof(LWSURFACE));
        sb = surfb;
        ZeroMemory(surfb, sizeof(LWSURFACE) * srflCount);
        for (n = 0; n < srflCount; n++)
        {
            sn = srfl->srflName[n];
            for (n2 = 0; n2 < slCount; n2++)
            {
                if (!stricmp((*surfs + n2)->srfName, sn))
                {
                    *sb = *(*surfs + n2);
                    break;
                }
            }
            if (n2 == slCount)
            {
                sb->srfName = stringAlloc("Default");
                sb->srfCol.colRed = 255;
                sb->srfCol.colGreen = 255;
                sb->srfCol.colBlue = 255;
                slCount = srflCount;
            }
            sb++;
        }
        Mem_Free(*surfs);
        *surfs = surfb;
    }
    sizeData->lwSurfaceCount = slCount;
    if (sd)
        *sd = *sizeData;
    Mem_Free(sizeData);

    //****************************************
    // Free surface list

    if (srflCount)
    {
        U32 i;
        for (i = 0; i < srflCount; i++)
            Mem_Free(srfl->srflName[i]);
        Mem_Free(srfl->srflName);
        Mem_Free(srfl);
    }

    //****************************************

    File_Close(file);

    return TRUE;

err:
    Mem_Free(sizeData);
    File_Close(file);
    sprintf(buff, "Error in Lightwave file %s", fn);
    Error_Warn(TRUE, buff);
    return FALSE;
}

B32 FreeLWOB(APPOBJ *ao)
{
    U32 i, n;
    LWSURFACE *srf;

    if (!ao)
        return FALSE;

    if (ao->aoFileUV)
        File_Close(ao->aoFileUV);

    srf = ao->aoSurface;
    if (srf)
    {
        n = ao->aoSize.lwSurfaceCount;
        for (i = 0; i < n; i++)
        {
            surfFree(srf, FALSE);
            srf++;
        }
        Mem_Free(ao->aoSurface);
    }

    if (ao->aoPoly)
    {
        n = ao->aoSize.lwPolyCount;
        for (i = 0; i < n; i++)
        {
            Mem_Free((ao->aoPoly + i)->plyData);
        }
        Mem_Free(ao->aoPoly);
    }

    Mem_Free(ao->aoVerts);
    Mem_Free(ao->aoPath);
    Mem_Free(ao);

    return TRUE;
}

B32 LoadAppObj(char* fn, APPOBJ **ao, B32 flag)
{
    char path[128];

    (*ao) = Mem_Alloc(sizeof(APPOBJ));
    ZeroMemory((*ao), sizeof(APPOBJ));
    _splitpath(fn, NULL, path, NULL, NULL);
    (*ao)->aoPath = stringAlloc(path);
    if (LoadLWOB(fn, &(*ao)->aoSize, &(*ao)->aoVerts, &(*ao)->aoPoly, &(*ao)->aoSurface, &(*ao)->aoFileUV, flag))
    {
        return TRUE;
    } else {
        Mem_Free((*ao)->aoPath);
        Mem_Free(*ao);

        return FALSE;
    }
}
