on run {targetBuddyPhone, targetMessage}

tell application "Location Helper"
	set listCoords to get location coordinates
	set location to (item 1 of listCoords as text) & ", " & (item 2 of listCoords) as text
end tell

tell application "Messages"
    set targetService to 1st service whose service type = iMessage
    set targetBuddy to buddy targetBuddyPhone of targetService
    send targetMessage to targetBuddy
    send location to targetBuddy
end tell

end run