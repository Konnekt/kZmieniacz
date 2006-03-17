/**
 *  kZmieniacz Core
 *
 *  Licensed under The GNU Lesser General Public License
 *  Redistributions of files must retain the above copyright notice.
 *
 *  @filesource
 *  @copyright    Copyright (c) 2006 Sijawusz Pur Rahnama
 *  @copyright    Copyright (c) 2003-2006 Kamil 'Olórin' Figiela
 *  @link         svn://kplugins.net/kzmieniacz/ kZmieniacz plugin SVN Repo
 *  @version      $Revision$
 *  @modifiedby   $LastChangedBy$
 *  @lastmodified $Date$
 *  @license      http://creativecommons.org/licenses/LGPL/2.1/
 */

#include "stdafx.h"
#include "main.h"

#include "Helpers.h"
#include "MRU.h"
#include "NetList.h"
#include "Status.h"
#include "StatusWnd.h"
#include "Control.h"

int __stdcall DllMain(void * hinstDLL, unsigned long fdwReason, void * lpvReserved) {
  return(true);
}

using namespace kZmieniacz;

namespace kZmieniacz {
  LRESULT CALLBACK tbProcNew(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch (iMsg) {
      case WM_GETDLGCODE: {
        return(DLGC_DEFPUSHBUTTON | DLGC_BUTTON | DLGC_WANTALLKEYS | DLGC_WANTMESSAGE);
      }

      case WM_CTLCOLOREDIT: {
        int chars = GetWindowTextLength((HWND)lParam);
        int bottomLimit = sCtrl->getStInfoBottomLimit().length;

        SetTextColor((HDC)wParam, (bottomLimit && (chars > bottomLimit)) ? RGB(255,0,0) : RGB(0,0,0));
        SetBkColor((HDC)wParam, RGB(255,255,255));

        return((LRESULT)GetSysColorBrush(COLOR_3DHILIGHT));
      }

      /*
      case WM_COMMAND: {
        if (HIWORD(wParam) == EN_UPDATE) {
          int chars = GetWindowTextLength((HWND)lParam);
          int bottomLimit = sCtrl->getStInfoBottomLimit().length;

          if (bottomLimit && (chars == bottomLimit || chars == (bottomLimit + 1)))
            InvalidateRect((HWND)lParam, NULL, true);
        }
        break;
      }
      */

      case WM_SIZE: {
        // SendMessage(stInfoTb, WM_SIZE, SIZE_RESTORED, || HIWORD(lParam));
        WORD size = (WORD)SendMessage((HWND) hWnd, (UINT) TB_GETBUTTONSIZE, 0, 0);
        sUIActionInfo ai;

        ai.act = sUIAction(ui::tb::tb, ui::tb::width);
        ai.mask = UIAIM_SIZE;

        ai.w = LOWORD(lParam) - LOWORD(size);
        ai.h = HIWORD(lParam);

        // ai.x = LOWORD(lParam) - LOWORD(size);
        // ai.y = HIWORD(lParam);

        UIActionSet(ai);
        SetWindowPos(stInfoTb, HWND_TOP, 0, 0, LOWORD(lParam) - LOWORD(size) + pCtrl->width, 20, 
          SWP_ASYNCWINDOWPOS | SWP_NOMOVE);
        break;
      }
    }
    return(CallWindowProc(tbProc, hWnd, iMsg, wParam, lParam));
  }

  LRESULT CALLBACK mainProcNew(HWND hWnd, UINT iMsg, WPARAM  wParam, LPARAM lParam) {
    switch(iMsg) {
      case WM_COMMAND: {
        if ((LOWORD(wParam) == 1) && (GetFocus() == GetDlgItem(stInfoTb, IMIA_GGSTATUS_OFFLINE))) 
          Helpers::UIActionCall(ui::tb::tb, ui::tb::btnOk);
        break;
      }
    }
    return(CallWindowProc(mainProc, hWnd, iMsg, wParam, lParam));
  }

  int IStart() {
    HWND handle = (HWND) Ctrl->ICMessage(IMI_GROUP_GETHANDLE, (int) &sUIAction(0, IMIG_MAINWND));
    mainProc = (WNDPROC) SetWindowLongPtr(handle, GWL_WNDPROC, (LONG_PTR) mainProcNew);

    // toolbar z przyciskami sieci
    int n = 0, netsCount = Ctrl->IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_BAR, IMIG_STATUS));
    for (int idx = (netsCount - 1); idx > 0; idx--) {
      int acnID = Ctrl->IMessage(IMI_ACTION_GETID, 0, 0, IMIG_STATUS, idx);
      if (!acnID || !Ctrl->IMessage(IMI_ACTION_ISGROUP, 0, 0, acnID)) continue;

      int net = IMessageDirect(IM_PLUG_NET, ICMessage(IMI_ACTION_GETOWNER, (int)&sUIAction(IMIG_STATUS, acnID)));
      if (lCtrl->isIgnored(net)) continue;

      // sprawdzamy czy jest separator w menu
      int count = IMessage(IMI_GROUP_ACTIONSCOUNT, 0, 0, (int)&sUIAction(IMIG_STATUS, acnID), 0);
      int id = dynAct::stInfoBtn + (n++);
      bool isSep = false;

      for (int i = (count - 1); i > 0; i--) {
        // separator ma najczêœciej identyfikator 0
        if (!Ctrl->IMessage(IMI_ACTION_GETID, 0, 0, acnID, i)) {
          isSep = true; break;
        }
      }

      if (!isSep) UIActionInsert(acnID, 0, 0, ACTT_SEP);

      // dodajemy pozycjê w menu sieci
      UIActionInsert(acnID, id, 0, ACTR_INIT, "Opis: ", ico::stInfo);
      pCtrl->stTbNets[id] = net;
    }

    // ukrywamy standardowe okienko zmiany opisu w gg
    UIActionSetStatus(IMIG_GGSTATUS, IMIA_GGSTATUS_DESC, ACTS_HIDDEN, ACTS_HIDDEN);

    PlugStatusChange(GETINT(cfg::lastSt), GETSTRA(cfg::lastStInfo));
    Helpers::chgBtn(ui::tb::tb, ui::tb::btnOk, 0, 0, pCtrl->stIcon(GETINT(cfg::lastSt)), -1);

    return(1);
  }

  int IEnd() {
    delete pCtrl;
    pCtrl = NULL;

    delete sCtrl;
    sCtrl = NULL;

    delete wCtrl;
    wCtrl = NULL;

    delete lCtrl;
    lCtrl = NULL;

    delete MRUlist;
    MRUlist = NULL;

    return(1);
  }

  int ISetCols() {
    Ctrl->SetColumn(DTCFG, cfg::mruSize, DT_CT_INT, 20, "kZmieniacz/mruSize");
    Ctrl->SetColumn(DTCFG, cfg::netChange, DT_CT_STR, "", "kZmieniacz/netChange");
    Ctrl->SetColumn(DTCFG, cfg::dotsAppend, DT_CT_INT, 1, "kZmieniacz/dotsAppend");
    Ctrl->SetColumn(DTCFG, cfg::sepHistory, DT_CT_INT, 0, "kZmieniacz/sepHistory");

    Ctrl->SetColumn(DTCFG, cfg::showInMainWindow, DT_CT_INT, 0, "kZmieniacz/showInMainWindow");
    Ctrl->SetColumn(DTCFG, cfg::showInCntWindow, DT_CT_INT, 1, "kZmieniacz/showInCntWindow");
    Ctrl->SetColumn(DTCFG, cfg::thisNetStChange, DT_CT_INT, 1, "kZmieniacz/thisNetStChange");
    Ctrl->SetColumn(DTCFG, cfg::showInTrayMenu, DT_CT_INT, 1, "kZmieniacz/showInTrayMenu");
    Ctrl->SetColumn(DTCFG, cfg::stInfoInTrayMenu, DT_CT_INT, 1, "kZmieniacz/stInfoInTrayMenu");
    Ctrl->SetColumn(DTCFG, cfg::showInNetsTb, DT_CT_INT, 1, "kZmieniacz/showInNetsTb");

    Ctrl->SetColumn(DTCFG, cfg::showToolbar, DT_CT_INT, 1, "kZmieniacz/showToolbar");
    Ctrl->SetColumn(DTCFG, cfg::showToolbarOk, DT_CT_INT, 0, "kZmieniacz/showToolbarOk");

    Ctrl->SetColumn(DTCFG, cfg::lastStInfo, DT_CT_STR, "", "kZmieniacz/lastStInfo");
    Ctrl->SetColumn(DTCFG, cfg::lastSt, DT_CT_INT, ST_ONLINE, "kZmieniacz/lastSt");

    Ctrl->SetColumn(DTCFG, cfg::wnd::changeInfoOnEnable, DT_CT_INT, 1, "kZmieniacz/wnd/changeInfoOnEnable");
    Ctrl->SetColumn(DTCFG, cfg::wnd::changeOnEnable, DT_CT_INT, 1, "kZmieniacz/wnd/changeOnEnable");

    return(1);
  }

  int IPrepare() {
    MRUlist = new MRU(cfg::mruName, cfg::mruSize, true);

    pCtrl = new Control();
    wCtrl = new StatusWnd("kZStatusWnd");

    lCtrl = new NetList(cfg::netChange, ui::cfgGroup, dynAct::dynAct, 
      act::cfgGroupCheckCreate, act::cfgGroupCheckDestroy);
    lCtrl->addIgnored(plugsNET::konnferencja);
    lCtrl->addIgnored(plugsNET::klan);
    lCtrl->addIgnored(plugsNET::checky);
    lCtrl->addIgnored(plugsNET::actio);
    lCtrl->addIgnored(plugsNET::metak);
    lCtrl->loadNets();

    sCtrl = new Status(lCtrl, NULL, cfg::dotsAppend);
    sCtrl->addReplacementSt(plugsNET::gg, ST_CHAT, ST_ONLINE);
    sCtrl->addReplacementSt(plugsNET::gg, ST_DND, ST_AWAY);
    sCtrl->addReplacementSt(plugsNET::gg, ST_NA, ST_AWAY);

    log("[kZmieniacz::IPrepare()]: Ctrl = %i, pCtrl = %i, sCtrl = %i, wCtrl = %i, lCtrl = %i", 
      Ctrl, pCtrl, sCtrl, wCtrl, lCtrl);

    /* Registering icons */
    std::string ico;

    ico = Helpers::icon16(pCtrl->stIcon(ST_CHAT, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_CHAT), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_CHAT), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_ONLINE, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_ONLINE), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_ONLINE), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_AWAY, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_AWAY), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_AWAY), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_NA, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_NA), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_NA), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_DND, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_DND), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_DND), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_HIDDEN, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_HIDDEN), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_HIDDEN), ico.c_str());

    ico = Helpers::icon16(pCtrl->stIcon(ST_OFFLINE, 0));
    IconRegister(IML_16, pCtrl->stIcon(ST_OFFLINE), ico.c_str());
    IconRegister(IML_ICO2, pCtrl->stIcon(ST_OFFLINE), ico.c_str());

    IconRegister(IML_16, ico::logoSmall, Ctrl->hDll(), IDI_LOGO);
    IconRegister(IML_16, UIIcon(IT_LOGO, net, 0, 0), Ctrl->hDll(), IDI_LOGO);
    IconRegister(IML_ICO, UIIcon(IT_LOGO, net, 0, 0), Ctrl->hDll(), IDI_LOGO);

    /* Adding configuration tabs */
    UIGroupAdd(IMIG_CFG_PLUGS, ui::cfgGroup, ACTR_SAVE, "kZmieniacz", ico::logoSmall);

    /* Plugin info box */
    char header[400];
    sprintf(header,
      "<span class='note'>Skompilowano: <b>%s</b> [<b>%s</b>]</span><br/>"
      "Informacje o wtyczce na stronie projektu "
      "<b>KPlugins</b> (http://kplugins.net/)<br/><br/>"
      "Copyright © 2006 <b>Sijawusz Pur Rahnama</b><br/>"
      "Copyright © 2003-2006 <b>Kamil 'Olórin' Figiela</b><br/>"
      "Copyright © 2003-2006 <b>KPlugins Team</b>",
      __DATE__, __TIME__);

    char desc[] = 
      "Wtyczka <b>kZmieniacz</b> umo¿liwia ustawienie statusu opisowego w ka¿dej "
      "sieci indywidualnie, oraz we wszystkich sieciach jednoczeœnie.";

    UIActionCfgAddPluginInfoBox2(ui::cfgGroup, desc, header, Helpers::icon16(ico::logoSmall).c_str(), -3);

    /* Main tab */
    /* |-> General settings group */
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Ustawienia");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Dodawaj '...' przy skracaniu d³ugich opisów", cfg::dotsAppend);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK, "Osobna historia opisów dla ka¿dej sieci", cfg::sepHistory);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_SEPARATOR);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu wszystkich sieci w menu wtyczek", cfg::showInMainWindow);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu wszystkich sieci na osobnym toolbarze", 
      cfg::showToolbar);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "+ przycisk OK", cfg::showToolbarOk, 20);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu wszystkich sieci w oknie rozmowy", cfg::showInCntWindow);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu aktualnej sieci w oknie rozmowy", 
      cfg::thisNetStChange);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu wszystkich sieci w menu w tray", cfg::showInTrayMenu);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "+ zmiana opisu", cfg::stInfoInTrayMenu, 20);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_CHECK | ACTSC_NEEDRESTART, "Zmiana statusu wszystkich sieci na pasku sieci", cfg::showInNetsTb);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    /* |-> Status info history group */
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUP, "Historia statusów opisowych");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_COMMENT, "Iloœæ pozycji w historii:");
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_SLIDER, "Ma³o\nDu¿o" AP_MINIMUM "1" AP_MAXIMUM "100", cfg::mruSize);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_SEPARATOR);
    UIActionCfgAdd(ui::cfgGroup, act::clearMru, ACTT_BUTTON, "wyczyœæ" AP_ICO "50", 0, 0, 0, 80, 30);
    UIActionCfgAdd(ui::cfgGroup, 0, ACTT_GROUPEND);

    /* |-> Net selection group */
    lCtrl->UIDraw(3, "Wybierz sieci, na których chcesz zmieniaæ status:");

    /* Adding status toolbar */
    if (GETINT(cfg::showToolbar)) {
      UIGroupAdd(IMIG_BAR, ui::tb::tb, 0, "Pasek kZmieniacza");
      if (GETINT(cfg::showToolbarOk)) {
        UIGroupAdd(ui::tb::tb, ui::tb::btnOk, 0, "Ustaw", pCtrl->stIcon(ST_OFFLINE));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnOn, 0, "D&ostêpny", pCtrl->stIcon(ST_ONLINE));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnFfc, 0, "&Pogadam", pCtrl->stIcon(ST_CHAT));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnAway, 0, "Z&araz wracam", pCtrl->stIcon(ST_AWAY));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnNa, 0, "&Nieosi¹galny", pCtrl->stIcon(ST_NA));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnDnd, 0, "Nie przeszka&dzaæ", pCtrl->stIcon(ST_DND));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnInv, 0, "U&kryty", pCtrl->stIcon(ST_HIDDEN));
        UIActionAdd(ui::tb::btnOk, ui::tb::btnOff, 0, "Niedo&stêpny", pCtrl->stIcon(ST_OFFLINE));
        UIActionInsert(ui::tb::btnOk, 0, 0, ACTT_SEP);
        UIActionInsert(ui::tb::btnOk, ui::tb::stInfo, 0, ACTR_INIT, "Opis: ", ico::stInfo);
      } else {
        UIGroupAdd(ui::tb::tb, ui::tb::btnOk, ACTS_HIDDEN, "Ustaw", pCtrl->stIcon(ST_OFFLINE));
      }
      UIActionAdd(ui::tb::tb, ui::tb::width, ACTR_RESIZE | ACTT_HWND, "Opis Combobox");
    }

    /* Adding icon in status bar */
    if (GETINT(cfg::showInNetsTb)) {
      UIGroupAdd(IMIG_STATUS, ui::tb::stTb, 0, "Wszystkie", pCtrl->stIcon(ST_OFFLINE));
      UIActionAdd(ui::tb::stTb, ui::tb::btnOn, 0, "D&ostêpny", pCtrl->stIcon(ST_ONLINE));
      UIActionAdd(ui::tb::stTb, ui::tb::btnFfc, 0, "&Pogadam", pCtrl->stIcon(ST_CHAT));
      UIActionAdd(ui::tb::stTb, ui::tb::btnAway, 0, "Z&araz wracam", pCtrl->stIcon(ST_AWAY));
      UIActionAdd(ui::tb::stTb, ui::tb::btnNa, 0, "&Nieosi¹galny", pCtrl->stIcon(ST_NA));
      UIActionAdd(ui::tb::stTb, ui::tb::btnDnd, 0, "Nie przeszka&dzaæ", pCtrl->stIcon(ST_DND));
      UIActionAdd(ui::tb::stTb, ui::tb::btnInv, 0, "U&kryty", pCtrl->stIcon(ST_HIDDEN));
      UIActionAdd(ui::tb::stTb, ui::tb::btnOff, 0, "Niedo&stêpny", pCtrl->stIcon(ST_OFFLINE));
    }

    /* Adding items to tray menu */
    if (GETINT(cfg::showInTrayMenu)) {
      UIActionInsert(IMIG_TRAY, ui::tb::btnOff, 0, ACTT_SEP | ACTT_BARBREAK, "Niedo&stêpny", pCtrl->stIcon(ST_OFFLINE));
      UIActionInsert(IMIG_TRAY, ui::tb::btnInv, 0, 0, "U&kryty", pCtrl->stIcon(ST_HIDDEN));
      UIActionInsert(IMIG_TRAY, ui::tb::btnDnd, 0, 0, "Nie przeszka&dzaæ", pCtrl->stIcon(ST_DND));
      UIActionInsert(IMIG_TRAY, ui::tb::btnNa, 0, 0, "&Nieosi¹galny", pCtrl->stIcon(ST_NA));
      UIActionInsert(IMIG_TRAY, ui::tb::btnAway, 0, 0, "Z&araz wracam", pCtrl->stIcon(ST_AWAY));
      UIActionInsert(IMIG_TRAY, ui::tb::btnFfc, 0, 0, "&Pogadam", pCtrl->stIcon(ST_CHAT));
      UIActionInsert(IMIG_TRAY, ui::tb::btnOn, 0, 0, "D&ostêpny", pCtrl->stIcon(ST_ONLINE));
    }

    if (GETINT(cfg::stInfoInTrayMenu)) {
      UIActionInsert(IMIG_TRAY, 0, 0, ACTT_SEP);
      UIActionInsert(IMIG_TRAY, ui::tb::stInfo, 0, ACTR_INIT, "Opis: ", ico::stInfo);
    }

    /* Buttons */
    if (GETINT(cfg::showInMainWindow)) {
      UIActionAdd(pCtrl->getPluginsGroup(), ui::tb::stInfo, 0, "Zmieñ status", ico::logoSmall);
    }

    int allNetsStChg = GETINT(cfg::showInCntWindow);
    int thisNetStChg = GETINT(cfg::thisNetStChange);

    if (allNetsStChg && thisNetStChg) {
      UIGroupAdd(IMIG_MSGTB, ui::msgTbGrp, ACTR_INIT, "Zmieñ status ...", ico::logoSmall);
      UIActionAdd(ui::msgTbGrp, ui::tb::stInfo, ACTSC_BOLD, "na wszystkich sieciach", ico::stInfo);
      UIActionAdd(ui::msgTbGrp, ui::tb::stInfoThis, 0, "na tej sieci", ico::stInfo);
    } else {
      if (allNetsStChg) UIActionAdd(IMIG_MSGTB, ui::tb::stInfo, 0, "Zmieñ status", ico::stInfo);
      if (thisNetStChg) UIActionAdd(IMIG_MSGTB, ui::tb::stInfoThis, ACTR_SHOW, "Zmieñ status", ico::stInfo);
    }
    return(1);
  }

  actionProc(sUIActionNotify_base *anBase) {
    sUIActionNotify_2params *an = static_cast<sUIActionNotify_2params*>(anBase);

    logDebug("[kZmieniacz::actionProc()]: anBase->act.id = %i, anBase->act.cnt = %i, an->code = %i", 
      anBase->act.id, anBase->act.cnt, an->code);

    lCtrl->actionHandle(anBase->act.id, an->code);

    switch (anBase->act.id) {
      case ui::tb::width: {
        if (anBase->code == ACTN_CREATEWINDOW) {
          sUIActionNotify_createWindow * an = (anBase->s_size >= sizeof(sUIActionNotify_createWindow)) ? 
            static_cast<sUIActionNotify_createWindow*>(anBase) : 0;

          DWORD size = SendMessage((HWND) an->hwndParent, (UINT) TB_GETBUTTONSIZE, 0, 0);
          an->hwnd = CreateWindow("combobox", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | CBS_DROPDOWN | 
            WS_EX_CONTROLPARENT | WS_EX_NOPARENTNOTIFY, an->x, an->y + ((HIWORD(size) - 20) / 2), 100, 100, 
            an->hwndParent, (HMENU)anBase->act.id, Ctrl->hDll(), NULL); // + (HIWORD(size) / (2 - 10))

          pCtrl->width = LOWORD(size) - an->x;
          an->w = 100;
          an->h = 20;
          an->y += 100;
          an->h += 20;

          tbProc = (WNDPROC) SetWindowLongPtr(an->hwndParent, GWL_WNDPROC, (LONG_PTR) tbProcNew);
          HWND hwnd = GetDlgItem(an->hwnd, IMIA_GGSTATUS_OFFLINE);

          SetProp(hwnd, "oldWndProc", (HANDLE) SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR) editFix));
          SendMessage(an->hwnd, WM_SETFONT, (WPARAM) an->font, true);

          stInfoTb = an->hwnd;
          pCtrl->refreshCombo(GETSTRA(cfg::lastStInfo));
        } else if (anBase->code == ACTN_DESTROYWINDOW) {
          sUIActionNotify_destroyWindow * an = (anBase->s_size >= sizeof(sUIActionNotify_destroyWindow)) ? 
            static_cast<sUIActionNotify_destroyWindow*>(anBase) : 0;

          DestroyWindow(an->hwnd);
          stInfoTb = NULL;
        }
        break;
      }

      case act::clearMru: {
        if (an->code == ACTN_ACTION) {
          MRUlist->clear();
          pCtrl->refreshCombo(GETSTRA(cfg::lastStInfo));
        }
        break;
      }

      case ui::tb::btnFfc:
      case ui::tb::btnOn:
      case ui::tb::btnAway:
      case ui::tb::btnNa:
      case ui::tb::btnDnd:
      case ui::tb::btnInv:
      case ui::tb::btnOff:
      case ui::tb::btnOk: {
        if (an->code != ACTN_ACTION) break;

        int status = -1;
        switch (anBase->act.id) {
          case ui::tb::btnFfc: status = ST_CHAT; break;
          case ui::tb::btnOn: status = ST_ONLINE; break;
          case ui::tb::btnAway: status = ST_AWAY; break;
          case ui::tb::btnNa: status = ST_NA; break;
          case ui::tb::btnDnd: status = ST_DND; break;
          case ui::tb::btnInv: status = ST_HIDDEN; break;
          case ui::tb::btnOff: status = ST_OFFLINE; break;
        }

        int len = SendMessage(stInfoTb, WM_GETTEXTLENGTH, 0, 0) + 1;
        char * buff = new char[len];
        GetWindowText(stInfoTb, buff, len);

        if (an->act.id == ui::tb::btnOk) {
          MRUlist->append(buff);
          pCtrl->changeStatus(-1, buff);
        } else {
          pCtrl->changeStatus(status);
        }

        delete [] buff;
        break;
      }

      case ui::tb::stInfoThis: {
        int net = GETCNTI(an->act.cnt, CNT_NET);
        bool isHandled = !lCtrl->isIgnored(net);

        if (an->code == ACTN_ACTION) {
          wCtrl->show(isHandled ? net : 0);
        } else if (an->code == ACTN_SHOW) {
          Helpers::chgBtn(IMIG_MSGTB, ui::tb::stInfoThis, AC_CURRENT, 0, 0, 
            (isHandled) ? 0 : ACTS_HIDDEN);
        }
        break;
      }

      case ui::msgTbGrp: {
        if (an->code == ACTN_ACTION) {
          wCtrl->show();
        } else if (an->code == ACTN_CREATEGROUP) {
          Helpers::chgBtn(ui::msgTbGrp, ui::tb::stInfoThis, AC_CURRENT, 0, 0, 
            !lCtrl->isIgnored(GETCNTI(an->act.cnt, CNT_NET)) ? 0 : ACTS_DISABLED);
        }
        break;
      }

      case ui::tb::stInfo: {
        if (an->code == ACTN_ACTION) {
          wCtrl->show();
        } else if (an->code == ACTN_CREATE) {
          std::string info = "Opis: " + sCtrl->getActualInfo(net);
          if (info.length() > 16) {
            info = info.substr(0, 16) + "...";
          }
          UIActionSetText(an->act, info.c_str());
        }
        break;
      }
    }

    if (!Ctrl->isRunning()) return(0);
    if (pCtrl->stTbNets.find(anBase->act.id) != pCtrl->stTbNets.end()) {
      int net = pCtrl->stTbNets[anBase->act.id];

      if (anBase->code == ACTN_ACTION) {
        wCtrl->show(net);
      } else if (an->code == ACTN_CREATE) {
        std::string info = "Opis: " + sCtrl->getActualInfo(net);
        if (info.length() > 16) {
          info = info.substr(0, 16) + "...";
        }
        UIActionSetText(an->act, info.c_str());
      }
    }
    return(0);
  }
}

int __stdcall IMessageProc(sIMessage_base *msgBase) {
  sIMessage_2params *msg = static_cast<sIMessage_2params*>(msgBase);

  switch (msgBase->id) {
    case IM_PLUG_NET:        return(net);
    case IM_PLUG_TYPE:       return(IMT_UI | IMT_CONFIG | IMT_PROTOCOL);
    case IM_PLUG_VERSION:    return(0);
    case IM_PLUG_SDKVERSION: return(KONNEKT_SDK_V);
    case IM_PLUG_SIG:        return((int)"KZMIENIACZ");
    case IM_PLUG_CORE_V:     return((int)"W98");
    case IM_PLUG_UI_V:       return(0);
    case IM_PLUG_NAME:       return((int)"kZmieniacz");
    case IM_PLUG_NETNAME:    return((int)"Wszystkie");
    case IM_PLUG_PRIORITY:   return(PLUGP_LOW);
    case IM_PLUG_INIT:       Plug_Init(msg->p1, msg->p2);   return(1);
    case IM_PLUG_DEINIT:     Plug_Deinit(msg->p1, msg->p2); return(1);
    case IM_SETCOLS:         return(ISetCols());
    case IM_UI_PREPARE:      return(IPrepare());
    case IM_START:           return(IStart());
    case IM_END:             return(IEnd());
    case IM_UIACTION:        return(actionProc((sUIActionNotify_base*)msg->p1));

    case IM_GET_STATUSINFO:  return((int)GETSTRA(cfg::lastStInfo));
    case IM_GET_STATUS:      return((int)GETINT(cfg::lastSt));

    case IM_PLUG_UPDATE: {
      if (!msg->p1 || msg->p1 > VERSION_TO_NUM(1,4,0,4)) break;

      // odczytujemy liste MRU
      tMRUlist list = MRU::get("dlg_gg_status_desc", 1000, false);

      // dodajemy opisy do nowej listy MRU
      MRU::append(cfg::mruName, list, GETINT(cfg::mruSize));

      logDebug("[kZmieniacz::IMessageProc(IM_PLUG_UPDATE)]: zaimportowano opisów: %i",
        list.size());
      break;
    }

    case IM_ALLPLUGINSINITIALIZED: {
      if (int oldId = Helpers::pluginExists(plugsNET::kallstatus)) {
        Ctrl->IMessage(&sIMessage_plugOut(oldId, "kZmieniacz z powodzeniem "
          "zastêpuje nie support-owan¹ \nju¿ wtyczkê kAllStatus ;)",
          sIMessage_plugOut::erNo, sIMessage_plugOut::euNowAndOnNextStart));
        return(-1);
      }
      return(1);
    }

    /*
     *  API
     */
    case api::showStatusWnd: {
      logDebug("Remote API Call [showAwayWnd]: from = %s, net = %i",
        Helpers::getPlugName(msg->sender), msg->p1);
      wCtrl->show(msg->p1);
      break;
    }

    case api::changeSt: {
      logDebug("Remote API Call [changeSt]: from = %s, status = %i",
        Helpers::getPlugName(msg->sender), msg->p1);
      pCtrl->changeStatus(msg->p1);
      break;
    }

    case api::changeStInfo: {
      logDebug("Remote API Call [changeStInfo]: from = %s, status = %i, info = %s",
        Helpers::getPlugName(msg->sender), msg->p1, nullChk((char*)msg->p2));
      pCtrl->changeStatus(msg->p1, (char*)msg->p2);
      break;
    }

    default: {
      if (Ctrl) Ctrl->setError(IMERROR_NORESULT);
      break;
    }
  }
  return(0);
}