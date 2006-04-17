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
#include "StatusWnd.h"

// dlg ctrl ID
#define STATUS_OK           0x2000
#define STATUS_EDIT         0x2001
#define STATUS_WNDDESC      0x2002
#define STATUS_CHANGE       0x2003
#define STATUS_CHANGE_INFO  0x2004
#define STATUS_EDIT_INFO    0x2005
#define STATUS_COUNTER      0x2006

namespace kZmieniacz {
  LRESULT CALLBACK statusWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
    switch(iMsg) {
      case WM_CREATE: {
        SendMessage(hWnd, WM_SETICON, (WPARAM) ICON_BIG, (LPARAM) ICMessage(IMI_ICONGET, ico::logoSmall, IML_16));
        SendMessage(hWnd, WM_SETICON, (WPARAM) ICON_SMALL, (LPARAM) ICMessage(IMI_ICONGET, ico::logoSmall, IML_16));

        LPCREATESTRUCT pCreate = (LPCREATESTRUCT) lParam;
        int net = (int) pCreate->lpCreateParams;
        sWndData *data = new sWndData(net, sCtrl->getStInfoMaxLength(net));
        SetWindowLong(hWnd, GWL_USERDATA, (LONG) data);

        wCtrl->addInstance(net, hWnd);

        HFONT font = CreateFont(-11, 0, 0, 0, 0, 0, 0, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, 
          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
        HFONT fontBold = CreateFont(-11, 0, 0, 0, FW_BOLD, 0, 0, 0, EASTEUROPE_CHARSET, OUT_DEFAULT_PRECIS, 
          CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
        SendMessage(hWnd, WM_SETFONT, (WPARAM) font, true);

        HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
          WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
          CW_USEDEFAULT, CW_USEDEFAULT,
          CW_USEDEFAULT, CW_USEDEFAULT,
          hWnd, NULL, Ctrl->hInst(), NULL);
        SetWindowPos(hwndTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        RECT rect;
        GetClientRect(hWnd, &rect);
        TOOLINFO ti;
        ZeroMemory(&ti, sizeof(TOOLINFO));

        ti.cbSize = sizeof(TOOLINFO);
        ti.uFlags = TTF_SUBCLASS;       
        ti.hinst = Ctrl->hInst();
        ti.uId = 0;
        ti.rect.left = rect.left;
        ti.rect.top = rect.top;
        ti.rect.right = rect.right;
        ti.rect.bottom = rect.bottom;

        tStats stats;
        stats.push_back( sStatus( ST_ONLINE, "Dostêpny", data->hImlOnline ) );
        if (net != plugsNET::gg) {
          stats.push_back( sStatus( ST_CHAT, "Pogadam", data->hImlChat ) );
        }
        stats.push_back( sStatus( ST_AWAY, "Zaraz wracam", data->hImlAway ) );
        if (net != plugsNET::gg) {
          stats.push_back( sStatus( ST_NA, "Nieosi¹galny", data->hImlNa ) );
          stats.push_back( sStatus( ST_DND, "Nie przeszkadzaæ", data->hImlDnd ) );
        }
        stats.push_back( sStatus( ST_HIDDEN, "Ukryty", data->hImlInv ) );
        stats.push_back( sStatus( ST_OFFLINE, "Niedostêpny", data->hImlOffline ) );

        int count = stats.size();
        int width = count * 40;
        int x = 5 + int(double(300 - width) / 2);

        for (tStats::iterator it = stats.begin(); it != stats.end(); it++) {
          HWND hWndTmp = CreateWindow("button", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_ICON,
            x, 30, 40, 20, hWnd, (HMENU) it->id, Ctrl->hInst(), NULL);
          x += 40;
          ti.hwnd = hWndTmp;
          ti.lpszText = (LPSTR) it->name;
          SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) &ti);
          wCtrl->prepareButtonImage(it->img, hWnd, net, it->id);
        }
        CheckDlgButton(hWnd, sCtrl->getActualStatus(net), BST_CHECKED);

        // pole combo - wybór opisu
        HWND hWndCombo = CreateWindow("combobox", "", WS_TABSTOP | WS_CHILD | WS_VISIBLE | CBS_AUTOHSCROLL | WS_EX_CONTROLPARENT | 
          WS_EX_NOPARENTNOTIFY | CBS_DROPDOWNLIST, 13, 58, 274, 100, hWnd, (HMENU) STATUS_EDIT, Ctrl->hInst(), NULL);
        SendMessage(hWndCombo, WM_SETFONT, (WPARAM) font, true);
        // SetProp(edit, "oldWndProc", (HANDLE) SetWindowLongPtr(GetDlgItem(hWndCombo, 0x3e9), GWLP_WNDPROC, (LONG_PTR) editFix));

        // przycisk OK
        HWND hWndTmp = CreateWindowEx(WS_EX_CONTROLPARENT, "button", "OK", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_TEXT | WS_TABSTOP, 
          212, 190, 75, 25, hWnd, (HMENU) STATUS_OK, Ctrl->hInst(), NULL);
        SendMessage(hWndTmp, WM_SETFONT, (WPARAM) font, true);

        // napis
        hWndTmp = CreateWindow("static", "Zmieñ status i/lub opis", WS_CHILD | WS_VISIBLE | SS_CENTER, 
          13, 8, 274, 15, hWnd, (HMENU) STATUS_WNDDESC, Ctrl->hInst(), NULL);
        SendMessage(hWndTmp, WM_SETFONT, (WPARAM) font, true);

        if (net == kZmieniacz::net) {
          // checkbox - zmieñ status
          hWndTmp = CreateWindow("button", "status", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            13, 193, 50, 20, hWnd, (HMENU) STATUS_CHANGE, Ctrl->hInst(), NULL);      
          SendMessage(hWndTmp, WM_SETFONT, (WPARAM) font, true);

          ti.hwnd = hWndTmp;
          ti.lpszText = "Zmieñ status";
          SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) &ti);
          CheckDlgButton(hWnd, STATUS_CHANGE, GETINT(cfg::wnd::changeOnEnable) ? BST_CHECKED : 0);

          // checkbox - zmieñ opis
          hWndTmp = CreateWindow("button", "opis", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            73, 193, 40, 20, hWnd, (HMENU) STATUS_CHANGE_INFO, Ctrl->hInst(), NULL);      
          SendMessage(hWndTmp, WM_SETFONT, (WPARAM) font, true);

          ti.hwnd = hWndTmp;
          ti.lpszText = "Zmieñ opis";
          SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM) &ti);
          CheckDlgButton(hWnd, STATUS_CHANGE_INFO, GETINT(cfg::wnd::changeInfoOnEnable) ? BST_CHECKED : 0);
        }

        // odczytujemy liste mru
        tMRUlist list = MRUlist->get();

        if (GETINT(cfg::sepHistory) && (data->net != kZmieniacz::net)) {
          std::string name = pCtrl->getStInfoMruName(data->net);
          list = MRU::get(name.c_str(), GETINT(cfg::mruSize));
        }

        // wype³niamy combobox
        for (tMRUlist::iterator it = list.begin(); it != list.end(); it++) {
          SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM) (*it).c_str());
        }

        std::string info = sCtrl->getActualInfo(net).c_str();
        HWND edit = CreateWindowEx(WS_EX_CLIENTEDGE, "edit", info.c_str(), WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_VSCROLL | 
          ES_MULTILINE | ES_WANTRETURN, 13, 88, 274, 100, hWnd, (HMENU) STATUS_EDIT_INFO, Ctrl->hInst(), NULL);
        SendMessage(edit, WM_SETFONT, (WPARAM) font, true);
        SetProp(edit, "oldWndProc", (HANDLE) SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR) editFix));

        // licznik
        hWndTmp = CreateWindow("static", "Znaków:", WS_CHILD | WS_VISIBLE, 
          130, 197, 45, 15, hWnd, NULL, Ctrl->hInst(), NULL);
        SendMessage(hWndTmp, WM_SETFONT, (WPARAM) font, true);

        hWndTmp = CreateWindow("static", itos(info.length()).c_str(), WS_CHILD | WS_VISIBLE, 
          175, 197, 30, 15, hWnd, (HMENU) STATUS_COUNTER, Ctrl->hInst(), NULL);
        SendMessage(hWndTmp, WM_SETFONT, (WPARAM) fontBold, true);

        // SendMessage(hWndCombo, EM_LIMITTEXT, (WPARAM) data->maxChars, (LPARAM) 0);

        SetFocus(edit);
        SendMessage(hWndCombo, CB_SETCURSEL, 0, 0);
        break;
      }

      case WM_DESTROY: {
        sWndData *data = (sWndData*) GetWindowLong(hWnd, GWL_USERDATA);
        wCtrl->removeInstance(data->net);

        delete data;
        return(0);
      }

      case WM_CTLCOLOREDIT: {
        sWndData *data = (sWndData*) GetWindowLong(hWnd, GWL_USERDATA);
        int chars = GetWindowTextLength((HWND)lParam);

        if ((HWND) lParam != (HWND) GetDlgItem(hWnd, STATUS_EDIT_INFO))
          break;

        SetTextColor((HDC)wParam, (data->maxChars && (chars > data->maxChars)) ? RGB(255,0,0) : GetSysColor(COLOR_WINDOWTEXT));
        SetBkColor((HDC)wParam, GetSysColor(COLOR_WINDOW));

        return((LRESULT)GetSysColorBrush(COLOR_WINDOW));
      }

      case WM_COMMAND: {
        switch(LOWORD(wParam)) {
          case 1:
          case STATUS_OK: {
            sWndData *data = (sWndData*) GetWindowLong(hWnd, GWL_USERDATA);
            int st = 0, len = SendMessage(GetDlgItem(hWnd, STATUS_EDIT_INFO), WM_GETTEXTLENGTH, 0, 0) + 1;
            char * msg = new char[len];

            GetWindowText(GetDlgItem(hWnd, STATUS_EDIT_INFO), msg, len);

            if (IsDlgButtonChecked(hWnd, ST_ONLINE)) st = ST_ONLINE;
            if (IsDlgButtonChecked(hWnd, ST_CHAT)) st = ST_CHAT;
            if (IsDlgButtonChecked(hWnd, ST_AWAY)) st = ST_AWAY;
            if (IsDlgButtonChecked(hWnd, ST_NA)) st = ST_NA;
            if (IsDlgButtonChecked(hWnd, ST_DND)) st = ST_DND;
            if (IsDlgButtonChecked(hWnd, ST_HIDDEN)) st = ST_HIDDEN;
            if (IsDlgButtonChecked(hWnd, ST_OFFLINE)) st = ST_OFFLINE;

            if (data->net == kZmieniacz::net) {
              MRUlist->append(msg);

              SETINT(cfg::wnd::changeOnEnable, (IsDlgButtonChecked(hWnd, STATUS_CHANGE) == BST_CHECKED) ? 1 : 0);
              SETINT(cfg::wnd::changeInfoOnEnable, (IsDlgButtonChecked(hWnd, STATUS_CHANGE_INFO) == BST_CHECKED) ? 1 : 0);

              if (GETINT(cfg::wnd::changeInfoOnEnable))
                pCtrl->changeStatus(GETINT(cfg::wnd::changeOnEnable) ? st : -1, msg);
              else if (GETINT(cfg::wnd::changeOnEnable))
                pCtrl->changeStatus(st);
            } else {
              if (GETINT(cfg::sepHistory)) {
                std::string name = pCtrl->getStInfoMruName(data->net);
                MRU::append(name.c_str(), msg, GETINT(cfg::mruSize));
              }
              pCtrl->changeStatus(st, msg, data->net);
            }

            delete [] msg;
            DestroyWindow(hWnd);
            break;
          }

          case 2:
          case SC_CLOSE: {
            DestroyWindow(hWnd);
            break;
          }

          default: {
            if (HIWORD(wParam) == CBN_SELCHANGE) {
              int len = SendMessage(GetDlgItem(hWnd, STATUS_EDIT), WM_GETTEXTLENGTH, 0, 0) + 1;
              char * msg = new char[len];

              GetWindowText(GetDlgItem(hWnd, STATUS_EDIT), msg, len);
              SetWindowText(GetDlgItem(hWnd, STATUS_EDIT_INFO), msg);

              SetWindowText(GetDlgItem((HWND)hWnd, STATUS_COUNTER), itos(len - 1).c_str());

              delete [] msg;
              return(1);
            } else if (HIWORD(wParam) == EN_UPDATE) {
              sWndData *data = (sWndData*) GetWindowLong(hWnd, GWL_USERDATA);
              int chars = GetWindowTextLength((HWND)lParam);

              SetWindowText(GetDlgItem((HWND)hWnd, STATUS_COUNTER), itos(chars).c_str());
              if (data->maxChars && (chars == data->maxChars || chars == (data->maxChars + 1)))
                InvalidateRect((HWND)lParam, NULL, true);
            }
            break;
          }
        }
        break;
      }
    }
    return(DefWindowProc(hWnd, iMsg, wParam, lParam));
  }

  StatusWnd::StatusWnd(std::string className) {
    this->className = className;
    this->classRegister();
  }

  StatusWnd::~StatusWnd() {
    this->classUnRegister();
  }

  void StatusWnd::classRegister() {
    WNDCLASSEX statusWnd;
    ZeroMemory(&statusWnd, sizeof(WNDCLASSEX));
    statusWnd.cbSize = sizeof(statusWnd);
    statusWnd.style = CS_HREDRAW | CS_VREDRAW;
    statusWnd.lpfnWndProc = statusWndProc;
    statusWnd.cbClsExtra = 0;
    statusWnd.cbWndExtra = 0;
    statusWnd.hInstance = Ctrl->hInst();
    statusWnd.hCursor = LoadCursor(NULL, IDC_ARROW);
    statusWnd.lpszMenuName = NULL;
    statusWnd.lpszClassName = this->className.c_str();
    statusWnd.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    statusWnd.hIcon = LoadIcon(Ctrl->hDll(), MAKEINTRESOURCE(179));
    statusWnd.hIconSm = LoadIcon(Ctrl->hDll(), MAKEINTRESOURCE(179));
    RegisterClassEx(&statusWnd);
  }

  void StatusWnd::classUnRegister() {
    UnregisterClass(this->className.c_str(), Ctrl->hInst());
  }

  void StatusWnd::show(int net) {
    net = (!net) ? kZmieniacz::net : net;

    if (this->haveInstance(net)) {
      SetActiveWindow((HWND) this->getInstance(net));
    } else {
      std::string title = "Ustaw status - ";
      title += (net == kZmieniacz::net) ? "wszystkie sieci" : (char*) Ctrl->IMessage(IM_PLUG_NETNAME, net);

      HWND hWnd = CreateWindowEx(NULL, this->className.c_str(), title.c_str(), WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
        (GetSystemMetrics(SM_CXSCREEN) / 2) - ((300 - (GetSystemMetrics(SM_CXFIXEDFRAME) * 2)) / 2),
        (GetSystemMetrics(SM_CYSCREEN) / 2) - ((100 - (GetSystemMetrics(SM_CYFIXEDFRAME) * 2) + GetSystemMetrics(SM_CYCAPTION)) / 2),
        300 + GetSystemMetrics(SM_CXFIXEDFRAME) * 2,220 + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),
        NULL, NULL, Ctrl->hInst(), (void*) net);
    }
  }

  void StatusWnd::prepareButtonImage(HIMAGELIST &hIml, HWND hWnd, int net, int status) {
    hWnd = GetDlgItem(hWnd, status);

    if (ICMessage(IMC_ISWINXP)) {
      hIml = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 7, 0);
      HICON hIco = (HICON) IMessage(IMI_ICONGET, 0, 0, UIIcon(IT_STATUS, net, status, 0), IML_ICO2);

      if (ImageList_ReplaceIcon(hIml, -1, hIco) == -1) { // normal
        hIco = (HICON) IMessage(IMI_ICONGET, 0, 0, UIIcon(IT_STATUS, 0, status, 0), IML_16);
        ImageList_ReplaceIcon(hIml, -1, hIco);
      }

      ImageList_ReplaceIcon(hIml, -1, hIco); // hover
      ImageList_ReplaceIcon(hIml, -1, hIco); // pressed
      ImageList_ReplaceIcon(hIml, -1, hIco); // disabled
      ImageList_ReplaceIcon(hIml, -1, hIco); // focused
      ImageList_ReplaceIcon(hIml, -1, hIco); // focus + hover
      ImageList_ReplaceIcon(hIml, -1, hIco); // pressed + ??
      // DestroyIcon(hIco); // wedlug Konnekt SDK to jest kopia do usuniecia - naprawde nie jest to kopia

      BUTTON_IMAGELIST bil;
      ZeroMemory(&bil, sizeof(BUTTON_IMAGELIST));
      bil.himl = hIml;
      bil.margin.left = 0;
      bil.margin.top = 0;
      bil.margin.right = 0;
      bil.margin.bottom = 0;
      bil.uAlign = BUTTON_IMAGELIST_ALIGN_LEFT;
      Button_SetImageList(hWnd, &bil);
    } else {
      SendMessage(hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM) IMessage(IMI_ICONGET, 0, 0, UIIcon(IT_STATUS, net, status, 0), IML_ICO2));
    }
  }
}