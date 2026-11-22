/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "common/KeyModifierID.h"
#include "deskflow/KeyTypes.h"
#include "deskflow/OptionTypes.h"

#include <map>

/**
 * @brief Remaps only the right-hand modifier keys for the active client.
 *
 * The client's own modifier map (alt = super, ...) works per modifier type, so it cannot make the right Ctrl
 * behave differently from the left one. This runs on the server, before a key is sent: it swaps the right-hand key
 * for a different modifier key and rewrites the modifier mask of every key sent while that key is held.
 *
 * Overrides name the modifier the client should end up with. Because the client still applies its own map to
 * whatever it receives, the remapper sends the modifier that the client's map turns into that target.
 */
class RightModifierRemapper
{
public:
  //! Same as Config::ScreenOptions (Config.h cannot be included here, it includes Server.h)
  using Options = std::map<OptionID, OptionValue>;

  RightModifierRemapper();

  //! Load the client's modifier map and right-hand overrides; clears the held key state
  void configure(const Options *options);

  //! True if any right-hand override is set
  bool isActive() const;

  /**
   * @brief Update the held state for a key event and return the key to send.
   * @return kKeyNone if the key should be swallowed (override is "none"), otherwise the key to send
   */
  KeyID translateKey(KeyID id, bool down);

  //! Rewrite a modifier mask for the right-hand keys currently held
  KeyModifierMask translateMask(KeyModifierMask mask) const;

private:
  static constexpr KeyModifierID kUnset = kKeyModifierIDLast;

  //! The modifier to send so that the client's map produces @p target
  KeyModifierID sourceFor(KeyModifierID target) const;

  // per server modifier type: the modifier type the client turns it into
  KeyModifierID m_clientMap[kKeyModifierIDLast];
  // per server modifier type: modifier to send for its right-hand key (kUnset = no override, Null = swallow)
  KeyModifierID m_rightSource[kKeyModifierIDLast];
  bool m_leftHeld[kKeyModifierIDLast];
  bool m_rightHeld[kKeyModifierIDLast];
};
