/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "base/Log.h"

#include <QObject>

class RightModifierRemapperTests : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void inactiveWithoutOverrides();
  void rightControlFollowsClientMap();
  void rightControlWithoutClientMap();
  void leftControlUnchanged();
  void bothControlsHeld();
  void releaseRestoresMask();
  void noneSwallowsKey();
  void configureClearsHeldKeys();

private:
  Log m_log;
};
