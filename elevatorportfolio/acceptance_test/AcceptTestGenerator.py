# ###################################
# 18-649 Fall 2015
#
# Group 12
# ---------------------------
# Daniel Gorziglia - dgorzigl
# Shepard Emerson - semerson
# Tom Eliot - tke
# Daniel Haddox - dhaddox
# ####################################

import os
import sys
import re
import subprocess
import random
import time
import copy
from collections import Counter

####################################
# Constants
####################################

BUILDING = [
  # FRONT BACK
  (False, False), # Floor 0
  (True, True),   # Floor 1
  (False, True),  # Floor 2
  (True, False),  # Floor 3
  (True, False),  # Floor 4
  (True, False),  # Floor 5
  (True, False),  # Floor 6
  (True, True),   # Floor 7
  (True, False),  # Floor 8
]

SOURCE_FLOOR_WEIGHTS = [
  # WEIGHT # FLOOR NUM
  (1,     "1"),  # Floor 1
  (1,     "2"),  # Floor 2
  (1,     "3"),  # Floor 3
  (1,     "4"),  # Floor 4
  (1,     "5"),  # Floor 5
  (1,     "6"),  # Floor 6
  (1,     "7"),  # Floor 7
  (1,     "8")  # Floor 8
]

DEST_FLOOR_WEIGHTS = [
  # WEIGHT # FLOOR NUM
  (1,     "1"),  # Floor 1
  (1,     "2"),  # Floor 2
  (1,     "3"),  # Floor 3
  (1,     "4"),  # Floor 4
  (1,     "5"),  # Floor 5
  (1,     "6"),  # Floor 6
  (1,     "7"),  # Floor 7
  (1,     "8")  # Floor 8
]

####################################
# Test Generation
####################################

def getKey(item):
  return item[0]

# Weighted random algorithm taken from andrew cooke on StackOverFlow
# http://stackoverflow.com/questions/14992521/python-weighted-random
def weighted_random(pairs):
    total = sum(pair[0] for pair in pairs)
    r = random.randint(1, total)
    for (weight, value) in pairs:
        r -= weight
        if r <= 0: return value

def pickHallway(floor):
  floorTuple = BUILDING[floor]
  minBound = 0
  maxBound = 1

  if floorTuple[0] == False:
    minBound = 1
  elif floorTuple[1] == False:
    maxBound = 0

  hallway = random.randint(minBound, maxBound)

  if hallway == 0:
    return "FRONT"
  elif hallway == 1:
    return "BACK"

def pickFloorFromSource(sourceFloor, destinations):
  destinationList = destinations.elements();
  validDestList = list(destinationList)

  validDestList = map(int, validDestList)

  # Remove all instances of source floor from destination list
  #validDestList = [item for item in validDestList if item != sourceFloor]

  # Edge case where distribution left use with no valid destination floors
  if len(validDestList) == 0:
    destFloor = len(BUILDING) - sourceFloor
    return destFloor

  random.shuffle(validDestList)

  # Pick a floor at random
  destFloor = random.choice(validDestList)

  # Decrement count from selected destination floor
  destinations[str(destFloor)] -= 1;

  return destFloor

def generateTest(config, nameSeed):
  # Create params for test
  numOfPass = config['NUM_OF_PASSENGERS']
  timeForTest = random.randint(config['MIN_TIME'], config['MAX_TIME'])
  divisions = timeForTest / numOfPass
  timeOffset = 0
  filename = "genAcceptance_" + str(nameSeed) + ".pass"

  # Generate source and destination floor list based off of distribution
  sources = Counter(weighted_random(config["SOURCE_FLOOR_WEIGHTS"])
              for _ in range(numOfPass))
  destinations = Counter(weighted_random(config["DEST_FLOOR_WEIGHTS"])
              for _ in range(numOfPass))

  sourceList = sources.elements();
  sourceList = list(sourceList)
  sourceList.sort()

  # Randomize source floors
  random.shuffle(sourceList)

  target = open(filename, "w+")

  headerString = ('; 18-649 Fall 2015\n'
    ';\n'
    '; Group 12\n'
    '; ---------------------------\n'
    '; Daniel Gorziglia - dgorzigl\n'
    '; Shepard Emerson - semerson\n'
    '; Tom Eliot - tke\n'
    '; Daniel Haddox - dhaddox\n'
    ';\n'
    '; Acceptance Test: ' + filename + '\n\n')

  target.write(headerString)
  target.write(";Time StartFloor StartHallway EndFloor EndHallway\n")

  # Loop through source floors
  for i, sourceFloor in enumerate(sourceList):
    # Get source and destination floors and hallways
    sourceFloor = int(sourceFloor)
    destFloor = pickFloorFromSource(sourceFloor, destinations)
    sourceHallway = pickHallway(sourceFloor)
    destHallway = pickHallway(destFloor)

    line = str(timeOffset) + "s " + str(sourceFloor) + " " + sourceHallway;
    line += " " + str(destFloor) + " " + destHallway;

    target.write(line)
    target.write("\n")

    timeOffset = (i+1) * divisions

  target.close()

  return filename

def generateTests(config):
  numOfTests = config["NUM_OF_TESTS"]
  fileList = []

  # Loop through number of tests to generate
  for i in xrange(numOfTests):
    nameSeed = int(time.time()) + i
    filename = generateTest(config, nameSeed)
    fileList.append(filename)
    print ("Generated " + filename)

  name = raw_input("Please enter a name for test list: ")
  saveFilename = "generated/" + name + ".txt"
  target = open(saveFilename, "w+")

  for file in fileList:
    target.write(file)
    target.write("\n")

  target.close()

  print "Test list saved to " + saveFilename

  return saveFilename

####################################
# Utility
####################################

def createTableHeader():
  target = open("generated/table.html", "w+")

  tableString = ('<table style="text-align: left; width: 100%;" border="1" cellpadding="2" cellspacing="2">\n'
    "<tr>\n"
      ' <td style="vertical-align: top;">Acceptance Test File</td>\n'
      ' <td style="vertical-align: top;">Description</td>\n'
      ' <td style="vertical-align: top;">Test Result</td>\n'
      ' <td style="vertical-align: top;">Random Seed Used</td>\n'
      ' <td style="vertical-align: top;">Delivery Performance Score</td>\n'
      ' <td style="vertical-align: top;">Satisfaction Performance Score</td>\n'
      ' <td style="vertical-align: top;">Test Results</td>\n'
    "</tr>\n")
  target.write(tableString)
  target.close()

def addRowToTable(result, seed, filename, errString, i):
  target = open("generated/table.html", "a")

  if len(errString):
    testResult = "Failed"
  else:
    testResult = "Passed"

  re1='(Satisfaction_performance_score)'  # Variable Name 1
  re2='.*?' # Non-greedy match on filler
  re3='([+-]?\\d*\\.\\d+)(?![-+0-9\\.])'  # Float 1

  rg = re.compile(re1+re2+re3,re.IGNORECASE|re.DOTALL)
  m = rg.search(result)
  if m:
    performance_score = str(m.group(2))
  else:
    performance_score = "N/A"

  re1='(Delivery_performance_score)'  # Variable Name 1
  re2='.*?' # Non-greedy match on filler
  re3='([+-]?\\d*\\.\\d+)(?![-+0-9\\.])'  # Float 1

  rg = re.compile(re1+re2+re3,re.IGNORECASE|re.DOTALL)
  m = rg.search(result)
  if m:
    delivery_performance = str(m.group(2))
  else:
    delivery_performance = "N/A"

  tableString = ("<tr>\n"
      ' <td style="vertical-align: top;"><a href="' + filename + '">' + filename + '</a></td>\n'
      ' <td style="vertical-align: top;">Generated Test</td>\n'
      ' <td style="vertical-align: top;">' + testResult + '</td>\n'
      ' <td style="vertical-align: top;">' + str(seed) + '</td>\n'
      ' <td style="vertical-align: top;">' + delivery_performance + '</td>\n'
      ' <td style="vertical-align: top;">' + performance_score + '</td>\n'
      ' <td style="vertical-align: top;"><a href="elevator-' + filename + '-' + str(i) + '.stats">' + 'elevator-' + filename + '-' + str(i) + '.stats</a></td>\n'
    "</tr>\n")
  target.write(tableString)
  target.close()

def closeTable():
  target = open("generated/table.html", "a")

  tableString = "</table>"
  target.write(tableString)
  target.close()
  print "Saved HTML table to generated/table.html"

####################################
# Run Tests
####################################

def extractResults(result):
  ## Extract runtime req violations

  re1='.*?' # Non-greedy match on filler
  re2='(Count)' # Word 1
  re3='.*?' # Non-greedy match on filler
  re4='(\\d+)'  # Integer Number 1

  errString = ""

  rg = re.compile(re1+re2+re3+re4,re.IGNORECASE|re.DOTALL)
  m = rg.findall(result)
  if m:
    if int(m[1][1]) > 1: # Temp fix, issue with runtime montior saying error on startup
      errString += m[1][1] + " Bad Stop Violations \n"
    if int(m[2][1]) > 0:
      errString += m[2][1] + " Bad Door Openings Violations \n"
    if int(m[3][1]) > 0:
      errString += m[3][1] + " Bad Door Nudge Violations \n"
    if int(m[4][1]) > 0:
      errString += m[4][1] + " Drive Not Fast Enough Violations \n"
    if int(m[5][1]) > 0:
      errString += m[5][1] + " Lantern Not On With Calls Violations \n"
    if int(m[6][1]) > 0:
      errString += m[6][1] + " Lantern Switched Direciton Violations \n"
    if int(m[7][1]) > 0:
      errString += m[7][1] + " Dispatch opposite direction of lantern with calls Violations \n"

  ## Extract Delivery Summary

  re1='.*?' # Non-greedy match on filler
  re2='(Stranded)' # Word 1
  re3='.*?' # Non-greedy match on filler
  re4='(\\d+)'  # Integer Number 1

  rg = re.compile(re1+re2+re3+re4,re.IGNORECASE|re.DOTALL)
  m = rg.findall(result)
  if m:
    if int(m[0][1]) > 0:
      errString += m[0][1] + " Stranded Passengers \n"

  return errString

def runTest(filename, seed, config, i):
    print "Running " + filename + " with seed " + str(seed) + "...",
    execCmd = "java -cp ../../code/ simulator.framework.Elevator "
    execCmd += "-pf " + filename + " -monitor RuntimeRequirementsMonitor -b 200 -fs 5.0 -seed " + str(seed) + " -head generated/header.txt"

    try:
      result = subprocess.check_output(execCmd.split())
    except subprocess.CalledProcessError:
      print "Java error..."
      print "Printing executed command..."
      print execCmd
      if (config["GEN_TABLE"]):
        addRowToTable("FAILURE", seed, filename, "FAILURE", i)
      return

    trimmedResult = result[-1000:]

    errString = extractResults(trimmedResult)

    if (config["GEN_TABLE"]):
      addRowToTable(trimmedResult, seed, filename, errString, i)

    if len(errString):
      print "Error with test"
      print errString

      # Save captured result to file
      errFileName = "generated/error-" + str(seed) + '.txt'
      target = open(errFileName, "w+")
      target.write(result)
      target.write("\nExecuted command\n")
      target.write(execCmd)
      target.close()
      print "Saved generated output to " + errFileName

      if not config['SILENT_MODE']:
        response = raw_input("Would you like to run this test again with the GUI? [Y]: ")
        if (len(response) == 0 or response == "Y"):
          execCmd += " -gui"
          try:
            proc = subprocess.call (execCmd.split())
          except subprocess.CalledProcessError:
            print "ERROR"

        print "Printing executed command..."
        print execCmd

    else:
      print "Test passed"

def runTests(config):
  print "Beginning run tests command..."

  with open(config["RUN_TESTS_FILE"]) as f:
    fileList = f.read().splitlines()

  print "Running tests from " + config['RUN_TESTS_FILE']

  n = raw_input("How many times should each test run? ")
  n = int(n)
  dontCare = raw_input("Press ENTER to begin testing: ")

  # If enabling table, create file
  if (config["GEN_TABLE"]):
    createTableHeader()

  for filename in fileList:
    for i in range(n):
      seed = int(time.time()) + i
      runTest(filename, seed, config, i)

  if (config["GEN_TABLE"]):
    closeTable()

####################################
# Script Setup and Init
####################################

def runCommand(config):
  if config["COMMAND"] == "gen":
    config["RUN_TESTS_FILE"] = generateTests(config)
    response = raw_input("Would you like to run the generated tests? [Y]: ")
    if (len(response) == 0 or response == "Y"):
      runTests(config)

  if config["COMMAND"] == "run":
    runTests(config)

def adjustWeights(config):
  print("Distribution adjustment wizard.")

  print("SOURCE FLOORS")
  for i, floor in enumerate(config["SOURCE_FLOOR_WEIGHTS"]):
    weight = raw_input("Enter weight for source floor " + floor[1] + ": ")
    weight = int(weight)
    config["SOURCE_FLOOR_WEIGHTS"][i] = (weight, floor[1])

  print("DESTINATION FLOORS")
  for i, floor in enumerate(config["DEST_FLOOR_WEIGHTS"]):
    weight = raw_input("Enter weight for destination floor " + floor[1] + ": ")
    weight = int(weight)
    config["DEST_FLOOR_WEIGHTS"][i] = (weight, floor[1])

  print("Disributions updated...")

def main():
  hasP = False

  config = {
    "NUM_OF_PASSENGERS" : 50,
    "RUN_TESTS" : False,
    "MIN_TIME" : 10,
    "MAX_TIME" : 100,
    "COMMAND" : "gen",
    "NUM_OF_TESTS" : 1,
    "SOURCE_FLOOR_WEIGHTS" : SOURCE_FLOOR_WEIGHTS,
    "DEST_FLOOR_WEIGHTS" : DEST_FLOOR_WEIGHTS,
    "RUN_TESTS_FILE" : "",
    "SILENT_MODE" : False,
    "GEN_TABLE" : False
  }

  args = sys.argv

  if (len(args) < 2):
    printHelp()
    sys.exit(0)

  # Properly capture arguments
  for i, arg in enumerate(args):
    if (arg == '--help'):
      printHelp()
      sys.exit(0)

    if (arg == '--gen'):
      config['COMMAND'] = "gen"

    if (arg == '--run'):
      # Ignore p flag if trying to run
      hasP = True
      config['COMMAND'] = "run"
      if i+1 < len(args):
        value = args[i+1]
        config["RUN_TESTS_FILE"] = value;
      else:
        errorAndDie("Filename not defined.")

    # Number of passengers
    if (arg == '-p'):
      if i+1 < len(args):
        value = int(args[i+1])
        if (value == 0):
          errorAndDie("Number of passengers cannot be 0.")
        config["NUM_OF_PASSENGERS"] = value;
        hasP = True
      else:
        errorAndDie("Number of passengers not defined.")

    # Number of tests
    if (arg == '-n'):
      if i+1 < len(args):
        value = int(args[i+1])
        if (value == 0):
          errorAndDie("Number of tests cannot be 0.")
        config["NUM_OF_TESTS"] = value;
      else:
        errorAndDie("Number of tests not defined.")

    # Min time
    if (arg == '-minTime'):
      if i+1 < len(args):
        value = int(args[i+1])
        if (value == 0):
          errorAndDie("Min time cannot be 0.")
        config["MIN_TIME"] = value;
      else:
        errorAndDie("Min time not defined.")

    # Max time
    if (arg == '-maxTime'):
      if i+1 < len(args):
        value = int(args[i+1])
        if (value == 0):
          errorAndDie("Max time cannot be 0.")
        config["MAX_TIME"] = value;
      else:
        errorAndDie("Max time not defined.")

    # Distribution adjustment
    if (arg == '-adj'):
      adjustWeights(config)

    # Silent
    if (arg == '-silent'):
      config["SILENT_MODE"] = True

    if (arg == "-genTable"):
      config["GEN_TABLE"] = True

  # Make some argument checks
  if not hasP:
    errorAndDie("-p flag required.")
  if config["MAX_TIME"] < config["MIN_TIME"]:
    errorAndDie("Max time cannot be less than min time.")

  # Change time to seconds
  config["MIN_TIME"] = config["MIN_TIME"] * 60
  config["MAX_TIME"] = config["MAX_TIME"] * 60

  # Run script
  runCommand(config)

def printHelp():
  print("USAGE")
  print("   python AcceptanceTestGenerator.py [command] [options]")
  print("")
  print("EXAMPLE")
  print("   python AcceptanceTestGenerator.py -p 50")
  print("")
  print("COMMAND")
  print("   --help")
  print("   Displays this help message.")
  print("")
  print("   --run <testListFilename>")
  print("   Runs the generated tests listed in the test list filename.")
  print("   If you run the --gen command, the script will ask if you would ")
  print("   like to run the generated tests.")
  print("")
  print("   --gen")
  print("   Generates the tests. This is the default command")
  print("")
  print("OPTIONS")
  print("   -p <number>")
  print("   REQUIRED. Sets the number of passengers for the tests.")
  print("")
  print("   -n <number>")
  print("   Number of tests to generate.")
  print("   Default is set to 1.")
  print("")
  print("   -minTime <numberOfMinutes>")
  print("   The minimum number of minutes to run the test.")
  print("   Default is set to 10 minutes.")
  print("")
  print("   -maxTime <numberOfMinutes>")
  print("   The maximum number of minutes to run the test.")
  print("   Default is set to 60 minutes.")
  print("")
  print("   -silent")
  print("   If used, will not ask for input when running tests.")
  print("")
  print("   -adj")
  print("   Adjust distribution for source and destination floors.")
  print("   If this option is used, an interactive wizard will appear allowing")
  print("   you to adjust the distribution used for generating source and destination")
  print("   floors. The default weights for all floors is 1.")
  print("")
  print("   -genTable")
  print("   If used, the script will generate an HTML table of acceptance test results.")

def errorAndDie(message):
  print("ERROR")
  print(message)
  print("")
  print("Exiting...")
  sys.exit(-1)


if __name__ == "__main__":
    main()