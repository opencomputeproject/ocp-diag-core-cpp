// Copyright 2024 Samsung Electronics
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OCP_DIAG_SSD_OSV_EXCEPTION_H
#define OCP_DIAG_SSD_OSV_EXCEPTION_H

#include <string>

class OcpDiagSsdException : public std::exception {
public:
    OcpDiagSsdException() = default;
    explicit OcpDiagSsdException(const std::string& cause) : cause_(cause) { }
    ~OcpDiagSsdException() noexcept override = default;

    const char *what() const noexcept override { return cause_.c_str(); }

protected:
    std::string                     cause_;
};

#endif //OCP_DIAG_SSD_OSV_EXCEPTION_H
