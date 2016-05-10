import os
import sys
import re
import subprocess

UNIT_TEST_PATH = "../unit_test/"
INTEGRATION_TEST_PATH = "../integration_test/"

verboseMode = False
outputMode = False

unitTestMap = dict()
integrationTestMap = dict()

def getTests(dirName):
  # Holds map of integration tests to files
  mList = []
  testMap = dict()

  files = os.listdir(dirName)

  for f in files:
    filename, file_extension = os.path.splitext(f)

    if file_extension == ".cf":
      testMap[filename] = []

    if file_extension == ".mf":
      mList.append(filename)

  for m in mList:
    c = re.search("^[^_]*", m).group(0)
    if c in testMap:
      testMap[c].append(m)

  return testMap

def extractPassed(result):
  re1='.*?' # Non-greedy match on filler
  re2='(Passed)'  # Word 1
  re3='(:)' # Any Single Character 1
  re4='(\\s+)'  # White Space 1
  re5='(\\d+)'  # Integer Number 1

  rg = re.compile(re1+re2+re3+re4+re5,re.IGNORECASE|re.DOTALL)
  m = rg.search(result)
  if m:
    return m.group(4)
  return 0

def extractFailed(result):
  re1='.*?' # Non-greedy match on filler
  re2='(Failed)'  # Word 1
  re3='(:)' # Any Single Character 1
  re4='(\\s+)'  # White Space 1
  re5='(\\d+)'  # Integer Number 1

  rg = re.compile(re1+re2+re3+re4+re5,re.IGNORECASE|re.DOTALL)
  m = rg.search(result)
  if m:
    return m.group(4)
  return 0

def runTest(type, cfName, mfName, verbose):
  execCmd = "java -cp ../../code/ simulator.framework.Elevator "

  execCmd += "-cf ../" + type + "/" + cfName + ".cf "
  execCmd += "-mf ../" + type + "/" + mfName + ".mf"

  print("Running test " + mfName)

  try:
    result = subprocess.check_output(execCmd.split())
  except subprocess.CalledProcessError:
    print "ERROR"
    return (0, 0, 0)

  if verbose:
    print result

  passed = int(extractPassed(result[-50:]))
  failed = int(extractFailed(result[-50:]))

  print "Passed: " + str(passed) + " Failed: " + str(failed) + " Total: " + str(passed + failed)

  return (passed, failed, passed+failed)

def runUnitTests():
  global unitTestMap

  print("Running all unit tests...")

  passed = 0
  failed = 0
  total = 0

  tupleResults = []

  for c, tests in unitTestMap.iteritems():
    for m in tests:
      tupleResults.append(runTest("unit_test", c, m, False))

  for p, f, t in tupleResults:
    passed += p
    failed += f
    total += t

  print "======================"
  print "Unit Test Results"
  print "======================"
  print "Passed: " + str(passed)
  print "Failed: " + str(failed)
  print "Total: " + str(total)
  print ""

def runIntegrationTests():
  global integrationTestMap

  print("Running all integration tests...")

  passed = 0
  failed = 0
  total = 0

  tupleResults = []

  for c, tests in integrationTestMap.iteritems():
    for m in tests:
      tupleResults.append(runTest("integration_test", c, m, False))

  for p, f, t in tupleResults:
    passed += p
    failed += f
    total += t

  print "======================"
  print "Integration Test Results"
  print "======================"
  print "Passed: " + str(passed)
  print "Failed: " + str(failed)
  print "Total: " + str(total)
  print ""

def generateUnitTestSummary():
  lines = []

  for c, tests in unitTestMap.iteritems():
    for m in tests:
      if c == "hallbuttoncontrol":
        lines.append("HallButtonControl " + c + ".cf " + m + ".mf \n")
      if c == "carbuttoncontrol":
        lines.append("CarButtonControl " + c + ".cf " + m + ".mf \n")
      if c == "lanterncontrol":
        lines.append("LanternControl " + c + ".cf " + m + ".mf \n")
      if c == "carpositioncontrol":
        lines.append("CarPositionControl " + c + ".cf " + m + ".mf \n")
      if c == "dispatcher":
        lines.append("Dispatcher " + c + ".cf " + m + ".mf \n")
      if c == "doorcontrol":
        lines.append("DoorControl " + c + ".cf " + m + ".mf \n")
      if c == "drivecontrol":
        lines.append("DriveControl " + c + ".cf " + m + ".mf \n")

  lines.sort()

  summaryFile = open("../unit_test/unit_tests.txt", "w")

  summaryFile.writelines(lines)
  summaryFile.close()

  print "Updated Unit Test Summary File"

def generateIntegrationTestSummary():
  lines = []

  for c, tests in integrationTestMap.iteritems():
    for m in tests:
      lines.append(c[:-1].upper() + " " + c + ".cf " + m + ".mf \n")

  lines.sort()

  summaryFile = open("../integration_test/integration_tests.txt", "w")

  summaryFile.writelines(lines)
  summaryFile.close()

  print "Updated Integration Test Summary File"

def runSingleTest(testName, type):
  c = re.search("^[^_]*", testName).group(0)
  runTest(type, c, testName, type)

def runAcceptanceTest(testName):
  execCmd = "java -cp ../../code/ simulator.framework.Elevator "

  execCmd += "-pf ../" + "acceptance_test" + "/" + testName + ".pass -gui -fs 5.0"

  print("Running test " + testName)

  try:
    proc = subprocess.call (execCmd.split())
  except subprocess.CalledProcessError:
    print "ERROR"

def main():
  args = sys.argv

  global unitTestMap
  global integrationTestMap
  global outputMode

  # Read all tests from folders
  unitTestMap = getTests(UNIT_TEST_PATH)
  integrationTestMap = getTests(INTEGRATION_TEST_PATH)

  if len(args) == 2 and args[1] == "gis":
    generateIntegrationTestSummary()
    return
  if len(args) == 2 and args[1] == "gus":
    generateUnitTestSummary()
    return

  # if "-v" in args:
  #   print("Verbose mode enabled")

  if len(args) == 1:
    print("Running all tests...")
    runUnitTests()
    runIntegrationTests()
  elif len(args) == 2:
    if args[1] == "i":
      runIntegrationTests()
    if args[1] == "u":
      runUnitTests()
    if args[1] == "o":
      outputMode = True
      runUnitTests()
      runIntegrationTests()
  elif len(args) == 3:
    if args[1] == "i" and args[2] != "o":
      runSingleTest(args[2], "integration_test")
    if args[1] == "u" and args[2] != "o":
      runSingleTest(args[2], "unit_test")      
    if args[1] == "a":
      runAcceptanceTest(args[2])
    if args[1] == "u" and args[2] == "o":
      outputMode = True
      runUnitTests()
    if args[1] == "i" and args[2] == "o":
      outputMode = True
      runIntegrationTests()
  elif len(args) == 4 and args[3] == 'o':
    outputMode = True
    if args[1] == "i":
      runSingleTest(args[2], "integration_test")
    if args[1] == "u":
      runSingleTest(args[2], "unit_test")


  # remove stats files
  filelist = [ f for f in os.listdir(".") if f.endswith(".stats") ]
  if not outputMode:
    for f in filelist:
      os.remove(f)
  #rename stats files, append headers, move to appropriate folders
  else:
    for f in filelist:
      mf = f.find(".mf")
      newf = f[10:mf];
      with file(f, 'r') as original: data = original.read()
      os.remove(f)
      with file("header.txt", 'r') as headerfile: header = headerfile.read()
      if newf.find("sd") == 0:
        with file("../integration_test/" + newf + ".stats", 'w') as modified: modified.write(header + "; " + newf + " Integration Test\n"+ data)
      else:
        with file("../unit_test/" + newf + ".stats", 'w') as modified: modified.write(header + "; " + newf + " Unit Test\n" + data)



if __name__ == "__main__":
    main()