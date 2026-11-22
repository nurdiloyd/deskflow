/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "server/RightModifierRemapper.h"

#include "base/Log.h"
#include "deskflow/OptionTypes.h"

#include <algorithm>

namespace {

// indexed by KeyModifierID
const KeyID s_rightKey[kKeyModifierIDLast] = {kKeyNone,  kKeyShift_R, kKeyControl_R, kKeyAlt_R,
                                              kKeyMeta_R, kKeySuper_R, kKeyAltGr};
const KeyModifierMask s_mask[kKeyModifierIDLast] = {0,               KeyModifierShift, KeyModifierControl, KeyModifierAlt,
                                                    KeyModifierMeta, KeyModifierSuper, KeyModifierAltGr};

bool classify(KeyID id, KeyModifierID &type, bool &right)
{
  switch (id) {
  case kKeyShift_L:
  case kKeyShift_R:
    type = kKeyModifierIDShift;
    right = (id == kKeyShift_R);
    return true;
  case kKeyControl_L:
  case kKeyControl_R:
    type = kKeyModifierIDControl;
    right = (id == kKeyControl_R);
    return true;
  case kKeyAlt_L:
  case kKeyAlt_R:
    type = kKeyModifierIDAlt;
    right = (id == kKeyAlt_R);
    return true;
  case kKeyMeta_L:
  case kKeyMeta_R:
    type = kKeyModifierIDMeta;
    right = (id == kKeyMeta_R);
    return true;
  case kKeySuper_L:
  case kKeySuper_R:
    type = kKeyModifierIDSuper;
    right = (id == kKeySuper_R);
    return true;
  case kKeyAltGr:
    type = kKeyModifierIDAltGr;
    right = true;
    return true;
  default:
    return false;
  }
}

KeyModifierID clampModifier(OptionValue value)
{
  return static_cast<KeyModifierID>(std::clamp<OptionValue>(value, 0, kKeyModifierIDLast - 1));
}

} // namespace

RightModifierRemapper::RightModifierRemapper()
{
  configure(nullptr);
}

void RightModifierRemapper::configure(const Options *options)
{
  for (KeyModifierID id = 0; id < kKeyModifierIDLast; ++id) {
    m_clientMap[id] = id;
    m_rightSource[id] = kUnset;
    m_leftHeld[id] = false;
    m_rightHeld[id] = false;
  }

  if (options == nullptr) {
    return;
  }

  // the client's own map first, the overrides depend on it
  for (const auto &[optionId, value] : *options) {
    if (optionId == kOptionModifierMapForShift) {
      m_clientMap[kKeyModifierIDShift] = clampModifier(value);
    } else if (optionId == kOptionModifierMapForControl) {
      m_clientMap[kKeyModifierIDControl] = clampModifier(value);
    } else if (optionId == kOptionModifierMapForAlt) {
      m_clientMap[kKeyModifierIDAlt] = clampModifier(value);
    } else if (optionId == kOptionModifierMapForAltGr) {
      m_clientMap[kKeyModifierIDAltGr] = clampModifier(value);
    } else if (optionId == kOptionModifierMapForMeta) {
      m_clientMap[kKeyModifierIDMeta] = clampModifier(value);
    } else if (optionId == kOptionModifierMapForSuper) {
      m_clientMap[kKeyModifierIDSuper] = clampModifier(value);
    }
  }

  for (const auto &[optionId, value] : *options) {
    KeyModifierID type = kKeyModifierIDNull;
    if (optionId == kOptionModifierMapForRightShift) {
      type = kKeyModifierIDShift;
    } else if (optionId == kOptionModifierMapForRightControl) {
      type = kKeyModifierIDControl;
    } else if (optionId == kOptionModifierMapForRightAlt) {
      type = kKeyModifierIDAlt;
    } else if (optionId == kOptionModifierMapForRightSuper) {
      type = kKeyModifierIDSuper;
    } else {
      continue;
    }

    const auto target = clampModifier(value);
    m_rightSource[type] = (target == kKeyModifierIDNull) ? kKeyModifierIDNull : sourceFor(target);
    LOG_DEBUG("right modifier %d mapped to %d (sent as %d)", type, target, m_rightSource[type]);
  }
}

bool RightModifierRemapper::isActive() const
{
  return std::any_of(std::begin(m_rightSource), std::end(m_rightSource), [](KeyModifierID source) {
    return source != kUnset;
  });
}

KeyModifierID RightModifierRemapper::sourceFor(KeyModifierID target) const
{
  if (m_clientMap[target] == target) {
    return target;
  }
  for (KeyModifierID id = kKeyModifierIDShift; id < kKeyModifierIDLast; ++id) {
    if (m_clientMap[id] == target) {
      return id;
    }
  }
  LOG_WARN("no modifier maps to %d on the client, right modifier sent unchanged", target);
  return target;
}

KeyID RightModifierRemapper::translateKey(KeyID id, bool down)
{
  KeyModifierID type;
  bool right;
  if (!classify(id, type, right)) {
    return id;
  }

  if (right) {
    m_rightHeld[type] = down;
  } else {
    m_leftHeld[type] = down;
  }

  if (!right || m_rightSource[type] == kUnset) {
    return id;
  }
  return s_rightKey[m_rightSource[type]];
}

KeyModifierMask RightModifierRemapper::translateMask(KeyModifierMask mask) const
{
  KeyModifierMask clear = 0;
  KeyModifierMask set = 0;
  for (KeyModifierID type = kKeyModifierIDShift; type < kKeyModifierIDLast; ++type) {
    if (m_rightSource[type] == kUnset || !m_rightHeld[type]) {
      continue;
    }
    // the left key of the same type still produces the original modifier
    if (!m_leftHeld[type]) {
      clear |= s_mask[type];
    }
    set |= s_mask[m_rightSource[type]];
  }
  return (mask & ~clear) | set;
}
