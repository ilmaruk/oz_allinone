#pragma once

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

#include "secrets.h"

namespace OTAWeb {
    static WebServer server(80);

    static bool uploadAuthenticated = false;
    static bool uploadFailed = false;

    static const char* headers[] = {
        "Origin", "Host"
    };

    static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, initial-scale=1">
    <title>ESP32 Firmware Update</title>
    <style>
        body {
            font-family: sans-serif;
            max-width: 500px;
            margin: 50px auto;
            padding: 20px;
            background: #f4f4f4;
        }
        .panel {
            background: white;
            padding: 25px;
            border-radius: 10px;
        }
        input, button {
            width: 100%;
            padding: 12px;
            margin-top: 15px;
            box-sizing: border-box;
        }
        button {
            background: #1769aa;
            color: white;
            border: 0;
            border-radius: 5px;
            cursor: pointer;
        }
        #status {
            margin-top: 20px;
        }
    </style>
</head>
<body>
<div class="panel">
    <h2>ESP32 Firmware Update</h2>
    <p>Select a compiled firmware (.bin) file:</p>

    <form method="POST"
          action="/update"
          enctype="multipart/form-data">

        <input type="file"
               name="firmware"
               accept=".bin"
               required>

        <button type="submit">Upload firmware</button>
    </form>

    <p id="status">
        Device firmware update interface
    </p>
</div>
</body>
</html>
)rawliteral";


    static bool authenticate() {
        if (server.authenticate(OTA_USERNAME, OTA_PASSWORD)) {
            return true;
        }

        server.requestAuthentication();
        return false;
    }


    static bool validOrigin() {
        String origin = server.header("Origin");
        String host = server.header("Host");

        String expected = "http://" + host;

        return origin == expected;
    }


    static void handleRoot() {
        if (!authenticate()) return;

        server.send_P(
            200,
            "text/html",
            INDEX_HTML
        );
    }


    static void handleUpload() {

        if (!uploadAuthenticated) {
            server.requestAuthentication();
            return;
        }

        server.sendHeader("Connection", "close");

        if (uploadFailed || Update.hasError()) {

            server.send(
                500,
                "text/plain",
                "Firmware update failed: " +
                    String(Update.errorString())
            );

            Serial.println("OTA update failed");
            return;
        }

        server.send(
            200,
            "text/plain",
            "Firmware successfully uploaded. Rebooting..."
        );

        Serial.println("OTA update successful!");
        Serial.println("Rebooting...");

        delay(1000);
        ESP.restart();
    }


    static void handleUploadData() {

        HTTPUpload& upload = server.upload();

        if (upload.status == UPLOAD_FILE_START) {

            uploadAuthenticated = authenticate();
            uploadFailed = false;

            if (!uploadAuthenticated) {
                return;
            }

            // Protect against cross-origin upload requests.
            if (!validOrigin()) {
                uploadFailed = true;
                uploadAuthenticated = false;
                Serial.println("OTA rejected: invalid Origin");
                return;
            }

            Serial.printf(
                "OTA upload started: %s\n",
                upload.filename.c_str()
            );

            // Reserve the OTA partition.
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
                Update.printError(Serial);
                uploadFailed = true;
            }

        }
        else if (upload.status == UPLOAD_FILE_WRITE) {

            if (!uploadAuthenticated || uploadFailed) {
                return;
            }

            size_t written = Update.write(
                upload.buf,
                upload.currentSize
            );

            if (written != upload.currentSize) {
                Update.printError(Serial);
                uploadFailed = true;
            }

        }
        else if (upload.status == UPLOAD_FILE_END) {

            if (!uploadAuthenticated || uploadFailed) {
                return;
            }

            if (Update.end(true)) {

                Serial.printf(
                    "OTA complete: %u bytes\n",
                    upload.totalSize
                );

            } else {

                Update.printError(Serial);
                uploadFailed = true;
            }

        }
        else if (upload.status == UPLOAD_FILE_ABORTED) {

            Update.abort();
            uploadFailed = true;

            Serial.println("OTA upload aborted");
        }
    }


    inline void begin() {

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("OTA: WiFi not connected");
            return;
        }

        MDNS.begin(DEVICE_ID);

        server.collectHeaders(headers, 2);

        server.on(
            "/",
            HTTP_GET,
            handleRoot
        );

        server.on(
            "/update",
            HTTP_GET,
            handleRoot
        );

        server.on(
            "/update",
            HTTP_POST,
            handleUpload,
            handleUploadData
        );

        server.begin();

        Serial.printf(
            "OTA web server ready: http://%s.local/update\n",
            DEVICE_ID
        );

        Serial.printf(
            "Or use http://%s/update\n",
            WiFi.localIP().toString().c_str()
        );
    }


    inline void handle() {
        server.handleClient();
    }

}