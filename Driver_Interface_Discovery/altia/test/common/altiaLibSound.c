/* $Revision: 1.4 $    $Date: 2009-01-10 00:06:12 $
 * Copyright (c) 2006 Altia Inc.
 *
 * Permission to use, copy, and modify this software for any purpose is 
 * hereby granted, provided that this copyright notice appears in all 
 * copies and that the entity using, copying, or modifying this software 
 * has purchased an Altia Deep Screen license. Permission to sell or 
 * distribute this source code is denied. Altia makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * ALTIA INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL ALTIA INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include "altiaBase.h"

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>

static ALTIA_BOOLEAN sound_playing = false;
#endif

/* **************************************************************
 * 
 * This file contains code for playing sounds on both Windows 
 * and Windows CE.  In this file, PlaySound is used for both
 * targets (which means the DONE event is not supported).
 *
 * This code also has an empty section at the end that can be
 * filled in for targets other than Windows/WinCE.
 *
 */


/* **************************************************************
 * 
 * WINDOWS / WINDOWS CE CODE STARTS HERE 
 * 
 * **************************************************************
 */
#if defined(WIN32)


/*-------------------------------------------------------------
 * This function opens the hardware
 * 
 * It is called ONCE by altiaLibSoundObjDraw when the very first 
 * sound is drawn. 
 * 
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT init_snd()
{
    ALTIA_INT err = 0;
    return err;
}


/*-------------------------------------------------------------
 * This function closes the current sound
 *
 * This function is called when a DONE event is received for
 * the sound.  To support this, the way you play sound has to
 * be able to detect when it is finished playing the sound.
 * Currently this is only done for Windows (via MM_MCINOTIFY
 * in altiaUtils.c). 
 *
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT close_snd()
{
    ALTIA_INT err = 0;
    return err;
}


/*-------------------------------------------------------------
 * This function stops the current sound
 * 
 * This function is called when the STOP event is generated for
 * the sound.
 * 
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT stop_snd()
{
    ALTIA_INT err = PlaySound(NULL, NULL, 0);
    sound_playing = false;
    return err;
}

/*-------------------------------------------------------------
 * This function plays a sound.  
 * 
 * This function is called when the PLAY event is generated for
 * the sound.  If a sound is already playing it stops that sound and 
 * then plays.
 *
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT play_snd(const ALTIA_CHAR *name, void* obj, ALTIA_BOOLEAN wait)
{
    const ALTIA_CHAR *ptrname;
    ALTIA_BOOLEAN nowav = false;
    ALTIA_INT res;
    ALTIA_INT err;
    DWORD flags;
    ALTIA_CHAR* soundenv = NULL;
    ALTIA_INT pad;


    ALTIA_CHAR myname[96 +4+1];
    ALTIA_CHAR cwdpath[96];



    if (name == NULL)
        return -1;
    if (sound_playing)
    { /* We are currently playing a sound */
        stop_snd();
    }

    /* Remove any white space before the name */
    while (ALT_ISSPACE(*name))
    {
        name++;
        if (*name == '\0')
            return -1;
    }


    GetModuleFileName(NULL, cwdpath, 96 -1);
    soundenv = ALT_STRRCHR(cwdpath, '\\');
    if (soundenv != NULL) *soundenv = '\0';


    soundenv = cwdpath;

    if (soundenv == NULL)
        pad = 13+7+4; /* "\My Documents" "\sound\"+ ".wav" */
    else
        /* soundenv + "." + "\sound\" +".wav" */
        /* (worst case soundenv=empty) */
        pad = ALT_STRLEN(soundenv) + 1+7+4;

    


    /* *************************************************************
     *
     * Look in CWD
     */
    
    /* Make sure static string is big enough. If not, do nothing.
     *
     * +4 space for ".wav" already allocated to myname variable.
     * So, user just has to specify enough room for CWD+\+"beep" (not 
     * beep.wav) -- even though we still need enough room for the latter
     */
    if (soundenv == NULL ||
            ((ALT_STRLEN(soundenv)+1+ALT_STRLEN(name)) > 96))
        return -1;
    

    /* CWD + \ + name + .wav */
    if (soundenv != NULL)
        ALT_STRCPY(myname, soundenv);
    else
        ALT_STRCPY(myname, ALT_TEXT("."));

    ALT_STRCAT(myname, ALT_TEXT("\\"));
    ALT_STRCAT(myname, name);
    if (ALT_STRCHR(name,'.') == 0)
    { /* add the .wav extension */
        ALT_STRCAT(myname, ALT_TEXT(".wav"));
        ptrname = myname;
        nowav = true;
    }else
    {
        ptrname = myname;
    }
    res = GetFileAttributes(ptrname);


    /* *************************************************************
     *
     * Look in CWD\sound
     */
    
    /* Make sure static string is big enough. If not, do nothing.
     * +4 space for ".wav" already added to myname variable
     * +7 for "\sound\" is in pad already.
     */
    if ((res == -1) && ((ALT_STRLEN(name)+pad) > 96))
        return -1;
    

    /* soundenv is the CWD */
    if (soundenv != NULL &&
            (res == -1 || (res & FILE_ATTRIBUTE_DIRECTORY)))
    { /* could not find file in current directory
       * try .\sound\ sub-directory
       */
        ALT_STRCPY(myname, soundenv);
        ALT_STRCAT(myname, ALT_TEXT("\\sound\\"));
        ALT_STRCAT(myname, name);
        if (nowav)
            ALT_STRCAT(myname, ALT_TEXT(".wav"));
        ptrname = myname;
    }
    res = GetFileAttributes(ptrname);


    /* *************************************************************
     *
     * Look in \My Documents
     */
    
    /* Make sure static string is big enough. If not, do nothing. */
    if ((res == -1) && ((ALT_STRLEN(name)+pad) > 96))
        return -1;
    

    if (res == -1 || (res & FILE_ATTRIBUTE_DIRECTORY))
    { /* could not find the file in \sound sub-dir, either */
      /* last chance ... */
        ALT_STRCPY(myname, ALT_TEXT("\\My Documents\\"));
        ALT_STRCAT(myname, name);
        if (nowav)
            ALT_STRCAT(myname, ALT_TEXT(".wav"));
        ptrname = myname;
    }

    if (wait)
        flags = SND_NODEFAULT | SND_FILENAME | SND_SYNC;
    else
        flags = SND_NODEFAULT | SND_FILENAME | SND_ASYNC;

    if (err = PlaySound(ptrname, NULL, flags))
    {
        ALTIA_BOOLEAN played = false;
    }


    if (!wait)
    {
        sound_playing = true;
    }

    
    return 0;
}


/* **************************************************************
 *
 * GENERIC CODE STARTS HERE
 *
 * **************************************************************
 */
#else


/*-------------------------------------------------------------
 * This function opens the hardware
 * 
 * It is called ONCE by altiaLibSoundObjDraw when the very first 
 * sound is drawn. 
 * 
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT init_snd()
{
    return 0;
}


/*-------------------------------------------------------------
 * This function closes the current sound
 *
 * This function is called when a DONE event is received for
 * the sound.  To support this, the way you play sound has to
 * be able to detect when it is finished playing the sound.
 * Currently this is only done for Windows (via MM_MCINOTIFY
 * in altiaUtils.c). 
 *
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT close_snd()
{
    return 0;
}


/*-------------------------------------------------------------
 * This function stops the current sound
 * 
 * This function is called when the STOP event is generated for
 * the sound.
 * 
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT stop_snd()
{
    return 0;
}


/*-------------------------------------------------------------
 * This function plays a sound.  
 * 
 * This function is called when the PLAY event is generated for
 * the sound.  If a sound is already playing it stops that sound and 
 * then plays.
 *
 * The return value is currently unused.
 *-------------------------------------------------------------
 */
ALTIA_INT play_snd(const ALTIA_CHAR *name, void* obj, ALTIA_BOOLEAN wait)
{
    return 0;
}

#endif 
