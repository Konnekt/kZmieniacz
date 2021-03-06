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

#include "stdafx.h"
#include "main.h"

#include "Helpers.h"
#include "MRU.h"
#include "NetList.h"
#include "Status.h"

namespace kZmieniacz {
  class Control {
    public:
      Control();
      ~Control();

      inline int getPluginsGroup() {
        return(this->pluginsGroup);
      }

      inline int stIcon(int status, int _net = -1) {
        _net = (_net != -1) ? _net : net;
        return(UIIcon(IT_STATUS, _net, status, 0));
      }

      inline std::string getStInfoMruName(int net) {
        std::string name = cfg::mruName;
        return(name + "@" + itos(net));
      }

      void changeStatus(int status, std::string info, int net = 0);
      void changeStatus(int status, int net = 0);

      void refreshCombo(std::string info);

      int width;
      std::map<int, int> stTbNets;
      POINT pos;

    protected:
      int pluginsGroup;
  };

  HWND stInfoTb = NULL;
  WNDPROC mainProc = NULL;
  WNDPROC tbProc = NULL;

  Control *pCtrl = NULL;
  Status *sCtrl = NULL;
  NetList *lCtrl = NULL;
  MRU *MRUlist = NULL;
}