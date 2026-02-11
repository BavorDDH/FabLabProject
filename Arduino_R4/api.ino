enum HttpState {
  HTTP_IDLE,
  HTTP_CONNECTING,
  HTTP_WAITING_HEADERS,
  HTTP_READING_BODY,
  HTTP_DONE,
  HTTP_ERROR
};

const char *host = "fablab-api.mia-coder-512.workers.dev";
const int port = 80;

WiFiClient wifi;
HttpClient client(wifi, host, port);

HttpState httpState = HTTP_IDLE;

unsigned long requestStartTime;

const unsigned long HTTP_TIMEOUT = 10000;

void startRequest(int departureCount, bool toTown) {
  if (httpState != HTTP_IDLE) {
    debugDisplay("NOT IDLE");
    return;
  }

  showUpdatingText(true);

  requestStartTime = millis();

  String path = "/departures?count=" + String(departureCount) + "&toTown=" + (toTown ? "true" : "false");

  int err = client.get(path);

  if (err != 0) {
    debugDisplay("ERR: " + String(err));
    httpState = HTTP_ERROR;
    return;
  }

  httpState = HTTP_WAITING_HEADERS;
}

bool pollRequest(DepartureResult &outResult) {
  if (httpState == HTTP_IDLE || httpState == HTTP_DONE || httpState == HTTP_ERROR) {
    return false;
  }

  // Timeout check
  if (millis() - requestStartTime > HTTP_TIMEOUT) {
    client.stop();
    httpState = HTTP_ERROR;
    debugDisplay("TIMEOUT");
    return false;
  }

  if (httpState == HTTP_WAITING_HEADERS) {
    if (client.available()) {
      int statusCode = client.responseStatusCode();
      if (statusCode != 200) {
        //debugDisplay(client.responseBody());
        client.stop();
        httpState = HTTP_ERROR;
        debugDisplay("Http err: " + String(statusCode));
        return false;
      }

      client.skipResponseHeaders();
      httpState = HTTP_READING_BODY;
    }
  }

  if (httpState == HTTP_READING_BODY) {
    parseResponse(outResult);

    client.stop();
    httpState = HTTP_IDLE;
    return true;
  }
  return false;
}

void parseResponse(DepartureResult &outResult) {
  StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, client);

  if (error) {
    outResult.count = 0;
    outResult.success = false;
    httpState = HTTP_ERROR;
    debugDisplay("JSON ERROR");
    return;
  }

  if (!doc.containsKey("time") || !doc.containsKey("departures") || !doc["departures"].is<JsonArray>()) {
    outResult.count = 0;
    outResult.success = false;
    debugDisplay("JSON FORMAT");
    return;
  }

  outResult.time = doc["time"].as<unsigned long long>();

  JsonArray arr = doc["departures"];
  int i = 0;
  for (JsonVariant v : arr) {
    if (i >= MAX_DEPARTURES) break;

    strncpy(outResult.departures[i], v.as<const char *>(), MAX_DEPARTURE_STR_LEN - 1);
    outResult.departures[i][MAX_DEPARTURE_STR_LEN - 1] = '\0';
    i++;
  }
  outResult.count = i;
  outResult.success = true;
}