/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "RightModifierRemapperTests.h"

#include "deskflow/OptionTypes.h"
#include "server/RightModifierRemapper.h"

#include <QTest>

namespace {

// a macOS client with Alt and Super swapped (Alt -> Command, Super -> Option)
// and the right Ctrl set to end up as Option (Alt on the client)
RightModifierRemapper::Options macOptions()
{
  return {
      {kOptionModifierMapForAlt, kKeyModifierIDSuper},
      {kOptionModifierMapForSuper, kKeyModifierIDAlt},
      {kOptionModifierMapForRightControl, kKeyModifierIDAlt},
  };
}

} // namespace

void RightModifierRemapperTests::inactiveWithoutOverrides()
{
  RightModifierRemapper remapper;
  QVERIFY(!remapper.isActive());

  const RightModifierRemapper::Options options = {{kOptionModifierMapForAlt, kKeyModifierIDSuper}};
  remapper.configure(&options);
  QVERIFY(!remapper.isActive());
  QCOMPARE(remapper.translateKey(kKeyControl_R, true), kKeyControl_R);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierControl);
}

void RightModifierRemapperTests::rightControlFollowsClientMap()
{
  RightModifierRemapper remapper;
  const auto options = macOptions();
  remapper.configure(&options);
  QVERIFY(remapper.isActive());

  // the client turns Super into Alt, so Super is sent to end up with Alt
  QCOMPARE(remapper.translateKey(kKeyControl_R, true), kKeySuper_R);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierSuper);
  QCOMPARE(remapper.translateMask(KeyModifierControl | KeyModifierShift), KeyModifierSuper | KeyModifierShift);
}

void RightModifierRemapperTests::rightControlWithoutClientMap()
{
  RightModifierRemapper remapper;
  const RightModifierRemapper::Options options = {{kOptionModifierMapForRightControl, kKeyModifierIDAlt}};
  remapper.configure(&options);

  QCOMPARE(remapper.translateKey(kKeyControl_R, true), kKeyAlt_R);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierAlt);
}

void RightModifierRemapperTests::leftControlUnchanged()
{
  RightModifierRemapper remapper;
  const auto options = macOptions();
  remapper.configure(&options);

  QCOMPARE(remapper.translateKey(kKeyControl_L, true), kKeyControl_L);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierControl);
  QCOMPARE(remapper.translateKey(kKeyAlt_L, true), kKeyAlt_L);
}

void RightModifierRemapperTests::bothControlsHeld()
{
  RightModifierRemapper remapper;
  const auto options = macOptions();
  remapper.configure(&options);

  remapper.translateKey(kKeyControl_L, true);
  remapper.translateKey(kKeyControl_R, true);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierControl | KeyModifierSuper);

  remapper.translateKey(kKeyControl_L, false);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierSuper);
}

void RightModifierRemapperTests::releaseRestoresMask()
{
  RightModifierRemapper remapper;
  const auto options = macOptions();
  remapper.configure(&options);

  remapper.translateKey(kKeyControl_R, true);
  QCOMPARE(remapper.translateKey(kKeyControl_R, false), kKeySuper_R);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierControl);
  QCOMPARE(remapper.translateMask(0), static_cast<KeyModifierMask>(0));
}

void RightModifierRemapperTests::noneSwallowsKey()
{
  RightModifierRemapper remapper;
  const RightModifierRemapper::Options options = {{kOptionModifierMapForRightControl, kKeyModifierIDNull}};
  remapper.configure(&options);

  QCOMPARE(remapper.translateKey(kKeyControl_R, true), kKeyNone);
  QCOMPARE(remapper.translateMask(KeyModifierControl), static_cast<KeyModifierMask>(0));
  QCOMPARE(remapper.translateKey('a', true), static_cast<KeyID>('a'));
}

void RightModifierRemapperTests::configureClearsHeldKeys()
{
  RightModifierRemapper remapper;
  const auto options = macOptions();
  remapper.configure(&options);
  remapper.translateKey(kKeyControl_R, true);

  remapper.configure(&options);
  QCOMPARE(remapper.translateMask(KeyModifierControl), KeyModifierControl);
}

QTEST_MAIN(RightModifierRemapperTests)
