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

#include "LSLClient.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>

#include <cassert>
#include <cmath>

using namespace ellsl;

namespace
{
const int32 NCHANNELS = 17;
}

LSLClient::LSLClient( )
{
}

LSLClient::~LSLClient( )
{
    shutdown( );
}

bool
LSLClient::isConnected( ) const
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    return m_api && m_api->isConnected( );
}

bool
LSLClient::isStreaming( ) const
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    return m_api && m_api->isConnected( ) && m_outlet;
}

bool
LSLClient::hasConsumers( ) const
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    return m_outlet && m_outlet->have_consumers( );
}

void
LSLClient::shutdown( )
{
    disconnectELApi( );
    closeStream( );
}

void
LSLClient::closeStream( )
{
    stopTracking( );

    std::unique_lock< std::mutex > lock( m_resourceMutex );
    m_outlet = nullptr;
}

std::unique_lock< std::mutex >
LSLClient::openStream( int32 samplerate, std::unique_lock< std::mutex >&& lock )
{
    assert( lock.owns_lock( ) );

    if ( !m_api ) {
        return std::move( lock );
    }

    if ( m_outlet ) {
        m_outlet = nullptr;
    }

    // create streaminfo
    lsl::stream_info lslInfo( "EyeLogic",
                              "Gaze",
                              NCHANNELS,
                              samplerate,
                              lsl::cf_double64,
                              "EyeLogic One | " + std::to_string( m_deviceConfig->deviceSerial ) );

    // append some (optional) meta-data
    lsl::xml_element channels = lslInfo.desc( ).append_child( "channels" );

    channels.append_child( "channel" )
        .append_child_value( "label", "FrameNumber" )
        .append_child_value( "type", "FrameNumber" )
        .append_child_value( "unit", "number" );

    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_X_raw" )
        .append_child_value( "eye", "both" )
        .append_child_value( "type", "ScreenX" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_Y_raw" )
        .append_child_value( "eye", "both" )
        .append_child_value( "type", "ScreenY" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_X_filtered" )
        .append_child_value( "eye", "both" )
        .append_child_value( "type", "ScreenX" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_Y_filtered" )
        .append_child_value( "eye", "both" )
        .append_child_value( "type", "ScreenY" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );

    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_X_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "ScreenX" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_Y_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "ScreenY" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_X_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "ScreenX" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Screen_Y_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "ScreenY" )
        .append_child_value( "unit", "pixels" )
        .append_child_value( "coordinate_system", "image-space" );

    channels.append_child( "channel" )
        .append_child_value( "label", "Diameter_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "Diameter" )
        .append_child_value( "unit", "millimeters" );
    channels.append_child( "channel" )
        .append_child_value( "label", "Diameter_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "Diameter" )
        .append_child_value( "unit", "millimeters" );

    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_X_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "PositionX" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_Y_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "PositionY" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_Z_left" )
        .append_child_value( "eye", "left" )
        .append_child_value( "type", "PositionZ" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_X_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "PositionX" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_Y_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "PositionY" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );
    channels.append_child( "channel" )
        .append_child_value( "label", "EyePosition_Z_right" )
        .append_child_value( "eye", "right" )
        .append_child_value( "type", "PositionZ" )
        .append_child_value( "unit", "millimeters" )
        .append_child_value( "coordinate_system", "world-space" );

    lslInfo.desc( )
        .append_child( "acquisition" )
        .append_child_value( "manufacturer", "EyeLogic" )
        .append_child_value( "model", "One" )
        .append_child_value( "serial number", std::to_string( m_deviceConfig->deviceSerial ) );

    // instantiate new m_outlet
    m_outlet = std::make_unique< lsl::stream_outlet >( lslInfo );

    return std::move( lock );
}

void
LSLClient::disconnectELApi( )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    m_api->disconnect( );
}

elapi::ELApi::ReturnConnect
LSLClient::connectELApi( )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );

    if ( !m_api ) {
        m_api = std::make_unique< elapi::ELApi >( "LSL Client" );
    }
    // connect to server
    const auto retConnect = m_api->connect( );
    if ( retConnect != elapi::ELApi::ReturnConnect::SUCCESS ) {
        return retConnect;
    }

    // register ELApi callback handlers
    m_api->registerEventListener( this );
    m_api->registerGazeSampleListener( this );

    lock = updateDevice( std::move( lock ) );

    return retConnect;
}

elapi::ELApi::ReturnStart
LSLClient::requestTracking( int32 samplerate )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );

    if ( !m_api ) {
        return elapi::ELApi::ReturnStart::FAILURE;
    }
    if ( m_hz2Mode.count( samplerate ) == 0 ) {
        return elapi::ELApi::ReturnStart::INVALID_FRAMERATE_MODE;
    }

    auto retTracking = m_api->requestTracking( m_hz2Mode[ samplerate ] );
    if ( retTracking != elapi::ELApi::ReturnStart::SUCCESS ) {
        return retTracking;
    }

    if ( !m_outlet || m_outlet->info( ).nominal_srate( ) != samplerate ) {
        lock = openStream( samplerate, std::move( lock ) );
    }

    return retTracking;
}

elapi::ELApi::ReturnCalibrate
LSLClient::requestCalibration( int32 calibration )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    if ( !m_api ) {
        return elapi::ELApi::ReturnCalibrate::FAILURE;
    }
    if ( m_pt2Mode.count( calibration ) == 0 ) {
        return elapi::ELApi::ReturnCalibrate::INVALID_CALIBRATION_MODE;
    }
    return m_api->calibrate( m_pt2Mode[ calibration ] );
}

void
LSLClient::stopTracking( )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    if ( !m_api ) {
        return;
    }
    m_api->unrequestTracking( );
}

std::string
LSLClient::listFramerates( )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    std::string                    values;
    std::tie( values, lock ) = listReadable( m_hz2Mode, std::move( lock ) );
    return values + " [hz]";
}

std::string
LSLClient::listCalibrations( )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );
    std::string                    values;
    std::tie( values, lock ) = listReadable( m_pt2Mode, std::move( lock ) );
    return values;
}

void
LSLClient::onEvent( elapi::ELApi::Event event )
{
    if ( !m_api ) {
        return;
    }
    std::string out = "\n";
    switch ( event ) {
        case elapi::ELApi::Event::SCREEN_CHANGED:
            out += "stimulus screen has changed";
            break;
        case elapi::ELApi::Event::CONNECTION_CLOSED: {
            out += "server has closed the connection";
            std::unique_lock< std::mutex > lock( m_resourceMutex );
            updateDevice( std::move( lock ) );
        } break;
        case elapi::ELApi::Event::DEVICE_CONNECTED: {
            out += "a new device has connected";
            std::unique_lock< std::mutex > lock( m_resourceMutex );
            updateDevice( std::move( lock ) );
        } break;
        case elapi::ELApi::Event::DEVICE_DISCONNECTED: {
            out += "device has disconnected";
            std::unique_lock< std::mutex > lock( m_resourceMutex );
            updateDevice( std::move( lock ) );
        } break;
        case elapi::ELApi::Event::TRACKING_STOPPED:
            out += "tracking has stopped";
            break;
    }
    std::cout << out + "\n>> ";
    std::cout << std::flush;
}

void
LSLClient::onGazeSample( const elapi::ELGazeSample& gazeSample )
{
    std::unique_lock< std::mutex > lock( m_resourceMutex );

    if ( !m_outlet ) {
        return;
    }
    if ( !m_outlet->have_consumers( ) ) {
        return;
    }

    double sample[ NCHANNELS ];

    sample[ 0 ] =
        ( gazeSample.index == elapi::ELInvalidValue ? std::numeric_limits< double >::quiet_NaN( )
                                                    : gazeSample.index );
    // raw, averaged por
    sample[ 1 ] =
        ( gazeSample.porRawX == elapi::ELInvalidValue ? std::numeric_limits< double >::quiet_NaN( )
                                                      : gazeSample.porRawX );
    sample[ 2 ] =
        ( gazeSample.porRawY == elapi::ELInvalidValue ? std::numeric_limits< double >::quiet_NaN( )
                                                      : gazeSample.porRawY );
    // filtered, averaged por
    sample[ 3 ] = ( gazeSample.porFilteredX == elapi::ELInvalidValue
                        ? std::numeric_limits< double >::quiet_NaN( )
                        : gazeSample.porFilteredX );
    sample[ 4 ] = ( gazeSample.porFilteredY == elapi::ELInvalidValue
                        ? std::numeric_limits< double >::quiet_NaN( )
                        : gazeSample.porFilteredY );
    // eye por
    sample[ 5 ] =
        ( gazeSample.porLeftY == elapi::ELInvalidValue ? std::numeric_limits< double >::quiet_NaN( )
                                                       : gazeSample.porLeftX );
    sample[ 6 ] =
        ( gazeSample.porLeftY == elapi::ELInvalidValue ? std::numeric_limits< double >::quiet_NaN( )
                                                       : gazeSample.porLeftY );
    sample[ 7 ] = ( gazeSample.porRightX == elapi::ELInvalidValue
                        ? std::numeric_limits< double >::quiet_NaN( )
                        : gazeSample.porRightX );
    sample[ 8 ] = ( gazeSample.porRightY == elapi::ELInvalidValue
                        ? std::numeric_limits< double >::quiet_NaN( )
                        : gazeSample.porRightY );
    // pupil diameter
    sample[ 9 ]  = ( gazeSample.pupilRadiusLeft == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.pupilRadiusLeft );
    sample[ 10 ] = ( gazeSample.pupilRadiusRight == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.pupilRadiusRight );
    // eye world-position
    sample[ 11 ] = ( gazeSample.eyePositionLeftX == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionLeftX );
    sample[ 12 ] = ( gazeSample.eyePositionLeftY == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionLeftY );
    sample[ 13 ] = ( gazeSample.eyePositionLeftZ == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionLeftZ );
    sample[ 14 ] = ( gazeSample.eyePositionRightX == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionRightX );
    sample[ 15 ] = ( gazeSample.eyePositionRightY == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionRightY );
    sample[ 16 ] = ( gazeSample.eyePositionRightZ == elapi::ELInvalidValue
                         ? std::numeric_limits< double >::quiet_NaN( )
                         : gazeSample.eyePositionRightZ );

    // calc sample age
    auto   timestamp        = gazeSample.timestampMicroSec;
    double timestampSeconds = timestamp / 1000000.0;  // lsl expects time in seconds

    // push into LSL
    m_outlet->push_sample( sample, timestampSeconds );
}

std::unique_lock< std::mutex >
LSLClient::updateDevice( std::unique_lock< std::mutex >&& lock )
{
    assert( lock.owns_lock( ) );

    if ( !m_api ) {
        m_screenConfig = nullptr;
        m_deviceConfig = nullptr;
        m_hz2Mode.clear( );
        m_pt2Mode.clear( );
        return std::move( lock );
    }

    if ( !m_screenConfig ) {
        m_screenConfig = std::make_unique< elapi::ELApi::ScreenConfig >( );
    }
    if ( !m_deviceConfig ) {
        m_deviceConfig = std::make_unique< elapi::ELApi::DeviceConfig >( );
    }

    m_api->getActiveScreen( *m_screenConfig );
    m_api->getDeviceConfig( *m_deviceConfig );

    m_hz2Mode.clear( );
    for ( int32 i = 0; i < m_deviceConfig->numFrameRates; i++ ) {
        m_hz2Mode[ m_deviceConfig->frameRates[ i ] ] = i;
    }

    m_pt2Mode.clear( );
    for ( int32 i = 0; i < m_deviceConfig->numCalibrationMethods; i++ ) {
        m_pt2Mode[ m_deviceConfig->calibrationMethods[ i ] ] = i;
    }
    return std::move( lock );
}

std::tuple< std::string, std::unique_lock< std::mutex > >
LSLClient::listReadable( std::map< int32, int32 >& map, std::unique_lock< std::mutex >&& lock )
{
    assert( lock.owns_lock( ) );

    std::stringstream ss;
    for ( auto& value_mode : map ) {
        ss << value_mode.first << ", ";
    }
    std::string str = ss.str( );
    str             = str.substr( 0, str.length( ) - 2 );
    return { str, std::move( lock ) };
}
