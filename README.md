# Whoot?

## I

Ok, the other day I needed to send a daily mail with an ESP (8266 && 32). So I went out to search for a lib.

Turns out, there are some. One had lots of buffer allocations that were never freed. One sends headers that are wrongly terminated (or, at least, my server thinks that they are). One is bloated to hell and developers response to an issue was talking down to me about what I had to do and what not. ╭∩╮

## ][ 

Aside that there was this idea floating through in my synapses. Just for the giggles "write it in a different way"...

## ]I[ 

Górász Péters https://github.com/gpepe/esp8266-sendemail was a good starting point. So.

# Call backs

The usual way to work with objects (They're Made Out of Classes) is to set variouse values, then call some methodes with some parameters, set values, call methodes etc... 

This sometimes leads into a fractured program structur: "my code" and calls to "foreign code" (the lib code) is mixed. And it sometimes forces to collect a huge amount of data and then pass it to the "foreign code", thus the memory footprint might be big.

Why no turn it upside down? The "foreign code" asks for data that is needed and when it is needed. And "my code" provides the data just in time?

Like, in call back functions.

# Nothing new, please move along

Of course that is nothing really new. But I don't see it very often. So.

In the case of the mail sender the minuscule advantage is that I can create adress lists and mail bodys "on the fly" and don't need to provide pre made memory blobs (aka String).


# Status

This is a work in progress. I implement what I need and publish occasional.


# Original README.md, totally obsolete, see examples

Send email with esp8266/Arduino, smtp auth and ssl support.

Examples:

ssl:<br>
  SendEmail e("mail.example.com", 465, "login", "password", 5000, true); <br>
  e.send("test@example.com", "me@example.com", "subject", "message"); <br>
  <br>
  
plain no auth:<br>
  SendEmail e("mail.example.com", 25, "", "", 5000, false);<br>
  e.send("test@example.com", "me@example.com", "subject", "message");<br>


# technical documentation used

RFC 5322 (was RFC 2822 (was RFC 822))
