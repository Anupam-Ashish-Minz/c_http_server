#!/bin/sh


URL="http://127.0.0.1:3000"

curl -v -X POST -d "this is sometext" -H "Content-Type: text/plain" $URL
