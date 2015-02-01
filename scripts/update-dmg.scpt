-- adaptation of https://gist.github.com/asmaloney/55d96a8c3558b2f92cb3

on run argv

tell application "Finder"
  tell disk (item 1 of argv)
    open
    set current view of container window to icon view
    set toolbar visible of container window to false
    set statusbar visible of container window to false
    set the bounds of container window to {400, 100, 1125, 400}
    set viewOptions to the icon view options of container window
    set arrangement of viewOptions to not arranged
    set icon size of viewOptions to 148
    set background picture of viewOptions to file ".background:background.png"
    set position of item (item 2 of argv) of container window to {160, 145}
    set position of item "Applications" of container window to {550, 145}
    update without registering applications
    close

    open
    update without registering applications
    
    -- learned from: https://github.com/andreyvit/create-dmg
    -- give the finder some time to write the .DS_Store file
    delay 3
		set waitTime to 0
		set ejectMe to false
    set dsStore to "\"" & (item 1 of argv) & "/.DS_STORE\""
		repeat while ejectMe is false
			delay 1
			set waitTime to waitTime + 1
	
			if (do shell script "[ -f " & dsStore & " ]; echo $?") = "0" then set ejectMe to true
		end repeat
		log "waited " & waitTime & " seconds for .DS_STORE to be created."
  end tell
end tell

end run
