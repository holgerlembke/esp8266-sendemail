#include <ESP8266WiFi.h>
#include <sendemail.h>

// This declares the preprocessor macros privatedata_ with my, hmmm, private data.
// See https://github.com/holgerlembke/privatedata
#include <privatedata.h>

//--------------------------------------------------------
bool servercommunication(SendEmail::data_t * data) {
  /** /
    Serial.print("Data Call: ");
    Serial.print((int)data->data);
    Serial.println();
    /**/

  switch (data->data) {
    case SendEmail::ahost :         data->value = privatedata_SMTPHOST; break;
    case SendEmail::aport :         data->value = "25"; break;
    case SendEmail::auser :         data->value = privatedata_SMTPUSERNAME; break;
    case SendEmail::apassword :     data->value = privatedata_SMTPPASSWORD; break;
    case SendEmail::atimeout :      data->value = "5000"; break;
    case SendEmail::ausessl :       data->value = "f"; break; // t)rue else false
    case SendEmail::afrom :
    case SendEmail::aenvelopefrom : data->value = privatedata_MAIL1; break;
    case SendEmail::ato : 
        switch (data->callcount) {
          case 0 : data->value = privatedata_MAIL2; return true;
          case 1 : data->value = privatedata_MAIL3; return false;
        }
        // just in case, shouldn't end here
        return false;
    case SendEmail::aenvelopeto :   data->value = privatedata_MAIL2; break;
    case SendEmail::acc :           data->value = ""; break;
    case SendEmail::asubject :      data->value = "Test Mail Subject"; break;

    case SendEmail::axtraheader : {
        // just for every call feed some additional header line....
        // terminate by returning false
        switch (data->callcount) {
          case 0 : data->value = F("X-Mailer: esp8266-sendemail (https://github.com/holgerlembke/esp8266-sendemail)");
            return true;
          case 1 : data->value = F("Content-Type: text/plain; charset=UTF-8");
            return true;
          case 2 : data->value =
              "Message-ID: <" +
              String(ESP.getChipId()) + "." + String(ESP.getCycleCount()) +
              "@" + String(WiFi.hostname()) + ">";
            return false;
        }
        // just in case, shouldn't end here
        return false;
      }

    default: Serial.println(F("Panic!")); break;
  }

  return false;
}

//--------------------------------------------------------
bool mailbody(SendEmail::data_t * data) {
  data->value = "Hallo " + String(data->callcount);
  // Tricky: this will print "Hello 0" ... "Hello 10"
  // return true means: request more
  return data->callcount < 10;
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nmailtester3");

  Serial.print("Connecting to AP");

  WiFi.begin(privatedata_mySSID, privatedata_myWAP2);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  SendEmail mail;
  mail.datacallback = servercommunication;
  mail.messagebodycallback = mailbody;
  mail.send();
}

void loop() {}
