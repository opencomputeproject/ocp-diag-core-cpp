// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_THREAD_MANAGER_H
#define OCP_DIAG_SSD_OSV_THREAD_MANAGER_H

#include <functional>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>

namespace ocp_diag_ssd_osv {

class ThreadManager
{
public:
    ThreadManager() = default;
    virtual ~ThreadManager() { Join(); }

public:
    static std::shared_ptr<ThreadManager>           GetGlobalInstance();

    void			                                Launch(std::function<void(void)> func);
    void			                                Join();

private:
    std::vector<std::thread>                        threads_;
    std::mutex                                      mutex_;
};

}

#endif //FULL_SPEC_THREAD_MANAGER_H