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

#include <cctype>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std::chrono_literals;
using namespace ellsl;

namespace
{
const std::string COM_QUITSCOPE = "q";
const std::string COM_HELP      = "help";
const std::string COM_CONNECT   = "connect";
const std::string COM_INIT      = "startstream";
const std::string COM_CLOSE     = "closestream";
const std::string COM_CALIBRATE = "calibrate";

const std::string OPT_INITRATE  = "-r";
const std::string OPT_CALIBMODE = "-m";

std::string
trim( const std::string& s, const std::string& undesired = " \t" )
{
    std::string trimmed = s.substr( s.find_first_not_of( undesired ), std::string::npos );
    return trimmed.substr( 0, trimmed.find_last_not_of( undesired ) + 1 );
}

void
getinput( std::string& input )
{
    std::cout << std::flush;
    std::getline( std::cin, input );
    if ( input.empty( ) ) {
        return;
    }
    input = trim( input );
}

bool
string2long( std::string& s, int32& i )
{
    char* e;
    auto  trimmed = trim( s );
    int32 value   = std::strtol( trimmed.c_str( ), &e, 10 );
    if ( *e == '\0' &&   // consume the entire string
         errno == 0 ) {  // error, overflow or underflow
        i = value;
        return true;
    }
    return false;
}

bool
checkConnection( const LSLClient& client )
{
    if ( client.isConnected( ) ) {
        return true;
    }
    std::cout << "LSL client is not connected to server - has the server been shut down?"
              << std::endl;
    return false;
}

void
evaluateConnect( elapi::ELApi::ReturnConnect value )
{
    switch ( value ) {
        case elapi::ELApi::ReturnConnect::SUCCESS:
            std::cout << "LSL client connected" << std::endl;
            break;
        case elapi::ELApi::ReturnConnect::FAILURE:
            std::cout << "cannot connect to server - is the server running?" << std::endl;
            break;
        case elapi::ELApi::ReturnConnect::VERSION_MISMATCH:
            std::cout << "cannot connect to server - client-server version mismatch" << std::endl;
            break;
    }
}

void
evaluateTracking( elapi::ELApi::ReturnStart value )
{
    switch ( value ) {
        case elapi::ELApi::ReturnStart::SUCCESS:
            std::cout
                << "tracking started - please note:\n  * sample stream will be (paritally) invalid until device is calibrated\n  * the previous subject's calibration may still be active"
                << std::endl;
            break;
        case elapi::ELApi::ReturnStart::ALREADY_RUNNING_DIFFERENT_FRAMERATE:
            std::cout << "cannot begin tracking - server already tracking different framerate"
                      << std::endl;
            break;
        case elapi::ELApi::ReturnStart::DEVICE_MISSING:
            std::cout << "cannot begin tracking - device connected?" << std::endl;
            break;
        case elapi::ELApi::ReturnStart::INVALID_FRAMERATE_MODE:
            std::cout << "cannot begin tracking - framerate not supported" << std::endl;
            break;
        case elapi::ELApi::ReturnStart::NOT_CONNECTED:
            std::cout << "cannot begin tracking - is the server running?" << std::endl;
            break;
        case elapi::ELApi::ReturnStart::FAILURE:
            std::cout << "cannot begin tracking" << std::endl;
    }
}

void
evaluateCalibration( elapi::ELApi::ReturnCalibrate value )
{
    switch ( value ) {
        case elapi::ELApi::ReturnCalibrate::SUCCESS:
            std::cout << "successfully calibrated" << std::endl;
            break;
        case elapi::ELApi::ReturnCalibrate::ALREADY_BUSY:
            std::cout << "cannot begin calibration - calibration or validation in progress"
                      << std::endl;
            break;
        case elapi::ELApi::ReturnCalibrate::INVALID_CALIBRATION_MODE:
            std::cout << "cannot begin calibration - calibration mode not supported" << std::endl;
            break;
        case elapi::ELApi::ReturnCalibrate::NOT_CONNECTED:
            std::cout << "cannot begin calibration - not connected: is the server running?"
                      << std::endl;
            break;
        case elapi::ELApi::ReturnCalibrate::NOT_TRACKING:
            std::cout << "cannot begin calibration - client not in trackign mode: call startstream"
                      << std::endl;
            break;
        case elapi::ELApi::ReturnCalibrate::FAILURE:
            std::cout << "calibration failed" << std::endl;
    }
}

std::string
helpMessage( )
{
    int32             commandwidth = 30;
    int32             indentwidth  = commandwidth + 1;
    std::stringstream ss;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_HELP + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "displays this message" << std::endl;

    ss << std::endl;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_QUITSCOPE + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "quits current scope - at top level, closes LSL client" << std::endl;

    ss << std::endl;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_CONNECT + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "connects LSL client to EyeLogic server" << std::endl;

    ss << std::setw( indentwidth ) << "";
    ss << "is attempted automatically upon startup" << std::endl;

    ss << std::endl;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_INIT + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "initializes device (if necessary) and starts LSL stream" << std::endl;

    ss << std::setw( indentwidth ) << std::right << "Note: ";
    ss << "LSL stream will be empty as long as device is not calibrated" << std::endl;

    ss << std::endl
       << "  " << OPT_INITRATE << " <SAMPLERATE>" << std::setw( indentwidth - 17 ) << "";
    ss << "optional" << std::endl;
    ss << std::setw( indentwidth ) << "";
    ss << "determines the sampling mode the device is initialized in" << std::endl;

    ss << std::setw( indentwidth ) << "";
    ss << "this will determine the samplerate of the LSL stream" << std::endl;

    ss << std::setw( indentwidth ) << std::right << "Note: ";
    ss << "must align with the device's currently active sampling rate!" << std::endl;

    ss << std::endl;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_CALIBRATE + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "triggers a device calibration - place the subject within the" << std::endl;
    ss << std::setw( indentwidth ) << "";
    ss << "tracking box and advise them to watch the red dot on the screen" << std::endl;

    ss << std::endl << "  " << OPT_CALIBMODE << " <TYPE>" << std::setw( indentwidth - 11 ) << "";
    ss << "optional" << std::endl;
    ss << std::setw( indentwidth ) << "";
    ss << "determines the calibration type" << std::endl;

    ss << std::endl;

    ss << std::setfill( '.' );
    ss << std::setw( commandwidth ) << std::left << COM_CLOSE + " "
       << " ";
    ss << std::setfill( ' ' );
    ss << "closes LSL stream" << std::endl;

    ss << std::setw( indentwidth ) << std::right << "Note: ";
    ss << "device may remain initialized and running!" << std::endl;

    return ss.str( );
}

}  // namespace

int
main( )
{
    std::cout << "EyeLogic LSL console. Type \"help\" for a list of available commands."
              << std::endl;

    LSLClient client;
    evaluateConnect( client.connectELApi( ) );

    std::string input;
    bool        run = true;
    while ( run ) {
        std::cout << ">> ";
        getinput( input );
        if ( input == COM_QUITSCOPE ) {
            run = false;
        } else if ( input == COM_HELP ) {
            std::cout << helpMessage( );
        } else if ( input == COM_CONNECT ) {
            evaluateConnect( client.connectELApi( ) );
        } else if ( input == COM_CLOSE ) {
            if ( client.hasConsumers( ) ) {
                std::cout << "LSL stream currently being consumed, close anyway? - y/n: ";

                bool repeat = true;
                bool anyway = false;
                while ( repeat ) {
                    std::string sAnyway;
                    getinput( sAnyway );
                    if ( !( sAnyway == "y" || sAnyway == "n" ) ) {
                        std::cout << "invalid input - y/n: ";
                        continue;
                    } else if ( sAnyway == "q" ) {
                        repeat = false;
                    } else {
                        anyway = ( ( sAnyway == "y" ) ? true : false );
                        repeat = false;
                    }
                }
                if ( !anyway ) {
                    continue;
                }
            }
            if ( client.isStreaming( ) ) {
                client.closeStream( );
                std::cout << "closed LSL stream" << std::endl;
            } else {
                std::cout << "cannot close stream - not streaming" << std::endl;
            }
        } else {
            if ( input.find( COM_INIT ) == 0 &&
                 ( input.length( ) == COM_INIT.length( ) || input[ COM_INIT.length( ) ] == ' ' ) ) {
                if ( !checkConnection( client ) ) {
                    continue;
                }

                std::string srate;
                int32       irate = -1;

                const std::string chooseAgain =
                    "choose tracking mode " + client.listFramerates( ) + ": ";

                size_t pos = input.find( "-r" );
                if ( pos == std::string::npos ) {
                    std::cout << chooseAgain;
                    getinput( srate );
                } else {
                    size_t end = input.find( "-", pos + 1 );
                    srate      = input.substr( pos + 3, end );
                }

                while ( irate < 0 ) {
                    if ( !string2long( srate, irate ) ) {
                        if ( srate == "q" ) {
                            break;
                        } else {
                            std::cout << "samplerate must be single integer!" << std::endl;
                            std::cout << chooseAgain;
                            getinput( srate );
                        }
                    }
                }

                if ( irate >= 0 ) {
                    evaluateTracking( client.requestTracking( irate ) );
                }

            } else if ( input.find( COM_CALIBRATE ) == 0 &&
                        ( input.length( ) == COM_CALIBRATE.length( ) ||
                          input[ COM_CALIBRATE.length( ) ] == ' ' ) ) {
                if ( !checkConnection( client ) ) {
                    continue;
                }

                std::string smode;
                int32       imode = -1;

                const std::string chooseAgain =
                    "choose calibration mode " + client.listCalibrations( ) + ": ";

                size_t pos = input.find( "-m" );
                if ( pos == std::string::npos ) {
                    std::cout << chooseAgain;
                    getinput( smode );
                } else {
                    size_t end = input.find( "-", pos + 1 );
                    smode      = input.substr( pos + 3, end );
                }

                while ( imode < 0 ) {
                    if ( !string2long( smode, imode ) ) {
                        if ( smode == "q" ) {
                            break;
                        } else {
                            std::cout << "calibration mode must be single integer!" << std::endl;
                            std::cout << chooseAgain;
                            getinput( smode );
                        }
                    }
                }

                if ( imode >= 0 ) {
                    evaluateCalibration( client.requestCalibration( imode ) );
                }

            } else {
                std::cout << "unknown command \"" << input << "\" - enter \"help\" for more info"
                          << std::endl;
            }
        }
    }

    std::cout << "Client has shut down. Press ENTER to exit." << std::endl;
    std::cin.get( );
    return 0;
}
