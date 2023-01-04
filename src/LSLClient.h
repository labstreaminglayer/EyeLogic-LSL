// -----------------------------------------------------------------------
// Copyright (C) 2019-2022, EyeLogic GmbH
//
// Permission is hereby granted, free of charge, to any person or
// organization obtaining a copy of the software and accompanying
// documentation covered by this license (the "Software") to use,
// reproduce, display, distribute, execute, and transmit the Software,
// and to prepare derivative works of the Software, and to permit
// third-parties to whom the Software is furnished to do so.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
// NON-INFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR ANYONE
// DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER
// LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
// OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// -----------------------------------------------------------------------

#pragma once

#include <cstdint>

using int32  = int32_t;
using int64  = int64_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

#include "elapi/ELApi.h"
#include "lsl_cpp.h"

#include <map>
#include <tuple>
#include <mutex>
#include <condition_variable>

namespace ellsl
{
class LSLClient : public elapi::ELApi::ELEventCallback, public elapi::ELApi::ELGazeSampleCallback
{
public:
    LSLClient( );
    virtual ~LSLClient( );

    bool isConnected( ) const;
    bool isStreaming( ) const;
    bool hasConsumers( ) const;

    elapi::ELApi::ReturnConnect connectELApi( );
    void                        closeStream( );

    elapi::ELApi::ReturnStart     requestTracking( int32 samplerate );
    elapi::ELApi::ReturnCalibrate requestCalibration( int32 calibration );

    std::string listFramerates( );
    std::string listCalibrations( );

private:
    void                           disconnectELApi( );
    std::unique_lock< std::mutex > openStream( int32                            samplerate,
                                               std::unique_lock< std::mutex >&& lock = { } );
    void                           shutdown( );

    void onEvent( elapi::ELApi::Event event ) override;
    void onGazeSample( const elapi::ELGazeSample& gazeSample ) override;

    void stopTracking( );

    std::tuple< std::string, std::unique_lock< std::mutex > > listReadable(
        std::map< int32, int32 >& map, std::unique_lock< std::mutex >&& lock );
    std::unique_lock< std::mutex > updateDevice( std::unique_lock< std::mutex >&& );

    // mutex secures all access to the resources below
    mutable std::mutex                            m_resourceMutex;
    std::unique_ptr< elapi::ELApi::ScreenConfig > m_screenConfig;
    std::unique_ptr< elapi::ELApi::DeviceConfig > m_deviceConfig;
    std::map< int32, int32 >                      m_hz2Mode;
    std::map< int32, int32 >                      m_pt2Mode;
    std::shared_ptr< elapi::ELApi >               m_api;
    std::unique_ptr< lsl::stream_outlet >         m_outlet;
};

}  // namespace ellsl