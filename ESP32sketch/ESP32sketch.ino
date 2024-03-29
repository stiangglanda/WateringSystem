#define ANALOGPIN ADC1_CHANNEL_4  // = GPIO 32
#define AOUT_PIN 32
#define EENGINE_PIN 2

#define MAXVAL 4095
#define MINVAL 1800

#define NETWORKNAME "NETWORKNAME"
#define PASSWORD "PASSWORD"

#include "WiFi.h"
#include "Wire.h"
#include "Adafruit_SSD1306.h"
#include "driver/adc.h"

unsigned long Curtime = 0;
constexpr int interval = 1000 * 60 * 10;
WiFiServer Server(80);
WiFiClient Client;
String Request;
Adafruit_SSD1306 display(128, 64); // Argumente: Displaybreite und -höhe

int NeedsWater = 40;
int HTTP_PORT = 80;
String HTTP_METHOD = "GET";
char HOST_NAME[] = "192.168.1.124"; // change to your PC's IP address
String PATH_NAME = "/WateringSystem/insert.php";
String queryString = "?moisturedata=";

void setup()
{
	Curtime = millis();
	pinMode(AOUT_PIN, INPUT);
	pinMode(EENGINE_PIN, OUTPUT);
	display.begin(SSD1306_SWITCHCAPVCC, 60);

	Serial.begin(115200);
	delay(100);
	Serial.println();
	Serial.print("Verbinde mit: ");
	Serial.println(NETWORKNAME);

	WiFi.begin(NETWORKNAME, PASSWORD);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.print("Erfolgreich. Eigene IP-Adresse: ");
	Serial.println(WiFi.localIP());

	Server.begin();
}

void loop()
{
	int moistureVal = map(analogRead(AOUT_PIN), MINVAL, MAXVAL, 0, 100);
	Client = Server.available();

	if (Client)
	{
		Serial.println("Neuer Client");
		boolean leereZeile = true;

		while (Client.connected())
		{
			if (Client.available())
			{
				char c = Client.read();
				if (Request.length() < 100)
					Request += c;

				if (c == '\n' && leereZeile)
				{
					Serial.print("Request von Client: ");
					Serial.println(Request);


					Client.println("HTTP/1.1 200 OK");
					Client.println("Content-Type: text/html");
					Client.println("Connection: close"); // Verbindung wird nach Antwort beendet
					Client.println("Refresh: 2"); // Seite alle 25 Sekunden neu abfragen
					Client.println();
					Client.println("<!DOCTYPE HTML>");
					Client.println("<html>");
					Client.print("Analogwert: ");
					Client.print(analogRead(AOUT_PIN));
					Client.println("</html>");

					Request = "";
					break;
				}

				if (c == '\n')
					leereZeile = true;
				else if (c != '\r')
					leereZeile = false;
			}
		}

		delay(1);
		Client.stop();
		Serial.println("Verbindung mit Client beendet.");
		Serial.println("");
	}

	if (moistureVal < NeedsWater)
	{
		digitalWrite(EENGINE_PIN, HIGH);
	}
	else
	{
		digitalWrite(EENGINE_PIN, LOW);
	}

	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0, 20);
	display.println("Current moisture:");
	display.setTextSize(3);
	display.println(moistureVal);
	display.setTextSize(1);
	display.println(WiFi.localIP());

	if (millis() - Curtime > interval) //Has one second passed?
	{
		display.println("Database Entry");
		Serial.print("Database Entry: ");
		Serial.println(analogRead(AOUT_PIN));
		Curtime = millis(); //and reset time.


		// connect to web server on port 80:
		if (Client.connect(HOST_NAME, HTTP_PORT))
		{
			// if connected:
			Serial.println("Connected to server");
			// make a HTTP request:
			// send HTTP header
			Client.println(HTTP_METHOD + " " + PATH_NAME + queryString + analogRead(AOUT_PIN) + " HTTP/1.1");
			Client.println("Host: " + String(HOST_NAME));
			Client.println("Connection: close");
			Client.println(); // end HTTP header

			while (Client.connected())
			{
				if (Client.available())
				{
					// read an incoming byte from the server and print it to serial monitor:
					char c = Client.read();
					Serial.print(c);
				}
			}

			// the server's disconnected, stop the client:
			Client.stop();
			Serial.println();
			Serial.println("disconnected");
		}
		else
		{
			// if not connected:
			Serial.println("connection failed");
		}
	}
	display.display();
}
