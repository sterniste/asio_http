#!/bin/sh
APP_URI=http://localhost:8567
BASIC_AUTH="dGVzdDp0ZXN0"
curl -X GET -H "Authorization: Basic $BASIC_AUTH" -H "Content-Type: application/json" -H "Cache-Control: no-cache" "$APP_URI"/app
