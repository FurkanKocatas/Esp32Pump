import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: ControlPage(),
    );
  }
}

class ControlPage extends StatefulWidget {
  @override
  _ControlPageState createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  final String baseUrl = 'http://192.168.0.19';
  double _potentiometerValue = 0.0;

  void sendRequest(String endpoint) async {
    final response = await http.get(Uri.parse('$baseUrl/$endpoint'));
    if (response.statusCode == 200) {
      print('Request successful: ${response.body}');
    } else {
      print('Request failed: ${response.statusCode}');
    }
  }

  void sendPotentiometerValue(double value) async {
    final response = await http.get(
        Uri.parse('$baseUrl/setPotentiometerValue?value=${value.toInt()}'));
    if (response.statusCode == 200) {
      print('Potentiometer value set: ${response.body}');
    } else {
      print('Request failed: ${response.statusCode}');
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Arduino Controller'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            ElevatedButton(
              onPressed: () => sendRequest('toggleRedLed'),
              child: Text('Toggle Red LED'),
            ),
            ElevatedButton(
              onPressed: () => sendRequest('toggleYellowLed'),
              child: Text('Toggle Yellow LED'),
            ),
            Text(
              'Adjust Potentiometer',
              style: TextStyle(fontSize: 16),
            ),
            Slider(
              value: _potentiometerValue,
              min: 0,
              max: 100,
              divisions: 100,
              label: _potentiometerValue.round().toString(),
              onChanged: (double value) {
                setState(() {
                  _potentiometerValue = value;
                });
                sendPotentiometerValue(value);
              },
            ),
          ],
        ),
      ),
    );
  }
}
