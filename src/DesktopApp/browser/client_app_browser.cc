// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser/client_app_browser.h"

#include "cef/base/cef_logging.h"
#include "cef/cef_cookie.h"
#include "browser/main_message_loop_external_pump.h"
#include "common/client_switches.h"

namespace client {

ClientAppBrowser::ClientAppBrowser() {
  CreateDelegates(delegates_);
}

void ClientAppBrowser::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
  // Pass additional command-line flags to the browser process.
  if (process_type.empty()) {

	  command_line->AppendSwitch(switches::kUseViews);
	  command_line->AppendSwitch(switches::kHideFrame);
	  command_line->AppendSwitch(switches::kHideControls);
	  command_line->AppendSwitchWithValue("disable-web-security", "true");

    // Pass additional command-line flags when off-screen rendering is enabled.
    if (command_line->HasSwitch(switches::kOffScreenRenderingEnabled) &&
        !command_line->HasSwitch(switches::kSharedTextureEnabled)) {
      // Use software rendering and compositing (disable GPU) for increased FPS
      // and decreased CPU usage. This will also disable WebGL so remove these
      // switches if you need that capability.
      // See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
      if (!command_line->HasSwitch(switches::kEnableGPU)) {
        command_line->AppendSwitch("disable-gpu");
        command_line->AppendSwitch("disable-gpu-compositing");
      }
    }

    if (command_line->HasSwitch(switches::kUseViews) &&
        !command_line->HasSwitch("top-chrome-md")) {
      // Use non-material mode on all platforms by default. Among other things
      // this causes menu buttons to show hover state. See usage of
      // MaterialDesignController::IsModeMaterial() in Chromium code.
      command_line->AppendSwitchWithValue("top-chrome-md", "non-material");
    }

    if (!command_line->HasSwitch(switches::kCachePath) &&
        !command_line->HasSwitch("disable-gpu-shader-disk-cache")) {
      // Don't create a "GPUCache" directory when cache-path is unspecified.
      command_line->AppendSwitch("disable-gpu-shader-disk-cache");
    }

    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
      (*it)->OnBeforeCommandLineProcessing(this, command_line);
  }
}

void ClientAppBrowser::OnContextInitialized() {
  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(NULL);
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_, NULL);

  print_handler_ = CreatePrintHandler();

  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnContextInitialized(this);
}

void ClientAppBrowser::OnBeforeChildProcessLaunch(
    CefRefPtr<CefCommandLine> command_line) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnBeforeChildProcessLaunch(this, command_line);
}

void ClientAppBrowser::OnRenderProcessThreadCreated(
    CefRefPtr<CefListValue> extra_info) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnRenderProcessThreadCreated(this, extra_info);
}

void ClientAppBrowser::OnScheduleMessagePumpWork(int64 delay) {
  // Only used when `--external-message-pump` is passed via the command-line.
  MainMessageLoopExternalPump* message_pump =
      MainMessageLoopExternalPump::Get();
  if (message_pump)
    message_pump->OnScheduleMessagePumpWork(delay);
}

}  // namespace client
