#include "sound.h"
#include "main.h"
#include "3DSound.h"

Sound_Globs soundGlobs = { 0 };

B32 Sound_Initialize(B32 nosound)
{
    soundGlobs.initialized = (nosound == FALSE);
    soundGlobs.loopCDTrack = -1;

    if (nosound)
        return TRUE;

    if (Sound3D_Initialize(Main_hWnd()))
        return TRUE;

    return FALSE;
}

B32 Sound_IsInitialized()
{
    return (soundGlobs.initialized && Sound3D_Initialized());
}

S32 WaveOpenFile(void* fileData, U32 fileSize, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF)
{
    HMMIO hmmioIn;
    MMCKINFO ckIn; // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat; // Temporary PCM structure to load in.
    U16 cbExtraAlloc; // Extra bytes for waveformatex
    S32 nError; // Return value.
    MMIOINFO mmioInfo; // For loading from memory...

    // Initialization...
    *ppwfxInfo = NULL;
    nError = 0;
    hmmioIn = NULL;

    memset(&mmioInfo, 0, sizeof(MMIOINFO));
    mmioInfo.fccIOProc = FOURCC_MEM;
    mmioInfo.pchBuffer = fileData;
    mmioInfo.cchBuffer = fileSize;

    if ((hmmioIn = mmioOpen(NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
    {
        nError = ER_CANNOTOPEN;
        goto ERROR_READING_WAVE;
    }

    if ((nError = (S32) mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
        goto ERROR_READING_WAVE;

    if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Search the input file for the 'fmt' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (S32) mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
        goto ERROR_READING_WAVE;

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them.

    if (ckIn.cksize < sizeof(PCMWAVEFORMAT))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
    {
        nError = ER_CANNOTREAD;
        goto ERROR_READING_WAVE;
    }

    // Ok, allocate the waveformatex, but if its not pcm format, read the next
    // word, and that's how many extra bytes to allocate.
    if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
        cbExtraAlloc = 0;
    else
    {
        // Read in length of extra bytes.
        if (mmioRead(hmmioIn, (LPSTR) &cbExtraAlloc, sizeof(cbExtraAlloc)) != sizeof(cbExtraAlloc))
        {
            nError = ER_CANNOTREAD;
            goto ERROR_READING_WAVE;
        }
    }

    // Ok, now allocate that waveformatex structure.
    if ((*ppwfxInfo = GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX) + cbExtraAlloc)) == NULL)
    {
        nError = ER_MEM;
        goto ERROR_READING_WAVE;
    }

    // Copy the bytes from the pcm structure to the waveformatex structure
    memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
    (*ppwfxInfo)->cbSize = cbExtraAlloc;

    // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
    if (cbExtraAlloc != 0)
    {
        if (mmioRead(hmmioIn, (LPTSTR) (((U8*)&((*ppwfxInfo)->cbSize)) + sizeof(cbExtraAlloc)), cbExtraAlloc) != cbExtraAlloc)
        {
            nError = ER_NOTWAVEFILE;
            goto ERROR_READING_WAVE;
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
        goto ERROR_READING_WAVE;

    goto TEMPCLEANUP;

ERROR_READING_WAVE:
    if (*ppwfxInfo != NULL)
    {
        GlobalFree(*ppwfxInfo);
        *ppwfxInfo = NULL;
    }

    if (hmmioIn != NULL)
    {
        mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
    }

TEMPCLEANUP:
    *phmmioIn = hmmioIn;

    return nError;
}

S32 WaveOpenFile2(const char* pszFileName, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF)
{
    HMMIO hmmioIn;
    MMCKINFO ckIn; // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat; // Temporary PCM structure to load in.
    U16 cbExtraAlloc; // Extra bytes for waveformatex
    S32 nError; // Return value.

    // Initialization...
    *ppwfxInfo = NULL;
    nError = 0;
    hmmioIn = NULL;

    if ((hmmioIn = mmioOpen((LPSTR) pszFileName, NULL, MMIO_ALLOCBUF | MMIO_READ)) == NULL)
    {
        nError = ER_CANNOTOPEN;
        goto ERROR_READING_WAVE;
    }

    if ((nError = (S32) mmioDescend(hmmioIn, pckInRIFF, NULL, 0)) != 0)
        goto ERROR_READING_WAVE;

    if ((pckInRIFF->ckid != FOURCC_RIFF) || (pckInRIFF->fccType != mmioFOURCC('W', 'A', 'V', 'E')))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Search the input file for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (S32) mmioDescend(hmmioIn, &ckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
        goto ERROR_READING_WAVE;

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them.

    if (ckIn.cksize < sizeof(PCMWAVEFORMAT))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
    {
        nError = ER_CANNOTREAD;
        goto ERROR_READING_WAVE;
    }

    // Ok, allocate the waveformatex, but if its not pcm format, read the next
    // word, and that's how many extra bytes to allocate.
    if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM)
        cbExtraAlloc = 0;
    else
    {
        // Read in length of extra bytes.
        if (mmioRead(hmmioIn, (LPSTR) &cbExtraAlloc, sizeof(cbExtraAlloc)) != sizeof(cbExtraAlloc))
        {
            nError = ER_CANNOTREAD;
            goto ERROR_READING_WAVE;
        }
    }

    // Ok, now allocate that waveformatex structure.
    if ((*ppwfxInfo = GlobalAlloc(GMEM_FIXED, sizeof(WAVEFORMATEX) + cbExtraAlloc)) == NULL)
    {
        nError = ER_MEM;
        goto ERROR_READING_WAVE;
    }

    // Copy the bytes from the pcm structure to the waveformatex structure
    memcpy(*ppwfxInfo, &pcmWaveFormat, sizeof(pcmWaveFormat));
    (*ppwfxInfo)->cbSize = cbExtraAlloc;

    // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
    if (cbExtraAlloc != 0)
    {
        if (mmioRead(hmmioIn, (LPSTR) (((U8*)&((*ppwfxInfo)->cbSize)) + sizeof(cbExtraAlloc)), cbExtraAlloc) != cbExtraAlloc)
        {
            nError = ER_NOTWAVEFILE;
            goto ERROR_READING_WAVE;
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if ((nError = mmioAscend(hmmioIn, &ckIn, 0)) != 0)
        goto ERROR_READING_WAVE;


    goto TEMPCLEANUP;

ERROR_READING_WAVE:
    if (*ppwfxInfo != NULL)
    {
        GlobalFree(*ppwfxInfo);
        *ppwfxInfo = NULL;
    }

    if (hmmioIn != NULL)
    {
        mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
    }

TEMPCLEANUP:
    *phmmioIn = hmmioIn;

    return nError;
}

S32 WaveStartDataRead(HMMIO* phmmioIn, MMCKINFO* pckIn, MMCKINFO* pckInRIFF)
{
    S32 nError = 0;

    // Do a nice little seek...
    if ((nError = mmioSeek(*phmmioIn, pckInRIFF->dwDataOffset + sizeof(FOURCC), SEEK_SET)) == -1)
        nError = 1;

    nError = 0;

    // Search the input file for the 'data' chunk.
    pckIn->ckid = mmioFOURCC('d', 'a', 't', 'a');
    if ((nError = mmioDescend(*phmmioIn, pckIn, pckInRIFF, MMIO_FINDCHUNK)) != 0)
        goto ERROR_READING_WAVE;

    goto CLEANUP;

ERROR_READING_WAVE:

CLEANUP:

    return nError;
}

S32 WaveReadFile(HMMIO hmmioIn, U32 cbRead, U8* pbDest, MMCKINFO* pckIn, U32* cbActualRead)
{
    MMIOINFO mmioinfoIn; // current status of <hmmioIn>
    S32 nError;
    U32 cT, cbDataIn;

    nError = 0;

    if ((nError = mmioGetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
        goto ERROR_CANNOT_READ;

    cbDataIn = cbRead;
    if (cbDataIn > pckIn->cksize)
        cbDataIn = pckIn->cksize;

    pckIn->cksize -= cbDataIn;

    for (cT = 0; cT < cbDataIn; cT++)
    {
        // Copy the bytes from the io to the buffer.
        if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
        {
            if ((nError = mmioAdvance(hmmioIn, &mmioinfoIn, MMIO_READ)) != 0)
                goto ERROR_CANNOT_READ;

            if (mmioinfoIn.pchNext == mmioinfoIn.pchEndRead)
            {
                nError = ER_CORRUPTWAVEFILE;
                goto ERROR_CANNOT_READ;
            }
        }

        // Actual copy.
        *((U8*)pbDest + cT) = *((U8*) mmioinfoIn.pchNext);
        mmioinfoIn.pchNext++;
    }

    if ((nError = mmioSetInfo(hmmioIn, &mmioinfoIn, 0)) != 0)
        goto ERROR_CANNOT_READ;

    *cbActualRead = cbDataIn;
    goto FINISHED_READING;

ERROR_CANNOT_READ:
    *cbActualRead = 0;

FINISHED_READING:
    return nError;
}

U32 GetWaveAvgBytesPerSec(const char* pszFileName)
{
    HMMIO hmmioIn;
    MMCKINFO ckIn; // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat; // Temporary PCM structure to load in.
    S32 nError;
    U32 avgBytesPerSec = 0;
    MMCKINFO pckInRIFF;

    // Initialization...
    nError = 0;
    hmmioIn = NULL;

    if ((hmmioIn = mmioOpen(pszFileName, NULL, MMIO_READ)) == NULL)
    {
        nError = ER_CANNOTOPEN;
        goto ERROR_READING_WAVE;
    }

    if ((nError = (S32)mmioDescend(hmmioIn, &pckInRIFF, NULL, 0)) != 0)
        goto ERROR_READING_WAVE;

    if ((pckInRIFF.ckid != FOURCC_RIFF) || (pckInRIFF.fccType != mmioFOURCC('W', 'A', 'V', 'E')))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Search the input file for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if ((nError = (S32)mmioDescend(hmmioIn, &ckIn, &pckInRIFF, MMIO_FINDCHUNK)) != 0)
        goto ERROR_READING_WAVE;

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them.

    if (ckIn.cksize < (S32) sizeof(PCMWAVEFORMAT))
    {
        nError = ER_NOTWAVEFILE;
        goto ERROR_READING_WAVE;
    }

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if (mmioRead(hmmioIn, (HPSTR) &pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat))
    {
        nError = ER_CANNOTREAD;
        goto ERROR_READING_WAVE;
    }

    avgBytesPerSec = pcmWaveFormat.wf.nAvgBytesPerSec;

ERROR_READING_WAVE:
    if (hmmioIn != NULL)
    {
        mmioClose(hmmioIn, 0);
        hmmioIn = NULL;
    }

    return avgBytesPerSec;
}

S32 WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX **ppwfxSrc)
{
    if (*ppwfxSrc != NULL)
    {
        GlobalFree(*ppwfxSrc);
        *ppwfxSrc = NULL;
    }

    if (*phmmio != NULL)
    {
        mmioClose(*phmmio, 0);
        *phmmio = NULL;
    }

    return 0;
}
