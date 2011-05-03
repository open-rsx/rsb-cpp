
import subprocess
from optparse import OptionParser

parser = OptionParser()
parser.add_option("--exe", help="The executable to launch", metavar="program")
parser.add_option("--name", help="Name of the test", metavar="string")
parser.add_option("--xml", help="The junit xml file to write", metavar="filename")

(options, args) = parser.parse_args()

if not options.exe or not options.name or not options.xml:
    print("illegal arguments")
    exit(1)

errorXml = '''<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="1" failures="1" disabled="0" errors="0" time="0" name="%(name)s">
  <testsuite name="%(name)s" tests="1" failures="1" disabled="0" errors="0" time="0">
    <testcase name="%(name)s" status="run" time="0" classname="%(name)s">
      <failure message="Failed" type="">Program %(prog)s %(args)s did not die.</failure>
    </testcase>
  </testsuite>
</testsuites>
''' % {"name" : options.name, "prog" : options.exe, "args" : args}

goodXml = '''<?xml version="1.0" encoding="UTF-8"?>
<testsuites tests="1" failures="0" disabled="0" errors="0" time="0" name="%(name)s">
  <testsuite name="%(name)s" tests="1" failures="0" disabled="0" errors="0" time="0">
    <testcase name="%(name)s" status="run" time="0" classname="%(name)s" />
  </testsuite>
</testsuites>
''' % {"name" : options.name, "prog" : options.exe, "args" : args}

print("Calling %s" % ([options.exe] + args))
retcode = subprocess.call([options.exe] + args)
print("retcode=%s" % retcode)

f = open(options.xml, "w")

if retcode == 0:
    print("Error, no death")
    f.write(errorXml)
    f.close()
    exit(1)
else:
    print("Successful death")
    f.write(goodXml)
    f.close()
    exit(0)
