./daytime_client time.nist.gov  
58700 19-08-05 07:32:10 50 0 0 646.8 UTC(NIST) *  

./daytime_client localhost  
Mon Aug  5 18:01:10 2019

NIST Internet Time Servers  
https://tf.nist.gov/tf-cgi/servers.cgi  


https://tools.ietf.org/html/rfc867  
One daytime service is defined as a connection based application on
TCP.  A server listens for TCP connections on TCP port 13.  Once a
connection is established the current date and time is sent out the
connection as a ascii character string (and any data received is
        thrown away).  The service closes the connection after sending the
quote.

