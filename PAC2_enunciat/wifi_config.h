/*
 * Copyright (C) 2017 Universitat Oberta de Catalunya - http://www.uoc.edu/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Universitat Oberta de Catalunya nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*----------------------------------------------------------------------------*/

#define HOME_WLAN                   ( 0 )
#define HOME_SSID                   ( "XXXXXXXXX" )
#define HOME_SEC_KEY                ( "XXXXXXXXX" )
#define HOME_SEC_TYPE               ( SL_SEC_TYPE_WPA_WPA2 )

#define WORK_WLAN                   ( 1 )
#define WORK_SSID                   ( "XXXXXXXXX" )
#define WORK_SEC_KEY                ( "XXXXXXXXX" )
#define WORK_SEC_TYPE               ( SL_SEC_TYPE_WPA_WPA2 )

#define WIFI_NETWORK                ( WORK_WLAN )

#if (WIFI_NETWORK == HOME_WLAN)
    #define SSID_NAME               ( HOME_SSID )
    #define SEC_KEY                 ( HOME_SEC_KEY )
    #define SEC_TYPE                ( HOME_SEC_TYPE )
#elif (WIFI_NETWORK == WORK_WLAN)
    #define SSID_NAME               ( WORK_SSID )
    #define SEC_KEY                 ( WORK_SEC_KEY )
    #define SEC_TYPE                ( WORK_SEC_TYPE )
#else
    #error "Undefined WIFI_NETWORK!"
#endif
