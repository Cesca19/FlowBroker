# FlowBroker Network Protocol

## 1. Introduction

The FlowBroker server is a topic-based publish/subscribe broker. A client
connects, subscribes to the topics it cares about, learns the schema of each
one, and then receives a live stream of values.

The protocol uses two channels with different jobs:

&ensp; The **control channel** is the text conversation, carried over TCP, through which a client
announces itself, subscribes and unsubscribes to topics, sets alerts, and
receives the schema of each topic. Low volume, must be reliable, human-readable,
debuggable with a tool such as netcat. Everything the client asks for goes
here, and alerts come back here.

&ensp;The **data channel** is the packed binary UDP
stream that carries the high-rate values. This first version covers the control
channel in full. High volume, low latency, one directional, from the
server to the client only. The client never sends anything over UDP.

<br>

## 2. Protocols notation
The syntax of the control messages is described with **ABNF** (Augmented
Backus-Naur Form), the same notation used by internet standards such as HTTP and
FTP. Everything used in this document is listed here.

A rule has the form `name = definition`. The left side names a thing, the right
side says what it is made of.

### Symbols:

| Symbol | Meaning                                                                                                                  |
|--------|--------------------------------------------------------------------------------------------------------------------------|
| `=` | "is defined as"                                                                                                          |
| `"SUB"` | those exact characters, literally (anything in double quotes)                                                            |
| `SP` | a single space character                                                                                                 |
| `CRLF` | the end-of-line marker: carriage return (`\r`) then line feed (`\n`). Every control message ends with it. |
| `/` | "or". `op = ">" / "<"` means an `op` is either `>` or `<`.                                                               |
| `*` | repetition. `*DIGIT` means zero or more digits.                                                                          |
| `1*` | one or more. `1*DIGIT` means at least one digit.                                                                         |
| `n*m` | between n and m repetitions. `1*5DIGIT` means one to five digits.                                                        |
| `[ ... ]` | optional: what is inside may appear once or not at all.                                                                  |
| `( ... )` | grouping, so that `/` or `*` applies to several elements together.                                                       |
| `;` | starts a comment: the rest of the line is a note for the reader.                                                         |

Two rules are borrowed from the ABNF standard:
* `ALPHA` : one letter, `a` to `z` or `A` to `Z`.
* `DIGIT` : one digit, `0` to `9`.

### Specific terms
The following terms are specific to this protocol.
* `field` : the name of one value carried by a topic, that is, one entry of the
  topic's schema. `AAPL (Finance data)` has a single field `price`; `Paris (Weather data)` has three fields
  `temp`, `wind` and `pressure`, in that order. 
* `op` : a comparison operator, one of `>=`, `<=`, `>` or `<`. It sets the
  direction of an alert threshold, for example `>` fires when the field rises
  above the value, and `<` fires when it falls below.

### Worked example.
```abnf
value = ["-"] 1*DIGIT ["." 1*DIGIT]
```
Literally it means that a `value` is an optional minus sign, then one or more digits, then
optionally a dot followed by one or more digits. So `42`, `-7`, `500.00` and
`-3.14` are all valid values, while `1.2.3` or an empty string are not.

<br>

##  3. Control channel (TCP)

### 3.1 Message framing in the control channel

TCP delivers a continuous stream of bytes, not messages. It never tells the
server "this group of bytes is one complete command". The server therefore reads
incoming bytes and accumulates them until it sees a `CRLF`. At that point it has
one complete command line, which it processes, and it starts accumulating the
next one.

Consequences:

* Every client command is exactly one line ending in `CRLF`.
* Every server reply is exactly one line ending in `CRLF`.
* A command with no `CRLF` is never processed: the server is still waiting for
  the end of the line.

For robustness the server MAY also accept a lone `LF` (`\n`) as a line
terminator, but a compliant client MUST send `CRLF`.

<br>

### 3.2 Session lifecycle
A session is the lifetime of one TCP connection. It moves through a small number
of states.

```
   (TCP connection opened)
            |
            v
      +-----------+
      | CONNECTED |
      +-----------+
            |
            |  HELLO udp_port=... (valid)
            |  the server records the client UDP endpoint
            v
      +-----------+
      |   READY   | <------------------------+
      +-----------+                          |
            | TOPICS / SUB / UNSUB / ALERT   |
            +-------------------------------+
            |
            |  BYE, or the TCP connection is closed
            v
      +-----------+
      |  CLOSED   |
      +-----------+
```

* On connection the session is **CONNECTED**. The only command accepted in this
  state is `HELLO`.
* A valid `HELLO` moves the session to **READY**. This is where the server
  records the client's UDP endpoint: the IP address is taken from the TCP
  connection, and the port is the one the client announced in `HELLO`. Without
  this, the server would not know where to push the UDP data.
* In **READY**, the client may send `TOPICS`, `SUB`, `UNSUB` and `ALERT` freely.
* A `BYE`, or a closed TCP connection, ends the session and releases all of the
  client's subscriptions. 
* Any `TOPICS`, `SUB`, `UNSUB` or `ALERT` received while still in `CONNECTED` (that
  is, before a valid `HELLO`) is rejected with `425 NOT_READY`.

<br>

### 3.3 Client commands
```abnf
; FlowBroker control protocol, client commands
 
command  = hello / topics / sub / unsub / alert / bye
 
hello    = "HELLO" SP "udp_port=" port CRLF
topics   = "TOPICS" CRLF
sub      = "SUB" SP topic CRLF
unsub    = "UNSUB" SP topic CRLF
alert    = "ALERT" SP topic SP field SP op SP value CRLF
bye      = "BYE" CRLF
 
topic    = 1*(ALPHA / DIGIT / "-" / "_" / ".")
field    = 1*(ALPHA / DIGIT / "_")
op       = ">=" / "<=" / ">" / "<"
value    = ["-"] 1*DIGIT ["." 1*DIGIT]
port     = 1*5DIGIT
```

* **HELLO** announces the client and its UDP listening port. It must be the
  first command sent. It moves the session to READY.
* **TOPICS** asks for the list of topics currently available. The reply gives the
  name and stream type of each topic, so the client can discover what exists
  before subscribing.
* **SUB** subscribes to one topic. The reply carries the `topic_id`, the stream
  type, and the schema (the ordered list of field names) so the client can
  interpret the numeric values it will receive over UDP.
* **UNSUB** cancels a subscription to one topic.
* **ALERT** asks the server to watch one field of one topic and to notify the
  client (over TCP) when the field crosses the given threshold.
* **BYE** ends the session.


<br>

### 3.4 Server responses
Every reply is a single line: a three-digit code, a space, then text.

```abnf
reply       = code SP reply-text CRLF
code        = 3DIGIT
reply-text  = *(any character except CR or LF)
```

The first digit of the code gives its category, which lets a client react to the
category without knowing every individual code:

| Range | Meaning |
|-------|---------|
| 2xx | Success |
| 3xx | Asynchronous notification pushed by the server, not a reply to a command |
| 4xx | Client error |
| 5xx | Server error |

Defined codes:

```
2xx  Success
  200  OK                 generic acknowledgement of an accepted command
  201  SUBSCRIBED         subscription established; carries topic_id, type, fields
  202  UNSUBSCRIBED       subscription cancelled
  203  ALERT_SET          alert registered; carries its id
  210  TOPICS             list of available topics; carries name:type pairs
 
3xx  Asynchronous notification (server-initiated, unsolicited)
  300  ALERT              an alert threshold was crossed
 
4xx  Client error
  400  BAD_REQUEST        malformed command
  404  UNKNOWN_TOPIC      the topic does not exist
  425  NOT_READY          a command was sent before a valid HELLO
 
5xx  Server error
  500  INTERNAL_ERROR     unexpected server-side failure
```

Reply formats that carry extra data:

```
201 SUBSCRIBED topic_id=<int> type=<TYPE> fields=[<name>,<name>,...]
203 ALERT_SET id=<int>
210 TOPICS <name>:<TYPE> <name>:<TYPE> ...
300 ALERT id=<int> <topic> <field> <value> <op> <threshold>
```

<br>

### 3.5 Protocol rules

1. **HELLO is mandatory and must come first.** The server cannot push UDP data to
   a client whose endpoint it does not know, so a subscription before `HELLO`
   makes no sense. Any command other than `HELLO` in the CONNECTED state is
   rejected with `425 NOT_READY`.

2. **SUB and UNSUB are idempotent.** Subscribing to a topic already subscribed
   returns `201 SUBSCRIBED` again, with the same schema, rather than an error.
   Unsubscribing from a topic not currently subscribed returns `202
   UNSUBSCRIBED` rather than an error. This keeps client logic simple: the client
   states the desired end state and does not have to track exactly what it is
   already subscribed to.

3. **BYE is optional.** A client may end the session with `BYE`, but simply
   closing the TCP connection releases the session and all its subscriptions the
   same way. The server must handle an abrupt disconnection regardless, so `BYE`
   is a courtesy, not a requirement.

4. **The session id is informational.** `HELLO` is acknowledged with `200 OK
   session=<int>`. The client is not required to use this value; it exists mainly
   to correlate a client with its entries in the server logs.

<br>

### 3.6 Example session

Lines starting with `C:` are sent by the client, lines starting with `S:` by the
server. The trailing `CRLF` on each line is not shown.

```
C: HELLO udp_port=41000
S: 200 OK session=7
C: TOPICS
S: 210 TOPICS AAPL:FINANCE Paris:WEATHER capteur-A:SENSOR
C: SUB AAPL
S: 201 SUBSCRIBED topic_id=2 type=FINANCE fields=[price]
C: SUB Paris
S: 201 SUBSCRIBED topic_id=5 type=WEATHER fields=[temp,wind,pressure]
C: ALERT AAPL price > 500.00
S: 203 ALERT_SET id=3
S: 300 ALERT id=3 AAPL price 501.20 > 500.00
C: UNSUB Paris
S: 202 UNSUBSCRIBED Paris
C: BYE
S: 200 OK
```

After the two `SUB` replies, the client knows that `topic_id=2` carries a single
`price` field and `topic_id=5` carries `temp`, `wind` and `pressure` in that
order. It can now interpret the numeric values arriving in the UDP datagrams,
which reference topics by their `topic_id` rather than by name.

<br>

## 4. Data channel (UDP)

