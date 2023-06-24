#pragma once

#include "platform.h"
#include <amstream.h>

class G98CMovie
{
public:
    G98CMovie(const char* fName, LPDIRECTDRAWSURFACE3 bSurf3, LPDIRECTDRAW2 ddraw2);
    ~G98CMovie();

    bool Update(F32 speed, RECT* destRect);
    U64 GetDuration();

private:
    bool OpenAMStream(const char* fName, IAMMultiMediaStream** lplpAMMMStream, LPDIRECTDRAW2 ddraw2);
    bool InitSample(IAMMultiMediaStream* lpAMMMStream);

protected:
    IAMMultiMediaStream* m_amStream;
    HRESULT m_err;
    IMediaStream* m_sampleBaseStream;
    IDirectDrawMediaStream *m_sampleStream;
    LPDIRECTDRAWSURFACE m_baseSurf;
    LPDIRECTDRAWSURFACE3 m_surf;
    IDirectDrawStreamSample *m_sample;
    LPDIRECTDRAWSURFACE3 m_bSurf;
    RECT m_movieRect;
    char* m_filename;
    LPDIRECTDRAW2 m_ddraw2;
};
