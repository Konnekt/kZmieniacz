/**
 *  kZmieniacz Header
 *
 *  Licensed under The GNU Lesser General Public License
 *  Redistributions of files must retain the above copyright notice.
 *
 *  @filesource
 *  @copyright    Copyright (c) 2006 Sijawusz Pur Rahnama
 *  @link         svn://kplugins.net/kzmieniacz/ kZmieniacz plugin SVN Repo
 *  @version      $Revision$
 *  @modifiedby   $LastChangedBy$
 *  @lastmodified $Date$
 *  @license      http://creativecommons.org/licenses/LGPL/2.1/
 */

#pragma once

namespace kZmieniacz {
  const unsigned int net = 30;

  namespace api {
    const unsigned int api = net * 1000 + IM_USER;
    const unsigned int showStatusWnd = api + 1;
  }

  namespace ui {
    const unsigned int ui = net * 1000;

    const unsigned int cfgGroup = ui + 1;
    const unsigned int msgTbGrp = ui + 2;

    namespace tb {
      const unsigned int tb = ui + 10;
      const unsigned int stTb = tb + 1;

      const unsigned int btnOn = tb + 2;
      const unsigned int btnFfc = tb + 3;
      const unsigned int btnAway = tb + 4;
      const unsigned int btnNa = tb + 5;
      const unsigned int btnDnd = tb + 6;
      const unsigned int btnInv = tb + 7;
      const unsigned int btnOff = tb + 8;

      const unsigned int btnOk = tb + 9;
      const unsigned int width = tb + 10;
      const unsigned int stInfo = tb + 11;
      const unsigned int stInfoThis = tb + 12;
    }
  }

  namespace ico {
    const unsigned int ico = net * 1000 + 100;

    const unsigned int logoSmall = ico + 1;
    const unsigned int stInfo = 503;
  }

  namespace act {
    const unsigned int act = net * 1000 + 200;

    const unsigned int cfgGroupCheckCreate = act + 1;
    const unsigned int cfgGroupCheckDestroy = act + 2;

    const unsigned int clearMru = act + 3;
  }

  // dynamiczne akcje
  namespace dynAct {
    const unsigned int dynAct = net * 1000 + 300;
    const unsigned int stInfoBtn = dynAct + 50;
  }

  namespace cfg {
    const unsigned int cfg = net * 1000 + 400;

    const unsigned int showInMainWindow = cfg + 1;
    const unsigned int showInCntWindow = cfg + 2;
    const unsigned int thisNetStChange = cfg + 3;
    const unsigned int showInTrayMenu = cfg + 4;
    const unsigned int stInfoInTrayMenu = cfg + 5;
    const unsigned int showInNetsTb = cfg + 6;

    const unsigned int showToolbar = cfg + 7;
    const unsigned int showToolbarOk = cfg + 8;

    const unsigned int netChange = cfg + 9;
    const unsigned int mruSize = cfg + 10;
    const unsigned int dotsAppend = cfg + 11;

    const unsigned int lastStInfo = cfg + 12;
    const unsigned int lastSt = cfg + 13;

    const char mruName[] = "kZStatusMsgs";

    namespace wnd {
      const unsigned int wnd = cfg + 20;

      const unsigned int changeOnEnable = wnd + 1;
      const unsigned int changeInfoOnEnable = wnd + 2;
    }
  }
}