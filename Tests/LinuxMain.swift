import XCTest

import wyrdTests

var tests = [XCTestCaseEntry]()
tests += wyrdTests.allTests()
XCTMain(tests)