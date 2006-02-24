/**
 *  kZmieniacz Status Window class
 *
 *  Licensed under The GNU Lesser General Public License
 *  Redistributions of files must retain the above copyright notice.
 *
 *  @filesource
 *  @copyright    Copyright (c) 2005-2006 Sijawusz Pur Rahnama
 *  @copyright    Copyright (c) 2004 Kuba 'nix' Niegowski
 *  @copyright    Copyright (c) 2003-2006 Kamil 'Olórin' Figiela
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
#include "Control.h"
#include "editFix.h"

namespace kZmieniacz {
  struct sWndData {
    unsigned int net;
    HIMAGELIST hImlOnline;
    HIMAGELIST hImlChat;
    HIMAGELIST hImlAway;
    HIMAGELIST hImlNa;
    HIMAGELIST hImlDnd;
    HIMAGELIST hImlInv;
    HIMAGELIST hImlOffline;

    sWndData(int _net = 0): net(_net), hImlOnline(0), hImlChat(0), hImlAway(0), hImlNa(0), hImlDnd(0), hImlInv(0), hImlOffline(0) { }
    ~sWndData() {
      if (hImlOnline)  ImageList_Destroy(hImlOnline);
      if (hImlChat)    ImageList_Destroy(hImlChat);
      if (hImlAway)    ImageList_Destroy(hImlAway);
      if (hImlNa)      ImageList_Destroy(hImlNa);
      if (hImlDnd)     ImageList_Destroy(hImlDnd);
      if (hImlInv)     ImageList_Destroy(hImlInv);
      if (hImlOffline) ImageList_Destroy(hImlOffline);
    }
  };

  struct sStatus {
    unsigned int id;
    const char * name;
    HIMAGELIST img;

    sStatus(int _id, const char * _name, HIMAGELIST _img) : id(_id), name(_name), img(_img) { }
  };

  typedef std::list<sStatus> tStats;

  class StatusWnd {
    public:
      StatusWnd(std::string className);
      ~StatusWnd();

      void prepareButtonImage(HIMAGELIST &hIml, HWND hWnd, int net, int status);

      inline bool haveInstance(int key) {
        return(this->instances.find(key) != this->instances.end());
      }

      inline HWND getInstance(int key) {
        return(this->instances[key]);
      }

      inline void addInstance(int key, HWND hWnd) {
        this->instances[key] = hWnd;
      }

      inline void removeInstance(int key) {
        this->instances.erase(key);
      }

      void classRegister();
      void classUnRegister();
      void show(int net = 0);

    protected:
      std::string className;
      std::map<int, HWND> instances;
  };
  StatusWnd *wCtrl = NULL;
}