#pragma once

// C++ ints are 4 bytes long;
// Use byte 0 (0x000000ff) for user errors
// Bytes 1, 2 unused
// Use byte 3 (0xff000000) for code errors

namespace APEXDirectSDK::Errors
{
    const int J_USR_ERR = 0;
    const int J_MSC_ERR = 3;

    const int MUL_USR_ERR = 1 << (8*J_USR_ERR);
    const int MUL_MSC_ERR = 1 << (8*J_MSC_ERR);

    const int EC_GOOD = 0;
    const int EC_BADINPUT = 0x01;
    const int EC_NOTREADY = 0x80;

    const int EC_WRAPPED = 0x7f << (8*J_MSC_ERR);
    const int EC_UNIMPLEMENTED = 0xff << (8*J_MSC_ERR);
} // namespace APEXDirectSDK::Errors
