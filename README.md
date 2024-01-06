# ws_com_test
It's simple terminal program for manage of weather station CWT-UWD and other station, which support this protocol.
This program developed with old C++ standards and can be compling with old gcc/Qt versions. With minor changes program can be compiling for QNX.
## Internal features
Program has releasing master device MODBUS-RTU protocol.
Used classes:
  * `ModBusMaster` — provide master modbus device functions
  * `ModBusMasterSub` — provide subscribers functions
  * `WeatherStation` — provide manage of weather station
  * `ConsoleManager` — provide work of terminal interface of management

For more details see code documentations.