lua-beanstalk
===

Lua binding for [beanstalk-client](https://github.com/deepfryed/beanstalk-client).
See protocol spec [beanstalk-spec](https://github.com/kr/beanstalkd/blob/master/doc/protocol.txt) for more information about Beanstalk.

Example
===
```lua
local beanstalk = require("beanstalk")
loca client = beanstalk.client("localhost", 11300)
assert(client)
local ok, res, job
ok, res = client:watch("lua-tube")
ok, res = client:use("lua-tube")
ok, res = client:put("lua-job")
ok, job = client:reserve()
print(job.data) -- "lua-job"
```

Doc
===


version = beanstalk.version()
==
Get the version of the beanstalk-client.

Returns:
* **version** (str)

status = beanstalk.status_text(code)
==
Get the string representation of the a status code.

Params:
* **code** (int) - Status code.

Returns:
* **status** (string) - Status text.

Status constants
==
The status codes returned by the functions below, can use _beanstalk.status_text(code)_ to retreive the text.

* beanstalk.STATUS_OK
* beanstalk.STATUS_FAIL
* beanstalk.STATUS_EXPECTED_CRLF
* beanstalk.STATUS_JOB_TOO_BIG
* beanstalk.STATUS_DRAINING
* beanstalk.STATUS_TIMED_OUT
* beanstalk.STATUS_NOT_FOUND
* beanstalk.STATUS_DEADLINE_SOON
* beanstalk.STATUS_BURIED
* beanstalk.STATUS_NOT_IGNORED

beanstalk.client(host, port, [timeout])
==
* **host** (string) - Beanstalkd host
* **port** (number) - Beanstalkd port
* **timeout** (number, optional) - Timeout in msecs

Returns
* **c** (client) - Returns client instance on success, throws an error on failure

client
==
Properties:
* **addr** (string) - host:port (for information only)

client:disconnect()
==
Closes the connection if open

ok, status = client:use(tube)
==
Use tube.

Params:
* **tube** (string) - Tube name

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = client:watch(tube)
==
Watch tube.

Params:
* **tube** (string) - Tube name

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = client:ignore(tube)
==
Ignore tube.

Params:
* **tube** (string) - Tube name

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = client:put(data, [priority, delay, ttr]
==
Put a job in the used tube.

Params:
* **data** (string) - Job text
* **priority** (int) - Priority, default: 0
* **delay** (int) - Delay, default: 0
* **ttr** (int) - TTR (time-to-run), default: 120

ok, job = client:reserve([timeout])
==
Reserve a job.

Params:
* **timeout** (float, optional) - Reserve a job

Returns:
* **ok** (boolean) - If the action was successful
* **job** (beanstalk.job/status code) - Returns a job instance or the status code if failed.

ok, job = client:peek(id)
==
Peek a job for inspection.

Params:
* **id** (int) - Job ID 

Returns:
* **ok** (boolean) - If the action was successful
* **job** (beanstalk.job/status code) - Returns a job instance or the status code if failed.

ok, job = client:peek_ready()
==
Return the next ready job.

Params:

Returns:
* **ok** (boolean) - If the action was successful
* **job** (beanstalk.job/status code) - Returns a job instance or the status code if failed.

ok, job = client:peek_delayed()
==
Return the delayed job with the shortest delay left.

Params:

Returns:
* **ok** (boolean) - If the action was successful
* **job** (beanstalk.job/status code) - Returns a job instance or the status code if failed.

ok, job = client:peek_buried()
==
Return the next job in the list of buried jobs. 

Params:

Returns:
* **ok** (boolean) - If the action was successful
* **job** (beanstalk.job/status code) - Returns a job instance or the status code if failed.

ok, status = client:kick(bound)
==
Move jobs into the ready queue.

Params:
* **bound** (int) - Upper bound of jobs to kick.

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, tube = client:list_tube_used()
==
Get the tube currently being used by the client.

Returns:
* **ok** (boolean) - If the action was successful.
* **tube** (string/int) - Tube name or response status code.

ok, tubes = client:list_tubes()
==
List of all existing tubes.

Returns:
* **ok** (boolean) - If the action was successful.
* **tubes** (string/int) - YAML file with a list of tubes.

ok, tubes = client:list_tubes_watched()
==
List tubes currently being watched by the client.

Returns:
* **ok** (boolean) - If the action was successful.
* **tubes** (string/int) - YAML string with a list of watched tubes.

ok, stats = client:stats()
==
Statistical information about the system as a whole.

Returns:
* **ok** (boolean) - If the action was successful.
* **stats** (string/int) - a YAML string with statistical information represented a dictionary 

ok, stats = client:stats_job(id)
==
Statistical information about the specified job if it exists.

Params:
* **id** - Job ID.

Returns:
* **ok** (boolean) - If the action was successful.
* **stats** (string/int) - a YAML string of statistical information about the specified job if
it exists.


ok, stats = client:stats_tube(tube)
==
Statistical information about the specified tube if it exists.

Params:
* **tube** (string) - Tube name.
Returns:
* **ok** (boolean) - If the action was successful.
* **stats** (string/int) - a YAML string of statistical information about the specified tube
if it exists.

job
==
Properties:
* **id** (int) - Job ID
* **data** (string) - Job Data.

ok, status = job:delete()
==
Delete a job.

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = job:release()
==
Release a job and put back in the ready queue.

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = job:bury()
==
Bury a job.

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code

ok, status = job:touch()
==
Touch a job and ask for more time.

Returns:
* **ok** (boolean) - If the action was successful
* **status** (int) - Status code




