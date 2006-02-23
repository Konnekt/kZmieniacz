/**
 *  kZmieniacz Control class
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
#include "Control.h"

namespace kZmieniacz {
  Control::Control() {
    this->pluginsGroup = Helpers::getPluginsGroup();
    this->width = 0;
  }

  Control::~Control() {
    // c u ;>
  }

  void Control::changeStatus(int status, int net) {
    if (status == -1) return;
    if (net && (net != kZmieniacz::net)) {
      sCtrl->changeStatus(net, status);
      return;
    }

    sCtrl->changeStatus(status);
    if (status != GETINT(cfg::lastSt))
      PlugStatusChange(status, 0);

    SETINT(cfg::lastSt, status);
    Helpers::chgBtn(ui::tb::tb, ui::tb::btnOk, 0, 0, this->stIcon(status), -1);
  }

  void Control::changeStatus(int status, std::string info, int net) {
    if (net && (net != kZmieniacz::net)) {
      sCtrl->changeStatus(net, info, status);
      return;
    }

    sCtrl->changeStatus(info, status);
    int lastSt = (status != -1) ? status : GETINT(cfg::lastSt);

    if (lastSt != GETINT(cfg::lastSt) || info != GETSTRA(cfg::lastStInfo))
      PlugStatusChange(status, info.c_str());

    if (status != -1) {
      SETINT(cfg::lastSt, status);
      Helpers::chgBtn(ui::tb::tb, ui::tb::btnOk, 0, 0, this->stIcon(status), -1);
    }

    SETSTR(cfg::lastStInfo, info.c_str());
    this->refreshCombo(info);
  }

  void Control::refreshCombo(std::string info) {
    SendMessage(this->stInfoTb, CB_RESETCONTENT, (WPARAM)0, 0);

    sMRU list;
    list.name = cfg::mruName;
    list.buffSize = 1024;
    list.count = GETINT(cfg::mruSize);
    list.flags = MRU_GET_USETEMP | MRU_SET_LOADFIRST;

    sIMessage_MRU mru(IMC_MRU_GET, &list);
    Ctrl->IMessage(&mru);

    // wype³niamy combobox
    for (int i = 0; i < mru.MRU->count; i++) {
      SendMessage(this->stInfoTb, CB_ADDSTRING, 0, (LPARAM) (char*) mru.MRU->values[i]);
    }
    SetWindowText(this->stInfoTb, info.c_str());
  }
}