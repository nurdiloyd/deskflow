/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-FileCopyrightText: (C) 2012 - 2016 Synergy App Ltd
 * SPDX-FileCopyrightText: (C) 2002 Chris Schoeneman
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <cstdint>
#include <vector>

//! Option ID
/*!
Type to hold an option identifier.
*/
using OptionID = uint32_t;

//! Option Value
/*!
Type to hold an option value.
*/
using OptionValue = int32_t;

// for now, options are just pairs of integers
using OptionsList = std::vector<uint32_t>;

// macro for packing 4 character strings into 4 byte integers
#define OPTION_CODE(_s)                                                                                                \
  (static_cast<uint32_t>(static_cast<unsigned char>(_s[0]) << 24) |                                                    \
   static_cast<uint32_t>(static_cast<unsigned char>(_s[1]) << 16) |                                                    \
   static_cast<uint32_t>(static_cast<unsigned char>(_s[2]) << 8) |                                                     \
   static_cast<uint32_t>(static_cast<unsigned char>(_s[3])))

//! @name Option identifiers
//@{
static const OptionID kOptionHalfDuplexCapsLock = OPTION_CODE("HDCL");
static const OptionID kOptionHalfDuplexNumLock = OPTION_CODE("HDNL");
static const OptionID kOptionHalfDuplexScrollLock = OPTION_CODE("HDSL");
static const OptionID kOptionModifierMapForShift = OPTION_CODE("MMFS");
static const OptionID kOptionModifierMapForControl = OPTION_CODE("MMFC");
static const OptionID kOptionModifierMapForAlt = OPTION_CODE("MMFA");
static const OptionID kOptionModifierMapForAltGr = OPTION_CODE("MMFG");
static const OptionID kOptionModifierMapForMeta = OPTION_CODE("MMFM");
static const OptionID kOptionModifierMapForSuper = OPTION_CODE("MMFR");
// Right-hand only modifier overrides. Applied by the server before sending, so the value is the modifier the
// client should end up with (after its own modifier map). Unset means the right key follows the normal map.
static const OptionID kOptionModifierMapForRightShift = OPTION_CODE("MRFS");
static const OptionID kOptionModifierMapForRightControl = OPTION_CODE("MRFC");
static const OptionID kOptionModifierMapForRightAlt = OPTION_CODE("MRFA");
static const OptionID kOptionModifierMapForRightSuper = OPTION_CODE("MRFR");
// While a client with this option is active, a Windows server sends AltGr as a plain right Alt: the fake left
// Ctrl that Windows adds to AltGr is dropped and characters are not resolved through AltGr.
static const OptionID kOptionAltGrAsAlt = OPTION_CODE("AGAA");
static const OptionID kOptionHeartbeat = OPTION_CODE("HART");
static const OptionID kOptionComputerSwitchCorners = OPTION_CODE("SSCM");
static const OptionID kOptionComputerSwitchCornerSize = OPTION_CODE("SSCS");
static const OptionID kOptionComputerSwitchDelay = OPTION_CODE("SSWT");
static const OptionID kOptionComputerSwitchTwoTap = OPTION_CODE("SSTT");
static const OptionID kOptionXTestXineramaUnaware = OPTION_CODE("XTXU");
static const OptionID kOptionComputerX11WeakFocus = OPTION_CODE("SFOC");
static const OptionID kOptionRelativeMouseMoves = OPTION_CODE("MDLT");
static const OptionID kOptionWin32KeepForeground = OPTION_CODE("_KFW");
static const OptionID kOptionDefaultLockToComputerState = OPTION_CODE("LTSS");
static const OptionID kOptionDisableLockToComputer = OPTION_CODE("DLTS");
static const OptionID kOptionClipboardSharing = OPTION_CODE("CLPS");
static const OptionID kOptionClipboardSharingSize = OPTION_CODE("CLSZ");
//@}

//! @name Computer switch corner masks
//@{
inline static const auto s_noCornerMask = 0;
inline static const auto s_topLeftCornerMask = 1 << 0;
inline static const auto s_topRightCornerMask = 1 << 1;
inline static const auto s_bottomLeftCornerMask = 1 << 2;
inline static const auto s_bottomRightCornerMask = 1 << 3;
//@}

#undef OPTION_CODE
