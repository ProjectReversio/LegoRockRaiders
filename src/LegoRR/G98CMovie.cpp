#include "G98CMovie.hpp"
#include "mem.h"

G98CMovie::G98CMovie(const char* fName, LPDIRECTDRAWSURFACE3 bSurf3, LPDIRECTDRAW2 ddraw2)
{
    m_sampleBaseStream = nullptr;
    m_sampleStream = nullptr;
    m_baseSurf = nullptr;
    m_surf = nullptr;
    m_sample = nullptr;
    m_bSurf = bSurf3;
    m_ddraw2 = ddraw2;

    CoInitialize(nullptr);

    if (!fName)
        return;

    if (!strlen(fName))
        return;

    m_filename = (char*)Mem_Alloc(strlen(fName) + 1);
    strcpy(m_filename, fName);

    if (this->OpenAMStream(fName, &m_amStream, ddraw2))
        this->InitSample(m_amStream);
}

G98CMovie::~G98CMovie()
{
    if (m_sampleBaseStream)
        m_sampleBaseStream->Release();

    if (m_sampleStream)
        m_sampleStream->Release();

    if (m_sample)
        m_sample->Release();

    if (m_baseSurf)
        m_baseSurf->Release();

    if (m_bSurf)
        m_bSurf->Release();

    if (m_ddraw2)
        m_ddraw2->Release();

    if (m_amStream)
        m_amStream->Release();

    if (m_filename)
        Mem_Free(m_filename);
}

bool G98CMovie::Update(F32 speed, RECT *destRect)
{
    if (!m_sample)
        return false;

    if (m_sample->Update(0, nullptr, nullptr, 0) == DD_OK)
        return m_bSurf->Blt(destRect, m_surf, nullptr, DDBLT_WAIT, nullptr) == DD_OK;

    m_sampleBaseStream->Release();
    m_sampleBaseStream = nullptr;

    m_sampleStream->Release();
    m_sampleStream = nullptr;

    m_sample->Release();
    m_sample = nullptr;

    m_baseSurf->Release();
    m_baseSurf = nullptr;

    if (m_amStream)
    {
        m_amStream->Release();
        m_amStream = nullptr;
    }

    return false;
}

U64 G98CMovie::GetDuration()
{
    S64 duration;
    m_amStream->GetDuration(&duration);

    // 100-nanosecond units -> milliseconds
    return duration / 10000;
}

bool G98CMovie::OpenAMStream(const char *fName, IAMMultiMediaStream **lplpAMMMStream, LPDIRECTDRAW2 ddraw2)
{
    HRESULT hr;
    IAMMultiMediaStream* amMMStream = nullptr;

    hr = CoCreateInstance(CLSID_AMMultiMediaStream, nullptr, CLSCTX_INPROC_SERVER, IID_IAMMultiMediaStream, (void**)&amMMStream);
    if (SUCCEEDED(hr))
    {
        hr = amMMStream->Initialize(STREAMTYPE_READ, 0, nullptr);
        if (SUCCEEDED(hr))
        {
            hr = amMMStream->AddMediaStream(ddraw2, &MSPID_PrimaryVideo, 0, nullptr);
            if (SUCCEEDED(hr))
            {
                hr = amMMStream->AddMediaStream(nullptr, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, nullptr);
                if (SUCCEEDED(hr))
                {
                    WCHAR wFilename[MAX_PATH];
                    MultiByteToWideChar(CP_ACP, 0, fName, -1, wFilename, MAX_PATH);
                    amMMStream->OpenFile(wFilename, 0);
                    *lplpAMMMStream = amMMStream;
                    return true;
                }
            }
        }
    }

    amMMStream->Release();
    return false;
}

bool G98CMovie::InitSample(IAMMultiMediaStream *lpAMMMStream)
{
    m_err = lpAMMMStream->GetMediaStream(MSPID_PrimaryVideo, &m_sampleBaseStream);

    if (SUCCEEDED(m_err))
    {
        m_err = m_sampleBaseStream->QueryInterface(IID_IDirectDrawMediaStream, (void**)&m_sampleStream);
        if (SUCCEEDED(m_err))
        {
            m_err = m_sampleStream->CreateSample(nullptr, nullptr, 0, &m_sample);
            if (SUCCEEDED(m_err))
            {
                m_err = m_sample->GetSurface(&m_baseSurf, &m_movieRect);
                if (SUCCEEDED(m_err))
                {
                    m_err = m_baseSurf->QueryInterface(IID_IDirectDrawSurface3, (void**)&m_surf);
                    if (SUCCEEDED(m_err))
                    {
                        m_err = lpAMMMStream->SetState(STREAMSTATE_RUN);
                        return true;
                    }
                }
            }
        }
    }

    m_sampleBaseStream->Release();
    m_sampleStream->Release();
    m_sample->Release();
    m_baseSurf->Release();

    return SUCCEEDED(m_err);
}
