MalSiteBlocker
====

MalSiteBlocker - Block Malformed Site Application

## Key Features:
    Block malformed site on kernel level.
	Load black list from file

## Installation
    1. Compile ndis filter driver and user-level application
	2. Boot your computer with "Disable driver signature enforcement" option.
	3. Install driver.
	    3-1. network adapter attribute
		3-2. install -> add service
		3-3. from disk
		3-4. select .\MalSiteBlocker\Release\x64\ndislwf\ndislwf.inf
	4. execute MalSiteBlocker.exe
	5. load mal site list from file "black_list.txt"
	6. enable filter.