#pragma once

#if !defined(__cplusplus)
#error C++11 required
#endif

extern "C"
{

//-----------------------------------------------------------------------------
// Version
//-----------------------------------------------------------------------------

/// <summary>
/// The macro definition of the current library version. It is a '.' delimited
/// string where the fields have the following meanings. They are in order from
/// left to right:
/// YYYY Release year
/// MM Release month
/// YYYYMMDD Date string describing the date the build was created
/// rrr QFE number (000 indicates base release)
/// </summary>
#define XAL_VERSION "2022.10.20221205.000"

}
