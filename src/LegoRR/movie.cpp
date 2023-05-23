#include <cstdio>
#include "movie.h"
#include "G98CMovie.hpp"
#include "file.h"
#include "directdraw.h"
#include "error.h"

extern "C"
{
    Movie_t *Movie_Load(const char *fName)
    {
        const char* fullName = File_VerifyFilename(fName);

        LPDIRECTDRAW2 ddraw2;
        LPDIRECTDRAWSURFACE3 bSurf3;

        if (DirectDraw()->QueryInterface(IID_IDirectDraw2, (void**)(&ddraw2)) == DD_OK)
        {
            if (DirectDraw_bSurf()->QueryInterface(IID_IDirectDrawSurface3, (void**)&bSurf3) == DD_OK)
            {
                FILE* file = fopen(fullName, "r");
                if (file)
                {
                    fclose(file);
                    return new G98CMovie(fullName, bSurf3, ddraw2);
                }

                char buffer[512];
                if (File_GetCDFilePath(buffer, fName))
                {
                    return new G98CMovie(buffer, bSurf3, ddraw2);
                }
                else
                {
                    Error_Warn(TRUE, Error_Format("Movie_Load: Failed to find movie file '%s'", fName));
                }
            } else {
                Error_Warn(TRUE, "Movie_Load: Failed to get DirectDrawSurface3 interface");
            }
        } else
        {
            Error_Warn(TRUE, "Movie_Load: Failed to get DirectDraw2 interface");
        }

        return nullptr;
    }

    void Movie_Free(Movie_t *mov)
    {
        delete mov;
    }

    B32 Movie_Update(Movie_t *mov, F32 speed, RECT *destRect)
    {
        return mov->Update(speed, destRect);
    }

    U64 Movie_GetDuration(Movie_t *mov)
    {
        return mov->GetDuration();
    }
}
