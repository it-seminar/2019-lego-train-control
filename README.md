# lego-train-control

## Other repositories

* [lego-train-server](https://github.com/eguahlak/lego-train-server)

## Protocol

| Name               | Key  | Value(s)     | Type         | Length |
| :----------------- | ---: | :----------- | :----------- | -----: |
| TRAIN_FORWARD      |    2 | 0/1 (Fwd/Bck)| Byte         |      2 |
| TRAIN_SPEEP_SET    |    3 | speed        | Unsigned int |      5 |
| TRAIN_BREAK        |    4 | -            | None         |      1 |
| TRAIN_SPEED_STATUS |    5 | speed        | Unsigned int |      5 |
| TRAIN_POSITION     |    6 | magnet count | Int          |      5 |
| SWITCH_SET         |    7 | 0/1 (L/R)    | Byte         |      2 |
| SWITCH_STATUS      |    8 | 0/1 (L/R)    | Byte         |      2 |
| LIGHT_SET          |    9 | 0-255 (Light ID); 0/1 (Red/Green) | Byte | 3 |
| LIGHT_STATUS       |   10 | 0-255 (Light ID); 0/1 (Red/Green) | Byte | 3 |
| REQUEST_INFO       |   11 | -            | None         |      1 |
| DEVICE_INFO        |   12 | 0/1 (Train/Swicth); 0-255 (Device ID) | Byte | 3 |
