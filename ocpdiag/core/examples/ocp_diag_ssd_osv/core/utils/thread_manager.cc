// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/examples/ocp_diag_ssd_osv/core/utils/thread_manager.h"

namespace ocp_diag_ssd_osv {

void ThreadManager::Launch(std::function<void(void)> func) {

    std::lock_guard<std::mutex> lockGuard(mutex_);

    threads_.emplace_back([=]() {
        func();
    });
}

void ThreadManager::Join() {

    std::lock_guard<std::mutex> lockGuard(mutex_);

    for (std::thread& t : threads_)
    {
        if (t.joinable())
            t.join();
    }

    threads_.clear();
}

std::shared_ptr<ThreadManager> ThreadManager::GetGlobalInstance() {
    static std::shared_ptr<ThreadManager> inst = std::make_shared<ThreadManager>();
    return inst;
}

}