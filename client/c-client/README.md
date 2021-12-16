## Quick start

After install the binary broker server, it can be started with basic configuration:

```
$ bpsp_server

  INFO] Creating broker listen on 0.0.0.0:29010
  INFO] Binding...
  INFO] Listen...
  INFO] Broker listening on 0.0.0.0:29010 !

```

The borker will listen on all available network interfaces on your machine.
Then use [bpsp cli tool](https://github.com/barrydevp/bpsp/tree/master/tools/README.md) to interacting with broker:


```
$ client -s
Type support by client:                                
 -t power          range: 200w - 500w          time = 5s
 -t temperature    range: 0*C - 100*C          time = 8s
 -t percent        range: 0% - 100%          time = 8s
 -t level          range: 1 - 5                time = 20s
 -t status         range: active - deactive    time = 30s
     
eg: 
$ client -t power pub locationA/sensorA

Type : power
Broker 1.0.0BPSP - Basic Publish Subscribe Broker

Data send: 258 W
Published 5 bytes to "locationA/sensorA" .
Data send: 389 W

```