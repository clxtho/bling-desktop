// Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_BROWSER_H_
#define CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_BROWSER_H_
#pragma once

#include "cef/cef_base.h"
#include "browser/client_app_browser.h"

namespace client {
namespace browser {

// Create the browser delegate. Called from client_app_delegates_browser.cc.
void CreateDelegates(ClientAppBrowser::DelegateSet& delegates);

}  // namespace browser
}  // namespace client

#endif  // CEF_TESTS_CEFCLIENT_BROWSER_CLIENT_BROWSER_H_
