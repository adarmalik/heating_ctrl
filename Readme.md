


```
arduino-cli compile -b esp32:esp32:esp32s3 --build-property build.partitions=huge_app --build-property upload.maximum_size=3145728 --build-property upload.maximum_data_size=1048576 --build-property build.psram_type=opi .

arduino-cli upload -p /dev/ttyUSB0 -b esp32:esp32:esp32s3 .
```

