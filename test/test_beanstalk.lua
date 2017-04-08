local beanstalk = require("beanstalk")

print(beanstalk.version())
for k, v in pairs(beanstalk) do
  print(k, v)
end
local client, ok, res, job, yaml
print("* test client")
client = beanstalk.client("localhost", 11300)
assert(client)
print("* test client:use")
ok, res = client:use("test_tube")
assert(ok)
print("* test client:watch")
ok, res = client:watch("test_tube")
assert(ok)
print("* test client:put")
ok, res = client:put("some job", 1, 2, 3)
assert(ok)
print("* test client:reserve - without timeout")
ok, job = client:reserve()
assert(ok)
assert(job.id ~= nil)
print("* test client:reserve - without timeout")
ok, job = client:reserve(1)
print("* test job:delete")
assert(job:delete())
print("* test job:bury")
ok, res = client:put("buried job")
assert(ok)
ok, job = client:reserve()
assert(ok)
assert(job:bury())
print("* test client:peek_buried")
ok, job = client:peek_buried()
assert(ok)
job:delete()
print("* test job:release")
assert(client:put("delayed job"))
ok, job = client:reserve()
assert(job:release())
print("* test client:peek_delayed")
ok, job = client:peek_delayed()
if ok then assert(job:delete()) end
assert(client:put("another job", 1, 1, 1))
print("* test job:touch")
ok, job = client:reserve()
assert(job:touch())
print("* test job:touch")
print("* test client:list_tube_used")
ok, yaml = client:list_tube_used()
assert(ok and yaml:find("test_tube"))
print("* test client:list_tubes")
ok, yaml = client:list_tubes()
assert(ok and yaml:find("default"))
print("* test client:stats")
ok, yaml = client:stats()
assert(ok and yaml:find("pid"))
print("* test client:stats_job")
ok, yaml = client:stats_job(job.id)
assert(ok and yaml:find(job.id)) 
job:delete()
print("* test job:delete on a not found job")
ok, res = job:delete()
assert(not ok and (res == beanstalk.STATUS_NOT_FOUND))
print("* test beanstalk.status_text")
local ok, status = client:watch("check_status_tube")
assert(ok and beanstalk.status_text(status) == "Success")
