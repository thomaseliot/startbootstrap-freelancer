/* Copyright (c) 2010 Altia Inc.
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

#ifndef ALTIA_IMAGE_DATA_H
#define ALTIA_IMAGE_DATA_H

#define IMAGE_USE_DYNAMIC_MEMORY      0
#define IMAGE_USE_EXTERNAL_FILES      0
#define IMAGE_USE_FILE_SYSTEM         0
#define IMAGE_USE_COMPRESSION         0
#define IMAGE_USE_ALPHA_FONTS         1
#define IMAGE_USE_RUNTIME_FONTS       0
#define IMAGE_USE_CUSTOM_LOAD         0
#define IMAGE_USE_DDB                 0

#define ALTIA_IMAGE_INDEX_MASK        0x07ffffff
#define ALTIA_IMAGE_BANK_MASK         0x0000000f
#define ALTIA_IMAGE_BANK_SHIFT        27

#define ALTIA_IMAGE_BANK_COUNT        1

extern ALTIA_CONST ALTIA_UBYTE * ALTIA_CONST pAltiaImageBanks[ALTIA_IMAGE_BANK_COUNT];

#endif /* ALTIA_IMAGE_DATA_H */

