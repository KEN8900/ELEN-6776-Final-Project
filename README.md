# ELEN-6776-Final-Project

This repository is for the Final Project of Columbia University Course ELEN 6776 by Hongkuan Yu and Yelin Mao. ELEN 6776 is a course about Content Storage and Distribution.



In this final project, we implemented multi-threading data control between the database and the MQTT broker. The main program, running under [Ubuntu](https://ubuntu.com/), achieves multi-threading read and write operations using [Qt6](https://www.qt.io/) (C++ 14) and is connected to an MQTT broker ([Eclipse Mosquitto](https://mosquitto.org/)) to test and simulate the results.



Main classes used are [QMqttClient Class](https://doc.qt.io/qt-6/qmqttclient.html) and [QThread Class](https://doc.qt.io/qt-6/qthread.html).



An overview of [MQTT](https://www.emqx.com/en/blog/what-is-the-mqtt-protocol) protocol can be found here.