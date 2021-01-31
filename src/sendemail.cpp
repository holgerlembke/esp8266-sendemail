#include "sendemail.h"

String SendEmail::readClient(WiFiClient * client)
{
  String r = client->readStringUntil('\n');
  r.trim();
  while (client->available()) {
    r += client->readString();
  }
  return r;
}

bool SendEmail::CallbackCall(maildatatype_t data) {
  serverdata.data = data;
  serverdata.value = "";
  return datacallback(&serverdata);
}

bool SendEmail::send()
{
  bool more; // needed later

  CallbackCall(ahost);
  String host = serverdata.value;
  CallbackCall(aport);
  int port = serverdata.value.toInt();

  if (!host.length()) {
    return false;
  }
  WiFiClient * client = new WiFiClient();

  CallbackCall(atimeout);
  client->setTimeout(serverdata.value.toInt());

  // smtp connect
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("Connecting: "));
  DEBUG_EMAIL_PORT.print(host);
  DEBUG_EMAIL_PORT.print(F(":"));
  DEBUG_EMAIL_PORT.println(port);
#endif
  if (!client->connect(host.c_str(), port)) {
    client->stop();
    delete client;
    return false;
  }
  String buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  if (!buffer.startsWith(F("220"))) {
    client->stop();
    delete client;
    return false;
  }

  client->print(F("EHLO "));
  client->println(client->localIP().toString());
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("EHLO "));
  DEBUG_EMAIL_PORT.println(client->localIP().toString());
#endif
  buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  if (!buffer.startsWith(F("250"))) {
    client->stop();
    delete client;
    return false;
  }

  {
    CallbackCall(auser);
    String user = serverdata.value;

    CallbackCall(apassword);
    String passwd = serverdata.value;

    if (user.length() > 0  && passwd.length() > 0 ) {
      buffer = F("AUTH LOGIN");
      client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      if (!buffer.startsWith(F("334"))) {
        client->stop();
        delete client;
        return false;
      }
      base64 b;
      buffer = user;
      buffer = b.encode(buffer);
      client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      if (!buffer.startsWith(F("334"))) {
        client->stop();
        delete client;
        return false;
      }
      buffer = passwd;
      buffer = b.encode(buffer);
      client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(buffer);
#endif
      if (!buffer.startsWith(F("235"))) {
        client->stop();
        delete client;
        return false;
      }
    }
  }

  // Envelope begin

  client->print(F("MAIL FROM: "));
  CallbackCall(aenvelopefrom);
  client->println(serverdata.value);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("MAIL FROM: "));
  DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
  buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  if (!buffer.startsWith(F("250"))) {
    client->stop();
    delete client;
    return false;
  }

  client->print(F("RCPT TO: "));
  CallbackCall(aenvelopeto);
  client->println(serverdata.value);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("RCPT TO: "));
  DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
  buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  if (!buffer.startsWith(F("250"))) {
    client->stop();
    delete client;
    return false;
  }

  // Envelope end, Mail starts

  buffer = F("DATA");
  client->println(buffer);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  buffer = readClient(client);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(buffer);
#endif
  if (!buffer.startsWith(F("354"))) {
    client->stop();
    delete client;
    return false;
  }

  // Multiple Tos, & Ccs
  for (int i = 0; i < 2; i++) {
    serverdata.callcount = 0;
    do {
      serverdata.value = "";
      if (i == 0) {
        more = CallbackCall(ato);
      } else {
        more = CallbackCall(acc);
      }
      if (serverdata.value != "") {
        if (serverdata.callcount == 0) {
          if (i == 0) {
            client->print(F("To: "));
          } else {
            client->print(F("Cc: "));
          }
        }
        serverdata.callcount++;
        client->print(serverdata.value);
        if (more) {
          client->print(", ");
        }
#ifdef DEBUG_EMAIL_PORT
        if (i == 0) {
          DEBUG_EMAIL_PORT.print(F("To: "));
        } else {
          DEBUG_EMAIL_PORT.print(F("Cc: "));
        }
        DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
      }
    } while (more); // should check connected-status, too
    if (serverdata.callcount != 0) {
      client->println();
    }
  }

  client->print(F("From: "));
  CallbackCall(afrom);
  client->println(serverdata.value);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("From: "));
  DEBUG_EMAIL_PORT.println(serverdata.value);
#endif

  client->print(F("Subject: "));
  CallbackCall(asubject);
  client->println(serverdata.value);
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.print(F("Subject: "));
  DEBUG_EMAIL_PORT.println(serverdata.value);
#endif

  // extra headers?
  serverdata.callcount = 0;
  more = CallbackCall(axtraheader);
  if (serverdata.value != "") {
    client->println(serverdata.value);
    serverdata.callcount++;
#ifdef DEBUG_EMAIL_PORT
    DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
    while (more) {
      more = CallbackCall(axtraheader);
      client->println(serverdata.value);
      serverdata.callcount++;
#ifdef DEBUG_EMAIL_PORT
      DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
    }
  }

  // End of Header
  client->println(F("\r\n"));

  // Fetch multiple body lines.
  serverdata.data = amessage;
  serverdata.callcount = 0;

#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(F("Message body"));
#endif
  do {
    serverdata.value = "";
    more = messagebodycallback(&serverdata);
    serverdata.callcount++;
    client->println(serverdata.value);
#ifdef DEBUG_EMAIL_PORT
    DEBUG_EMAIL_PORT.println(serverdata.value);
#endif
  } while (more); // should check connected-status, too

  // Bye.
  client->println('.');

  // Now. Bye.
  client->println(F("QUIT"));
#ifdef DEBUG_EMAIL_PORT
  DEBUG_EMAIL_PORT.println(F("QUIT"));
#endif

  client->stop();
  delete client;

  return true;
}

