#ifndef __SENDEMAIL_H
#define __SENDEMAIL_H

#define DEBUG_EMAIL_PORT Serial

#include <Arduino.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <base64.h>



class SendEmail
{
  public:
    // Servercommunication
    enum maildatatype_t { // login
                          ahost, aport, auser, apassword, atimeout, ausessl,
                          // Mail-Envelope
                          aenvelopefrom, aenvelopeto, 
                          // Mail
                          afrom, ato /*x*/, acc /*x*/,
                          asubject, amessage /*x*/,
                          axtraheader /*x*/ };
/*  Adresses are either
      localpart@example.invalid
     or
      some real name <localpart@example.invalid>
    Strings will be added into the mail as they are. Give your best and make it work.  
    BCC is aenvelopeto...
    
    Field with (*x*) can have multiple calls
*/
    struct data_t {
        maildatatype_t data;
        int callcount;
        String value;    
    } serverdata;
    // for .....: true: more to come, false: nothing more
    // else ignored.
    typedef bool (*servercommunication_t) (SendEmail::data_t * data);
    servercommunication_t datacallback;
    
    // mail body gets an extra callback to make it cosy
    typedef bool (*mailcommunication_t) (SendEmail::data_t * data);
    mailcommunication_t messagebodycallback;
    
  private:
    /*
    const String host;
    const int port;
    const String user;
    const String passwd;
    const int timeout;
    const bool ssl;
    */
    bool CallbackCall(maildatatype_t data);
    String readClient(WiFiClient * client);
  public:
   bool send();
};

#endif
