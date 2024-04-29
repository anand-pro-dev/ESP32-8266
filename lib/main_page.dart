import 'dart:convert';
import 'package:esp_ble_classic/widgets/action_button.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:flutter/material.dart';
import 'package:permission_handler/permission_handler.dart';

class MainPage extends StatefulWidget {
  const MainPage({Key? key});

  @override
  State<MainPage> createState() => _MainPageState();
}

class _MainPageState extends State<MainPage> {
  final _bluetooth = FlutterBluetoothSerial.instance;
  bool _bluetoothState = false;
  bool _isConnecting = false;
  BluetoothConnection? _connection;
  List<BluetoothDevice> _pairedDevices = [];
  List<BluetoothDevice> _unpairedDevices = [];
  BluetoothDevice? _deviceConnected;
  int times = 0;

  @override
  void initState() {
    super.initState();
    _requestPermission();
    _bluetooth.state.then((state) {
      setState(() => _bluetoothState = state.isEnabled);
    });

    _bluetooth.onStateChanged().listen((state) {
      setState(() {
        _bluetoothState = state.isEnabled;
      });
    });

    _getDevices();
  }

  void _requestPermission() async {
    await Permission.location.request();
    await Permission.bluetooth.request();
    await Permission.bluetoothScan.request();
    await Permission.bluetoothConnect.request();
  }

  void _getDevices() async {
    var bondedDevices = await _bluetooth.getBondedDevices();
    setState(() {
      _pairedDevices = bondedDevices;
    });

    // Listen for unpaired devices
    _bluetooth.startDiscovery().listen((discoveryResult) {
      if (!_pairedDevices.contains(discoveryResult.device)) {
        setState(() {
          _unpairedDevices.add(discoveryResult.device);
        });
      }
    });
  }

  void _receiveData() {
    _connection?.input?.listen((event) {
      if (String.fromCharCodes(event) == "p") {
        setState(() => times = times + 1);
      }
    });
  }

  void _sendData(String data) {
    if (_connection?.isConnected ?? false) {
      _connection?.output.add(ascii.encode(data));
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        centerTitle: true,
        title: const Text('Flutter ❤️ Arduino'),
      ),
      body: Column(
        children: [
          _controlBT(),
          _infoDevice(),
          Expanded(
            child: SingleChildScrollView(
              child: Column(
                children: [
                  _pairedDevices.isNotEmpty
                      ? _buildPairedDevicesList()
                      : const SizedBox(),
                  _unpairedDevices.isNotEmpty
                      ? _buildUnpairedDevicesExpansionTile()
                      : const SizedBox(),
                ],
              ),
            ),
          ),
          _inputSerial(),
          _buttons(),
        ],
      ),
    );
  }

  Widget _controlBT() {
    return SwitchListTile(
      value: _bluetoothState,
      onChanged: (bool value) async {
        if (value) {
          await _bluetooth.requestEnable();
        } else {
          await _bluetooth.requestDisable();
        }
      },
      tileColor: Colors.black26,
      title: Text(
        _bluetoothState ? "Bluetooth enabled" : "Bluetooth disabled",
      ),
    );
  }

  Widget _infoDevice() {
    return ListTile(
      tileColor: Colors.black12,
      title: Text("Connected to: ${_deviceConnected?.name ?? "none"}"),
      trailing: _connection?.isConnected ?? false
          ? TextButton(
              onPressed: () async {
                await _connection?.finish();
                setState(() => _deviceConnected = null);
              },
              child: const Text("Disconnect"),
            )
          : TextButton(
              onPressed: _getDevices,
              child: const Text("Refresh devices"),
            ),
    );
  }

  Widget _buildPairedDevicesList() {
    return Column(
      children: [
        const Divider(),
        const Text(
          'Paired Devices',
          style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
        ),
        ListView.builder(
          shrinkWrap: true,
          physics: NeverScrollableScrollPhysics(),
          itemCount: _pairedDevices.length,
          itemBuilder: (context, index) {
            final device = _pairedDevices[index];
            return Column(
              children: [
                if (device.name != null)
                  ListTile(
                    title: Text(device.name ?? device.address),
                    trailing: TextButton(
                      child: const Text('Connect'),
                      onPressed: () async {
                        setState(() => _isConnecting = true);

                        _connection =
                            await BluetoothConnection.toAddress(device.address);
                        _deviceConnected = device;
                        _isConnecting = false;

                        _receiveData();

                        setState(() {});
                      },
                    ),
                  ),
              ],
            );
          },
        ),
      ],
    );
  }

  Widget _buildUnpairedDevicesExpansionTile() {
    return ExpansionTile(
      title: const Text(
        'Unpaired Devices',
        style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
      ),
      children: [
        ListView.builder(
          shrinkWrap: true,
          physics: NeverScrollableScrollPhysics(),
          itemCount: _unpairedDevices.length,
          itemBuilder: (context, index) {
            final device = _unpairedDevices[index];
            return Column(
              children: [
                if (device.name != null)
                  ListTile(
                    title: Text(device.name!),
                    trailing: TextButton(
                      child: const Text('Connect'),
                      onPressed: () async {
                        setState(() => _isConnecting = true);

                        _connection =
                            await BluetoothConnection.toAddress(device.address);
                        _deviceConnected = device;
                        _isConnecting = false;

                        _receiveData();

                        setState(() {});
                      },
                    ),
                  ),
              ],
            );
          },
        ),
      ],
    );
  }

  Widget _inputSerial() {
    return ListTile(
      trailing: TextButton(
        child: const Text('Reset'),
        onPressed: () => setState(() => times = 0),
      ),
      title: Padding(
        padding: const EdgeInsets.symmetric(vertical: 16.0),
        child: Text(
          "Button pressed (x$times)",
          style: const TextStyle(fontSize: 18.0),
        ),
      ),
    );
  }

  Widget _buttons() {
    return Container(
      padding: const EdgeInsets.symmetric(vertical: 24.0, horizontal: 8.0),
      color: Colors.black12,
      child: Column(
        children: [
          const Text('LED Controls', style: TextStyle(fontSize: 18.0)),
          const SizedBox(height: 16.0),
          Row(
            children: [
              Expanded(
                child: ActionButton(
                  text: "Turn on",
                  color: Colors.green,
                  onTap: () => _sendData("1"),
                ),
              ),
              const SizedBox(width: 8.0),
              Expanded(
                child: ActionButton(
                  color: Colors.red,
                  text: "Turn off",
                  onTap: () => _sendData("0"),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
